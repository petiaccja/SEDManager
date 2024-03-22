import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/bindings/storage_device.dart';
import 'package:sed_manager_gui/interface/components/encrypted_device_builder.dart';
import 'components/snapshot_builder.dart';
import 'components/status_page.dart';

class StackResetPage extends StatelessWidget {
  const StackResetPage(this.storageDevice, {super.key});
  final StorageDevice storageDevice;

  static Future<void> _stackReset(EncryptedDevice encryptedDevice) async {
    await encryptedDevice.stackReset();
  }

  void _close(BuildContext context) {
    Navigator.of(context).pop();
  }

  Widget _buildBody(EncryptedDevice encryptedDevice) {
    return FutureBuilder(
      future: _stackReset(encryptedDevice),
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          waiting: (context) => StatusPage.waiting(
            message: "Resetting stack...",
            onClose: () => _close(context),
          ),
          success: (context, data) => StatusPage.success(
            message: "Stack successfully reset!",
            onClose: () => _close(context),
          ),
          error: (context, error) => StatusPage.error(
            message: "Error: $error",
            onClose: () => _close(context),
          ),
        );
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Stack reset")),
      body: EncryptedDeviceBuilder(
        storageDevice,
        builder: (context, snapshot) {
          return SnapshotBuilder(
            snapshot,
            error: (context, error) => StatusPage.error(
              message: "Failed to open device: $error",
              onClose: () => Navigator.of(context).pop(),
            ),
            waiting: (context) => StatusPage.waiting(
              message: "Opening device...",
              onClose: () => Navigator.of(context).pop(),
            ),
            success: (context, data) => _buildBody(data!),
          );
        },
      ),
    );
  }
}
