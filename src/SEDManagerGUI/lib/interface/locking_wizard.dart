import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/bindings/storage_device.dart';
import 'package:sed_manager_gui/bindings/value.dart';
import 'package:sed_manager_gui/interface/components/encrypted_device_builder.dart';
import 'package:sed_manager_gui/interface/components/snapshot_builder.dart';
import 'package:sed_manager_gui/interface/components/wizard.dart';

import 'components/status_page.dart';

const UID _adminSpUid = 0x0000020500000001;
const UID _lockingSpUid = 0x0000020500000002;
const UID _sidUid = 0x0000000900000006;
const UID _psidUid = 0x000000090001FF01;

class _SuccessPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return WizardPage(
      title: "Factory reset",
      onValidate: () async {},
      child: const Center(
        child: StatusPage.success(
          message: "Configuration successful! Your device is now passsword protected.",
        ),
      ),
    );
  }
}

class _UserSetupPage extends StatelessWidget {
  _UserSetupPage(this._encryptedDevice, this._sidPassword);

  final EncryptedDevice _encryptedDevice;
  final String _sidPassword;
  final _userNameController = TextEditingController();
  final _userPasswordController = TextEditingController();
  final _userRepeatController = TextEditingController();

  Widget _buildContent(BuildContext context) {
    final nameField = Tooltip(
      waitDuration: Durations.long2,
      message: "You can use this to login during pre-boot authentication.",
      child: TextField(
        controller: _userNameController,
        decoration: const InputDecoration(hintText: "Username"),
      ),
    );

    final passwordField = Tooltip(
      waitDuration: Durations.long2,
      message: "This password will be used for pre-boot authentication. It can be the same as your Owner password.",
      child: TextField(
        controller: _userPasswordController,
        decoration: const InputDecoration(hintText: "New password"),
        obscureText: true,
      ),
    );

    final repeatField = TextField(
      controller: _userRepeatController,
      decoration: const InputDecoration(hintText: "Repeat password"),
      obscureText: true,
    );

    return Center(
      child: SizedBox(
        width: 280,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            const Text("Set your credentials for pre-boot authentication:", textAlign: TextAlign.center),
            nameField,
            passwordField,
            repeatField,
          ],
        ),
      ),
    );
  }

  Future<void> _setupUser() async {
    if (_userNameController.text.isEmpty) {
      throw Exception("username cannot be empty");
    }
    if (_userPasswordController.text != _userRepeatController.text) {
      throw Exception("the two passwords don't match");
    }
    final session = await _encryptedDevice.login(_lockingSpUid);
    try {
      final admin1Uid = await _encryptedDevice.findUid("Authority::Admin1", securityProvider: _lockingSpUid);
      final user1Uid = await _encryptedDevice.findUid("Authority::User1", securityProvider: _lockingSpUid);
      final cPinUser1 = await _encryptedDevice.findUid("C_PIN::User1", securityProvider: _lockingSpUid);
      final setRdLocked = await _encryptedDevice.findUid(
        "ACE::Locking_GlobalRange_Set_RdLocked",
        securityProvider: _lockingSpUid,
      );
      final setWrLocked = await _encryptedDevice.findUid(
        "ACE::Locking_GlobalRange_Set_WrLocked",
        securityProvider: _lockingSpUid,
      );
      final mbrControl = await _encryptedDevice.findUid(
        "ACE::MBRControl_Set_DoneToDOR",
        securityProvider: _lockingSpUid,
      );
      final authorityObjectRefUid = await _encryptedDevice.findUid("Type::Authority_object_ref");
      final booleanAceUid = await _encryptedDevice.findUid("Type::boolean_ACE");
      final globalRangeUid = await _encryptedDevice.findUid("Locking::GlobalRange", securityProvider: _lockingSpUid);

      // Authenticate as Admin1.
      await session.authenticate(admin1Uid, _sidPassword);

      // Set User1's common name.
      await session.setValue(user1Uid, 2, Value.bytesFromString(_userNameController.text));

      // Set User1's password.
      await session.setValue(cPinUser1, 3, Value.bytesFromString(_userPasswordController.text));

      // Set global range and MBR locking permissions.
      final halfUid = ByteData(4);

      halfUid.setUint32(0, authorityObjectRefUid & 0xFFFFFFFF);
      final authorityObjectRefValue = Value.bytes(halfUid);
      halfUid.setUint32(0, booleanAceUid & 0xFFFFFFFF);
      final booleanAceValue = Value.bytes(halfUid);

      final fullUid = ByteData(8);
      fullUid.setUint64(0, admin1Uid);
      final admin1Value = Value.bytes(fullUid);
      fullUid.setUint64(0, user1Uid);
      final user1Value = Value.bytes(fullUid);

      final admin1OrUser1Rights = Value.list(<Value>[
        Value.named(authorityObjectRefValue, admin1Value),
        Value.named(authorityObjectRefValue, user1Value),
        Value.named(booleanAceValue, Value.integer(1, 1, false)),
      ]);

      await session.setValue(setRdLocked, 3, admin1OrUser1Rights);
      await session.setValue(setWrLocked, 3, admin1OrUser1Rights);
      await session.setValue(mbrControl, 3, admin1OrUser1Rights);

      // Enable locking of global range.
      await session.setValue(globalRangeUid, 5, Value.integer(1, 1, false)); // RD
      await session.setValue(globalRangeUid, 6, Value.integer(1, 1, false)); // WR
    } finally {
      await session.end();
    }
  }

  @override
  Widget build(BuildContext context) {
    return WizardPage(
      title: "Configure locking",
      onValidate: _setupUser,
      onNext: (context) => _SuccessPage(),
      child: _buildContent(context),
    );
  }
}

