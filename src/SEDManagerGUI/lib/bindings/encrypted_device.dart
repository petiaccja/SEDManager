import 'dart:core';
import 'dart:ffi';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';

import 'future.dart';
import 'storage_device.dart';
import 'string.dart';
import 'value.dart';
import 'type.dart';
import 'sedmanager_capi.dart';

typedef UID = int;

class Session implements Finalizable {
  Session(this._handle) {
    _finalizer.attach(this, _handle.cast(), detach: this);
  }

  static final _capi = SEDManagerCAPI();
  final Pointer<CSession> _handle;
  static final _finalizer = NativeFinalizer(_capi.sessionDestroyAddress.cast());

  UID get securityProvider {
    return _capi.sessionGetSecurityProvider(_handle);
  }

  Future<void> end() {
    final futurePtr = _capi.sessionEnd(_handle);
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Stream<UID> getTableRows(UID tableUid) {
    final streamPtr = _capi.sessionGetTableRows(_handle, tableUid);
    final streamWrapper = StreamWrapperUID(streamPtr);
    return streamWrapper.toDartStream();
  }

  int getColumnCount(UID table) {
    return _capi.sessionGetColumnCount(_handle, table);
  }

  String getColumnName(UID table, int column) {
    return StringWrapper(_capi.sessionGetColumnName(_handle, table, column)).toDartString();
  }

  Type getColumnType(UID table, int column) {
    return Type(_capi.sessionGetColumnType(_handle, table, column));
  }

  Future<Value> getValue(UID objectUid, int column) {
    final futurePtr = _capi.sessionGetValue(_handle, objectUid, column);
    final futureWrapper = FutureWrapperValue(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<void> setValue(UID objectUid, int column, Value value) {
    final futurePtr = _capi.sessionSetValue(_handle, objectUid, column, value.handle());
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<void> authenticate(UID authority, String? password) {
    final bytes = password?.toNativeUtf8();
    final futurePtr = _capi.sessionAuthenticate(
      _handle,
      authority,
      bytes?.cast<Uint8>() ?? nullptr,
      bytes?.length ?? 0,
    );
    if (bytes != null) {
      malloc.free(bytes);
    }
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<void> authenticateBytes(UID authority, ByteData? password) {
    if (password != null) {
      final length = password.lengthInBytes;
      final ptr = malloc.allocate<Uint8>(length);
      final bytes = ptr.asTypedList(length);
      for (int i = 0; i < length; ++i) {
        bytes[i] = password.buffer.asUint8List()[i];
      }
      final futurePtr = _capi.sessionAuthenticate(_handle, authority, ptr, length);
      malloc.free(ptr);
      final futureWrapper = FutureWrapperVoid(futurePtr);
      return futureWrapper.toDartFuture();
    } else {
      final futurePtr = _capi.sessionAuthenticate(_handle, authority, nullptr, 0);
      final futureWrapper = FutureWrapperVoid(futurePtr);
      return futureWrapper.toDartFuture();
    }
  }

  Future<void> genMEK(UID lockingRange) {
    final futurePtr = _capi.sessionGenMEK(_handle, lockingRange);
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<void> activate(UID securityProvider) {
    final futurePtr = _capi.sessionActivate(_handle, securityProvider);
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<void> revert(UID securityProvider) {
    final futurePtr = _capi.sessionRevert(_handle, securityProvider);
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }
}

class EncryptedDevice implements Finalizable {
  EncryptedDevice(this._handle) {
    _finalizer.attach(this, _handle.cast(), detach: this);
  }

  static Future<EncryptedDevice> create(StorageDevice storageDevice) {
    final futurePtr = _capi.encryptedDeviceCreate(storageDevice.handle());
    final futureWrapper = FutureWrapperEncryptedDevice(futurePtr);
    return futureWrapper.toDartFuture();
  }

  static final _capi = SEDManagerCAPI();
  final Pointer<CEncryptedDevice> _handle;
  static final _finalizer = NativeFinalizer(_capi.encryptedDeviceDestroyAddress.cast());

  Future<Session> login(UID securityProvider) {
    final futurePtr = _capi.encryptedDeviceLogin(_handle, securityProvider);
    final futureWrapper = FutureWrapperSession(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<String> findName(UID uid, {UID? securityProvider}) {
    final futurePtr = _capi.encryptedDeviceFindName(_handle, uid, securityProvider ?? 0);
    final futureWrapper = FutureWrapperString(futurePtr);
    return futureWrapper.toDartFuture();
  }

  Future<UID> findUid(String name, {UID? securityProvider}) {
    final nameWrapper = StringWrapper.fromString(name);
    final futurePtr = _capi.encryptedDeviceFindUID(_handle, nameWrapper.handle(), securityProvider ?? 0);
    final futureWrapper = FutureWrapperUID(futurePtr);
    return futureWrapper.toDartFuture();
  }

  String renderValue(Value value, Type type, UID securityProvider) {
    final wrapper = StringWrapper(_capi.encryptedDeviceRenderValue(
      _handle,
      value.handle(),
      type.handle(),
      securityProvider,
    ));
    return wrapper.toDartString();
  }

  Value parseValue(String str, Type type, UID securityProvider) {
    final wrapper = StringWrapper.fromString(str);
    return Value(_capi.encryptedDeviceParseValue(
      _handle,
      wrapper.handle(),
      type.handle(),
      securityProvider,
    ));
  }

  Future<void> stackReset() {
    final futurePtr = _capi.encryptedDeviceStackReset(_handle);
    final futureWrapper = FutureWrapperVoid(futurePtr);
    return futureWrapper.toDartFuture();
  }
}
