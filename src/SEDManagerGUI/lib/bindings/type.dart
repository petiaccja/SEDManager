import "dart:ffi";
import "errors.dart";
import "sedmanager_capi.dart";

class Type implements Finalizable {
  Type(this._handle) {
    if (_handle == nullptr) {
      throw SEDException(getLastErrorMessage());
    }
    _finalizer.attach(this, _handle.cast(), detach: this);
  }

  Type.empty() {
    _handle = _capi.typeCreate();
    if (_handle == nullptr) {
      throw SEDException(getLastErrorMessage());
    }
  }

  static final _capi = SEDManagerCAPI();
  static final _finalizer = NativeFinalizer(_capi.typeDestroyAddress.cast());
  late Pointer<CType> _handle;

  Pointer<CType> handle() {
    return _handle;
  }
}