class _ActivateLockingPage extends StatelessWidget {
  const _ActivateLockingPage(this._encryptedDevice, this._sidPassword);

  final EncryptedDevice _encryptedDevice;
  final String _sidPassword;

  Widget _buildEnableNow(BuildContext context) {
    return const Center(
      child: SizedBox(
        width: 280,
        child: Text("Locking is ready to be enabled.\nPress next to continue.", textAlign: TextAlign.center),
      ),
    );
  }

  Widget _buildAlreadyEnabled(BuildContext context) {
    return const Center(
      child: SizedBox(
        width: 280,
        child: Text("Locking is already enabled.\nPress next to continue.", textAlign: TextAlign.center),
      ),
    );
  }

  Future<void> _enableLocking() async {
    final session = await _encryptedDevice.login(_adminSpUid);
    try {
      await session.authenticate(_sidUid, _sidPassword);
      await session.activate(_lockingSpUid);
    } finally {
      await session.end();
    }
  }

  Future<void> _enableMBR() async {
    final session = await _encryptedDevice.login(_lockingSpUid);
    try {
      final admin1Uid = await _encryptedDevice.findUid("Authority::Admin1", securityProvider: _lockingSpUid);
      final mbrControl = await _encryptedDevice.findUid("MBRControl::MBRControl", securityProvider: _lockingSpUid);
      await session.authenticate(admin1Uid, _sidPassword);
      await session.setValue(mbrControl, 1, Value.integer(1, 1, false));
    } finally {
      await session.end();
    }
  }

  Future<void> _enableLockingIfDisabled() async {
    if (!await _isLockingEnabled() || !await _isMBREnabled()) {
      await _enableLocking();
      await _enableMBR();
    }
  }

  Future<bool> _isLockingEnabled() async {
    final session = await _encryptedDevice.login(_adminSpUid);
    try {
      const manufacturedInactive = 8;
      final lifeCycleState = (await session.getValue(_lockingSpUid, 6)).getInteger();
      return lifeCycleState != manufacturedInactive;
    } finally {
      await session.end();
    }
  }

  Future<bool> _isMBREnabled() async {
    try {
      final session = await _encryptedDevice.login(_lockingSpUid);
      try {
        final mbrControl = await _encryptedDevice.findUid("MBRControl::MBRControl", securityProvider: _lockingSpUid);
        final mbrEnabled = (await session.getValue(mbrControl, 1)).getInteger();
        return mbrEnabled != 0;
      } finally {
        await session.end();
      }
    } catch (ex) {
      return false;
    }
  }

  @override
  Widget build(BuildContext context) {
    return WizardPage(
      title: "Enable locking",
      onValidate: _enableLockingIfDisabled,
      onNext: (context) => _UserSetupPage(_encryptedDevice, _sidPassword),
      child: FutureBuilder(
        future: (() async => await _isLockingEnabled() && await _isMBREnabled())(),
        builder: (context, snapshot) {
          return SnapshotBuilder(
            snapshot,
            waiting: (context) => const StatusPage.waiting(message: "Checking locking state..."),
            error: (context, error) => StatusPage.error(message: "Error: $error"),
            success: (context, data) => data! ? _buildAlreadyEnabled(context) : _buildEnableNow(context),
          );
        },
      ),
    );
  }
}

class _OwnershipPage extends StatelessWidget {
  _OwnershipPage(this._encryptedDevice);

  final EncryptedDevice _encryptedDevice;
  final _passwordController = TextEditingController();
  final _repeatController = TextEditingController();

