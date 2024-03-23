import 'dart:async';
import 'package:flutter/material.dart';
import "package:sed_manager_gui/bindings/encrypted_device.dart";
import "package:sed_manager_gui/bindings/storage_device.dart";

class EncryptedDeviceBuilder extends StatelessWidget {
  const EncryptedDeviceBuilder(
    this.storageDevice, {
    required this.builder,
    super.key,
  });

  final StorageDevice storageDevice;
  final Widget Function(BuildContext context, AsyncSnapshot<EncryptedDevice> snapshot) builder;

  Future<EncryptedDevice> _getEncryptedDevice() async {
    return await EncryptedDevice.create(storageDevice);
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _getEncryptedDevice(),
      builder: builder,
    );
  }
}
