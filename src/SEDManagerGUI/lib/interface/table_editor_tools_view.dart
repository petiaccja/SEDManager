import 'dart:async';
import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/bindings/value.dart';
import 'package:sed_manager_gui/interface/components/snapshot_builder.dart';
import 'package:sed_manager_gui/interface/components/status_indicator.dart';
import 'package:sed_manager_gui/interface/components/object_dropdown.dart';
import 'package:sed_manager_gui/interface/components/utility.dart';

Future<List<(UID, String)>> _getObjects(
  EncryptedDevice encryptedDevice,
  Session session,
  String tableName,
  FutureOr<bool> Function(UID object, Session session)? filter,
) async {
  final table = await encryptedDevice.findUid(tableName, securityProvider: session.securityProvider);
  final objects = <(UID, String)>[];

  await for (final object in session.getTableRows(table)) {
    final include = await filter?.call(object, session) ?? true;
    if (include) {
      objects.add((object, await getDisplayName(object, encryptedDevice, securityProvider: session.securityProvider)));
    }
  }
  return objects;
}

Widget _buildObjectDropdown(
  Future<List<(UID, String)>> objects,
  void Function(UID object) onSelected,
) {
  return FutureBuilder(
      future: objects,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          waiting: (context) => const ObjectDropdown(
            [],
            width: 280,
            hintText: "Loading...",
            enabled: false,
          ),
          success: (context, data) => ObjectDropdown(
            data!,
            width: 280,
            enabled: true,
            onSelected: onSelected,
          ),
          error: (context, error) => Tooltip(
              message: error.toString(),
              child: const ObjectDropdown(
                [],
                width: 280,
                hintText: "Error",
                enabled: false,
              )),
        );
      });
}

class TableEditorToolDialog extends StatelessWidget {
  const TableEditorToolDialog(
    this.title, {
    required this.children,
    super.key,
  });

  final String title;
  final List<Widget> children;

  @override
  Widget build(BuildContext context) {
    final header = Text(title, style: TextStyle(fontSize: 18, color: Theme.of(context).colorScheme.primary));

    const separator = SizedBox(height: 6);
    final separatedChildren = <Widget>[header];
    for (final child in children) {
      separatedChildren.add(separator);
      separatedChildren.add(child);
    }

    return Dialog(
      child: Container(
        margin: const EdgeInsets.all(8),
        width: 280,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          mainAxisAlignment: MainAxisAlignment.center,
          children: separatedChildren,
        ),
      ),
    );
  }
}

class AuthneticateDialog extends StatefulWidget {
  const AuthneticateDialog(
    this.encryptedDevice,
    this.session, {
    this.onAuthenticated,
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;
  final void Function(UID authority)? onAuthenticated;

  @override
  State<AuthneticateDialog> createState() => _AuthenticateDialogState();
}

class _AuthenticateDialogState extends State<AuthneticateDialog> {
  final _passwordController = TextEditingController();
  int? _selectedAuthority;
  final _result = StreamController<void>();

  @override
  void dispose() {
    _passwordController.dispose();
    super.dispose();
  }

  void _onAuthenticate() async {
    if (_selectedAuthority != null) {
      try {
        await widget.session.authenticate(_selectedAuthority!, _passwordController.text);
        _result.add(null);
        widget.onAuthenticated?.call(_selectedAuthority!);
      } catch (ex) {
        _result.addError(ex);
      }
    } else {
      _result.addError(Exception("select an authority!"));
    }
  }

  void _onBack(BuildContext context) {
    Navigator.of(context).pop();
  }

  static Future<bool> _filter(UID authority, Session session) async {
    try {
      final credentialValue = await session.getValue(authority, 10);
      if (!credentialValue.hasValue) {
        return true;
      }
      final credential = credentialValue.getBytes().getUint64(0);
      return credential != 0;
    } catch (ex) {
      return true;
    }
  }

  @override
  Widget build(BuildContext context) {
    final authorities = _getObjects(widget.encryptedDevice, widget.session, "Authority", _filter);
    final authoritySelector = _buildObjectDropdown(
      authorities,
      (object) => setState(() {
        _selectedAuthority = object;
      }),
    );

    final passwordField = TextField(
      obscureText: true,
      controller: _passwordController,
      decoration: const InputDecoration(hintText: "Password"),
    );

    final buttonStrip = Row(
      children: [
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: _onAuthenticate,
            child: const Text("Authenticate"),
          ),
        ),
        const SizedBox(width: 6),
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: () {
              _onBack(context);
            },
            child: const Text("Back"),
          ),
        ),
      ],
    );

    final errorStrip = StreamBuilder(
      stream: _result.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          none: (context) => const StatusIndicator.none(),
          waiting: (context) => const StatusIndicator.none(),
          success: (context, data) => const StatusIndicator.success(message: "Authentication successful!"),
          error: (context, error) => StatusIndicator.error(message: "Error: $error"),
        );
      },
    );

    return TableEditorToolDialog("Authenticate", children: [
      authoritySelector,
      passwordField,
      buttonStrip,
      Align(alignment: Alignment.centerLeft, child: errorStrip),
    ]);
  }
}

