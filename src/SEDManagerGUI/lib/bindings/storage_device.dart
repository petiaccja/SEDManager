import 'dart:core';
import 'dart:ffi';
import 'package:sed_manager_gui/bindings/errors.dart';
import 'package:sed_manager_gui/bindings/string.dart';
import 'sedmanager_capi.dart';

class StorageDevice implements Finalizable {
  StorageDevice(String path) {
    final api = SEDManagerCAPI();
    final pathWrapper = StringWrapper.fromString(path);
    _handle = api.storageDeviceCreate(pathWrapper.handle());
    if (_handle == nullptr) {
      throw SEDException(getLastErrorMessage());
    }
    _finalizer.attach(this, _handle.cast(), detach: this);
  }

  static final _capi = SEDManagerCAPI();
  late final Pointer<CStorageDevice> _handle;
  static final _finalizer = NativeFinalizer(_capi.storageDeviceDestroyAddress.cast());

  String getName() {
    final nameWrapper = StringWrapper(_capi.storageDeviceGetName(_handle));
    return nameWrapper.toDartString();
  }

  String getSerial() {
    final nameWrapper = StringWrapper(_capi.storageDeviceGetSerial(_handle));
    return nameWrapper.toDartString();
  }

  String getFirmware() {
    final nameWrapper = StringWrapper(_capi.storageDeviceGetFirmware(_handle));
    return nameWrapper.toDartString();
  }

  String getInterface() {
    final nameWrapper = StringWrapper(_capi.storageDeviceGetInterface(_handle));
    return nameWrapper.toDartString();
  }

  List<String> getSSCs() {
    final nameWrapper = StringWrapper(_capi.storageDeviceGetSSCs(_handle));
    var sscs = nameWrapper.toDartString().split(";");
    sscs.removeWhere((element) => element.isEmpty);
    return sscs;
  }

  Pointer<CStorageDevice> handle() {
    return _handle;
  }
}

List<StorageDevice> enumerateStorageDevices() {
  final capi = SEDManagerCAPI();
  final devicePaths = StringWrapper(capi.enumerateStorageDevices()).toDartString();

  final paths = devicePaths.split(';');
  paths.retainWhere((element) => element.isNotEmpty);

  var devices = <StorageDevice>[];
  for (var path in paths) {
    try {
      devices.add(StorageDevice(path));
    } catch (ex) {
      // Ignore devices that failed to open.
    }
  }
  return devices;
}
