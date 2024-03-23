import "dart:ffi";
import "package:ffi/ffi.dart";
import "package:sed_manager_gui/bindings/errors.dart";
import "sedmanager_capi.dart";

class StringWrapper implements Finalizable {
  StringWrapper(this._handle) {
    if (_handle == nullptr) {
      throw SEDException(getLastErrorMessage());
    }
    _finalizer.attach(this, _handle.cast(), detach: this);
  }

  StringWrapper.fromString(String str) {
    _handle = _capi.stringCreate();
    if (_handle == nullptr) {
      throw SEDException(getLastErrorMessage());
    }
    final nativeStr = str.toNativeUtf8();
    _capi.stringSet(_handle, nativeStr);
    malloc.free(nativeStr);
  }

  static final _capi = SEDManagerCAPI();
  static final _finalizer = NativeFinalizer(_capi.stringDestroyAddress.cast());
  late Pointer<CString> _handle;

  String toDartString() {
    return _capi.stringGet(_handle).toDartString();
  }

  Pointer<CString> handle() {
    return _handle;
  }
}