class PasswordDialog extends StatefulWidget {
  const PasswordDialog(
    this.encryptedDevice,
    this.session, {
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;

  @override
  State<PasswordDialog> createState() => _PasswordDialogState();
}

class _PasswordDialogState extends State<PasswordDialog> {
  final _passwordController = TextEditingController();
  final _repeatController = TextEditingController();
  int? _selectedAuthority;
  final _result = StreamController<void>();

  @override
  void dispose() {
    _passwordController.dispose();
    super.dispose();
  }

  void _onChangePassword() async {
    if (_selectedAuthority != null) {
      if (_passwordController.text == _repeatController.text) {
        try {
          final credential = await widget.session.getValue(_selectedAuthority!, 10);
          final credentialUid = credential.getBytes().getUint64(0);
          final password = Value.bytesFromString(_passwordController.text);
          await widget.session.setValue(credentialUid, 3, password);
          _result.add(null);
        } catch (ex) {
          _result.addError(ex);
        }
      } else {
        _result.addError(Exception("password do not match!"));
      }
    } else {
      _result.addError(Exception("select an authority!"));
    }
  }

  void _onBack(BuildContext context) {
    Navigator.of(context).pop();
  }

  @override
  Widget build(BuildContext context) {
    final authorities = _getObjects(widget.encryptedDevice, widget.session, "Authority", null);
    final authoritySelector = _buildObjectDropdown(
      authorities,
      (object) => setState(() {
        _selectedAuthority = object;
      }),
    );

    final passwordField = TextField(
      obscureText: true,
      controller: _passwordController,
      decoration: const InputDecoration(hintText: "Password"),
    );

    final repeatField = TextField(
      obscureText: true,
      controller: _repeatController,
      decoration: const InputDecoration(hintText: "Repeat password"),
    );

    final buttonStrip = Row(
      children: [
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: _onChangePassword,
            child: const Text("Change"),
          ),
        ),
        const SizedBox(width: 6),
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: () {
              _onBack(context);
            },
            child: const Text("Back"),
          ),
        ),
      ],
    );

    final errorStrip = StreamBuilder(
      stream: _result.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          none: (context) => const StatusIndicator.none(),
          waiting: (context) => const StatusIndicator.none(),
          success: (context, data) => const StatusIndicator.success(message: "Password changed!"),
          error: (context, error) => StatusIndicator.error(message: "Error: $error"),
        );
      },
    );

    return TableEditorToolDialog("Change password", children: [
      authoritySelector,
      passwordField,
      repeatField,
      buttonStrip,
      Align(alignment: Alignment.centerLeft, child: errorStrip),
    ]);
  }
}

class GenerateMEKDialog extends StatefulWidget {
  const GenerateMEKDialog(
    this.encryptedDevice,
    this.session, {
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;

  @override
  State<GenerateMEKDialog> createState() => _GenerateMEKDialogState();
}

class _GenerateMEKDialogState extends State<GenerateMEKDialog> {
  UID? _selectedLockingRange;
  final _result = StreamController<void>();

  Future<void> _genMEK() async {
    if (_selectedLockingRange != null) {
      try {
        final activeKey = await widget.session.getValue(_selectedLockingRange!, 10);
        final activeKeyUid = activeKey.getBytes().getUint64(0);
        await widget.session.genMEK(activeKeyUid);
        _result.add(null);
      } catch (ex) {
        _result.addError(ex);
      }
    } else {
      _result.addError(Exception("Select a locking range!"));
    }
  }

  void _onBack(BuildContext context) {
    Navigator.of(context).pop();
  }

  @override
  Widget build(BuildContext context) {
    final lockingRanges = _getObjects(widget.encryptedDevice, widget.session, "Locking", null);
    final lockingRangeSelector = _buildObjectDropdown(
      lockingRanges,
      (object) => setState(() {
        _selectedLockingRange = object;
      }),
    );

    const warningText = Row(
      children: [
        Icon(Icons.warning_outlined, color: Colors.amber),
        SizedBox(width: 6),
        Expanded(child: Text("This will erase all data in the selected locking range!"))
      ],
    );

    final buttonStrip = Row(
      children: [
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: () => _genMEK().ignore,
            child: const Text("Generate"),
          ),
        ),
        const SizedBox(width: 6),
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: () {
              _onBack(context);
            },
            child: const Text("Back"),
          ),
        ),
      ],
    );

    final errorStrip = StreamBuilder(
      stream: _result.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          none: (context) => const StatusIndicator.none(),
          waiting: (context) => const StatusIndicator.none(),
          success: (context, data) => const StatusIndicator.success(message: "Encryption key generated!"),
          error: (context, error) => StatusIndicator.error(message: "Error: $error"),
        );
      },
    );

    return TableEditorToolDialog(
      "Generate media encryption key",
      children: [
        warningText,
        lockingRangeSelector,
        buttonStrip,
        Align(alignment: Alignment.centerLeft, child: errorStrip),
      ],
    );
  }
}

