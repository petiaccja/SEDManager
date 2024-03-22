import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/storage_device.dart';

Widget standalone(Widget testee) {
  return MaterialApp(home: testee);
}

StorageDevice mockSD() {
  return StorageDevice("/dev/mock_device");
}