  Widget _buildNewOwner(BuildContext context) {
    const instruction = Tooltip(
      waitDuration: Durations.long1,
      message: "You'll later need this to do further configuration in the table editor or to perform a device reset."
          " Losing this password means you likely lose access to all your data and have to perform a PSID factory"
          " reset.",
      child: Text("Choose your owner's (SID) password:"),
    );

    final password = TextField(
      obscureText: true,
      decoration: const InputDecoration(hintText: "New password"),
      controller: _passwordController,
    );

    final repeat = TextField(
      obscureText: true,
      decoration: const InputDecoration(hintText: "Repeat new password"),
      controller: _repeatController,
    );

    return Center(
      child: SizedBox(
        width: 280,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            instruction,
            const SizedBox(height: 6),
            password,
            const SizedBox(height: 6),
            repeat,
          ],
        ),
      ),
    );
  }

  Widget _buildAlreadyOwned(BuildContext context) {
    const instruction = Tooltip(
      waitDuration: Durations.long1,
      message: "Looks like you've already configured this device. You can still go through the configuration and"
          " change settings.",
      child: Text("Enter your owner's (SID) password:"),
    );

    final forgot = Tooltip(
      triggerMode: TooltipTriggerMode.tap,
      waitDuration: Durations.short4,
      message: "You have to perform a factory reset using your physical owner's (PSID) password. "
          " This password is usually printed on the device's label under 'PSID'. You can find the factory reset on the"
          " device's landing page.",
      child: Text(
        "Forgot your password?",
        style: TextStyle(
          color: Theme.of(context).colorScheme.primary,
          fontSize: 10.0,
        ),
      ),
    );

    final password = TextField(
      obscureText: true,
      decoration: const InputDecoration(hintText: "Owner (SID) password"),
      controller: _passwordController,
    );

    return Center(
      child: SizedBox(
        width: 280,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            instruction,
            const SizedBox(height: 6),
            password,
            const SizedBox(height: 4),
            forgot,
          ],
        ),
      ),
    );
  }

  Future<void> _takeOwnership() async {
    if (_passwordController.text != _repeatController.text) {
      throw Exception("the two passwords don't match");
    }
    // Start a session on the Admin SP
    final session = await _encryptedDevice.login(_adminSpUid);
    try {
      // Authenticate as SID using the MSID password.
      final cPinMsid = await _encryptedDevice.findUid("C_PIN::MSID", securityProvider: _adminSpUid);
      final cPinSid = await _encryptedDevice.findUid("C_PIN::SID", securityProvider: _adminSpUid);
      final msidPassword = await session.getValue(cPinMsid, 3);
      await session.authenticateBytes(_sidUid, msidPassword.getBytes());
      // Change SID password to the user-provided one.
      final password = Value.bytesFromString(_passwordController.text);
      await session.setValue(cPinSid, 3, password);
    } finally {
      await session.end();
    }
  }

  Future<void> _checkOwnerCredentials() async {
    final session = await _encryptedDevice.login(_adminSpUid);
    try {
      // Authenticate as SID using the user-provided password.
      await session.authenticate(_sidUid, _passwordController.text);
    } finally {
      await session.end();
    }
  }

  Future<void> _takeOrCheckOwnership() async {
    if (await _isAlreadyOwned()) {
      await _checkOwnerCredentials();
    } else {
      await _takeOwnership();
    }
  }

  Future<bool> _isAlreadyOwned() async {
    final session = await _encryptedDevice.login(_adminSpUid);
    try {
      final cPinMsid = await _encryptedDevice.findUid("C_PIN::MSID", securityProvider: _adminSpUid);
      final msidPassword = await session.getValue(cPinMsid, 3);
      await session.authenticateBytes(_sidUid, msidPassword.getBytes());
      return false;
    } catch (ex) {
      return true;
    } finally {
      await session.end();
    }
  }

  Widget _next(BuildContext context) {
    return _ActivateLockingPage(_encryptedDevice, _passwordController.text);
  }

  @override
  Widget build(BuildContext context) {
    return WizardPage(
      title: "Take ownership",
      onValidate: _takeOrCheckOwnership,
      onNext: _next,
      child: FutureBuilder(
        future: _isAlreadyOwned(),
        builder: (context, snapshot) {
          return SnapshotBuilder(
            snapshot,
            waiting: (context) => const StatusPage.waiting(message: "Checking device ownership..."),
            error: (context, error) => StatusPage.error(message: "Error: $error"),
            success: (context, data) => data! ? _buildAlreadyOwned(context) : _buildNewOwner(context),
          );
        },
      ),
    );
  }
}

class LockingWizard extends StatelessWidget {
  const LockingWizard(this._storageDevice, {super.key});

  final StorageDevice _storageDevice;

  @override
  Widget build(BuildContext context) {
    return EncryptedDeviceBuilder(
      _storageDevice,
      builder: (context, encryptedDevice) {
        return SnapshotBuilder(
          encryptedDevice,
          error: (context, error) => Material(
            child: StatusPage.error(
              message: "Failed to open device: $error",
              onClose: () => Navigator.of(context).pop(),
            ),
          ),
          waiting: (context) => Material(
            child: StatusPage.waiting(
              message: "Opening device...",
              onClose: () => Navigator.of(context).pop(),
            ),
          ),
          success: (context, data) => _OwnershipPage(data!),
        );
      },
    );
  }
}