class ActivateDialog extends StatefulWidget {
  const ActivateDialog(
    this.encryptedDevice,
    this.session, {
    this.onActivated,
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;
  final void Function(UID securityProvider)? onActivated;

  @override
  State<ActivateDialog> createState() => _ActivateDialogState();
}

class _ActivateDialogState extends State<ActivateDialog> {
  int? _selectedSecurityProvider;
  final _result = StreamController<void>();

  void _onActivate() async {
    if (_selectedSecurityProvider != null) {
      try {
        await widget.session.activate(_selectedSecurityProvider!);
        widget.onActivated?.call(_selectedSecurityProvider!);
        _result.add(null);
      } catch (ex) {
        _result.addError(ex);
      }
    } else {
      _result.addError(Exception("select a security provider!"));
    }
  }

  void _onBack(BuildContext context) {
    Navigator.of(context).pop();
  }

  Future<bool> _filter(UID subjectSp, Session session) async {
    try {
      const manufacturedInactive = 8;
      final lifeCycleState = (await session.getValue(subjectSp, 6)).getInteger();
      return lifeCycleState == manufacturedInactive;
    } catch (ex) {
      return false;
    }
  }

  @override
  Widget build(BuildContext context) {
    final securityProviders = _getObjects(widget.encryptedDevice, widget.session, "SP", _filter);
    final spSelector = _buildObjectDropdown(
      securityProviders,
      (object) => setState(() {
        _selectedSecurityProvider = object;
      }),
    );

    final buttonStrip = Row(
      children: [
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: _onActivate,
            child: const Text("Activate"),
          ),
        ),
        const SizedBox(width: 6),
        Expanded(
          flex: 1,
          child: FilledButton(
            onPressed: () {
              _onBack(context);
            },
            child: const Text("Back"),
          ),
        ),
      ],
    );

    final errorStrip = StreamBuilder(
      stream: _result.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          none: (context) => const StatusIndicator.none(),
          waiting: (context) => const StatusIndicator.none(),
          success: (context, data) => const StatusIndicator.success(message: "Security provider activated!"),
          error: (context, error) => StatusIndicator.error(message: "Error: $error"),
        );
      },
    );

    return TableEditorToolDialog("Activate security provider", children: [
      spSelector,
      buttonStrip,
      Align(alignment: Alignment.centerLeft, child: errorStrip),
    ]);
  }
}

class TableEditorToolsView extends StatelessWidget {
  const TableEditorToolsView(
    this.encryptedDevice,
    this.session, {
    this.onAuthenticated,
    this.onActivated,
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;
  final void Function(UID authority)? onAuthenticated;
  final void Function(UID securityProvider)? onActivated;

  Widget _buildButton(IconData icon, String title, void Function()? onPressed) {
    final style = ButtonStyle(
      padding: const MaterialStatePropertyAll(EdgeInsets.all(6)),
      shape: MaterialStatePropertyAll(
        RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
      ),
    );

    return Container(
      margin: const EdgeInsets.fromLTRB(0, 3, 0, 3),
      child: ElevatedButton(
        onPressed: onPressed,
        style: style,
        child: Tooltip(
          waitDuration: Durations.medium1,
          message: title,
          child: Icon(icon, size: 40),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    const UID adminSpUid = 0x0000020500000001;
    const UID lockingSpUid = 0x0000020500000002;

    final authenticateButton = _buildButton(
      Icons.person,
      "Authenticate",
      () {
        showDialog(
          context: context,
          builder: (context) => AuthneticateDialog(
            encryptedDevice,
            session,
            onAuthenticated: onAuthenticated,
          ),
        );
      },
    );

    final changePassButton = _buildButton(Icons.password, "Change password", () {
      showDialog(
        context: context,
        builder: (context) => PasswordDialog(encryptedDevice, session),
      );
    });

    final genMekButton = _buildButton(
      Icons.key,
      "Generate media encryption key",
      session.securityProvider != lockingSpUid
          ? null
          : () {
              showDialog(
                context: context,
                builder: (context) => GenerateMEKDialog(encryptedDevice, session),
              );
            },
    );

    final activateButton = _buildButton(
      Icons.rocket_launch,
      "Activate security provider",
      session.securityProvider != adminSpUid
          ? null
          : () {
              showDialog(
                context: context,
                builder: (context) => ActivateDialog(
                  encryptedDevice,
                  session,
                  onActivated: onActivated,
                ),
              );
            },
    );

    return SizedBox(
      width: 64,
      child: ListView(
        shrinkWrap: true,
        itemExtent: 70,
        children: [
          authenticateButton,
          changePassButton,
          genMekButton,
          activateButton,
        ],
      ),
    );
  }
}
