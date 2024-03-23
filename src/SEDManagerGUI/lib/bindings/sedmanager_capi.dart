import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'dart:io' show Platform;
import 'package:path/path.dart' as path;

final libraryName = Platform.isWindows ? "SEDManagerCAPI.dll" : "libSEDManagerCAPI.so";
const libraryDirEnv = String.fromEnvironment("CAPI_LIBRARY_PATH", defaultValue: "");
final libraryDirLocal = Platform.resolvedExecutable;
final libraryPath = path.join(libraryDirEnv.isEmpty ? libraryDirLocal : libraryDirEnv, libraryName);

typedef CUID = Uint64;

final class CString extends Opaque {}

final class CValue extends Opaque {}

final class CType extends Opaque {}

final class CStorageDevice extends Opaque {}

final class CEncryptedDevice extends Opaque {}

final class CSession extends Opaque {}

final class CFuture<T> extends Opaque {}

final class CStream<T> extends Opaque {}

typedef CFutureVoid = CFuture<Void>;

typedef CFutureEncryptedDevice = CFuture<CEncryptedDevice>;
typedef CFutureSession = CFuture<CSession>;
typedef CFutureValue = CFuture<CValue>;
typedef CFutureString = CFuture<CString>;
typedef CFutureUID = CFuture<CUID>;
typedef CStreamUid = CStream<CUID>;
typedef CStreamString = CStream<CString>;
typedef CFutureCallback<T> = NativeFunction<Void Function(Bool, T)>;
typedef CStreamCallback<T> = NativeFunction<Void Function(Bool, Bool, T)>;

class SEDManagerCAPI {
  SEDManagerCAPI._privateConstructor();

  static final SEDManagerCAPI _instance = _create();
  static final dylib = DynamicLibrary.open(libraryPath);

  factory SEDManagerCAPI() {
    return _instance;
  }

  static SEDManagerCAPI _create() {
    return SEDManagerCAPI._privateConstructor();
  }

  //----------------------------------------------------------------------------
  // Error handling
  //----------------------------------------------------------------------------

  final getLastExceptionMessage = dylib.lookupFunction<Pointer<CString> Function(), Pointer<CString> Function()>(
    "CGetLastException_Message",
    isLeaf: true,
  );

  //----------------------------------------------------------------------------
  // CString
  //----------------------------------------------------------------------------

  final stringCreate = dylib.lookupFunction<Pointer<CString> Function(), Pointer<CString> Function()>(
    "CString_Create",
    isLeaf: true,
  );

  final stringDestroyAddress = dylib.lookup<NativeFunction<Void Function(Pointer<CString>)>>("CString_Destroy");
  late final stringDestroy = stringDestroyAddress.asFunction<void Function(Pointer<CString>)>(isLeaf: true);

  final stringSet = dylib
      .lookupFunction<Void Function(Pointer<CString>, Pointer<Utf8>), void Function(Pointer<CString>, Pointer<Utf8>)>(
    "CString_Set",
    isLeaf: true,
  );

  final stringGet =
      dylib.lookupFunction<Pointer<Utf8> Function(Pointer<CString>), Pointer<Utf8> Function(Pointer<CString>)>(
    "CString_Get",
    isLeaf: true,
  );

  //----------------------------------------------------------------------------
  // CValue
  //----------------------------------------------------------------------------

  final valueCreate = dylib.lookupFunction<Pointer<CValue> Function(), Pointer<CValue> Function()>(
    "CValue_Create",
    isLeaf: true,
  );

  final valueDestroyAddress = dylib.lookup<NativeFunction<Void Function(Pointer<CValue>)>>("CValue_Destroy");
  late final valueDestroy = valueDestroyAddress.asFunction<void Function(Pointer<CValue>)>(isLeaf: true);

  final valueHasValue = dylib.lookupFunction<Bool Function(Pointer<CValue>), bool Function(Pointer<CValue>)>(
    "CValue_HasValue",
    isLeaf: true,
  );

  final valueIsBytes = dylib.lookupFunction<Bool Function(Pointer<CValue>), bool Function(Pointer<CValue>)>(
    "CValue_IsBytes",
    isLeaf: true,
  );

  final valueIsCommand = dylib.lookupFunction<Bool Function(Pointer<CValue>), bool Function(Pointer<CValue>)>(
    "CValue_IsCommand",
    isLeaf: true,
  );

  final valueIsInteger = dylib.lookupFunction<Bool Function(Pointer<CValue>), bool Function(Pointer<CValue>)>(
    "CValue_IsInteger",
    isLeaf: true,
  );

  final valueIsList = dylib.lookupFunction<Bool Function(Pointer<CValue>), bool Function(Pointer<CValue>)>(
    "CValue_IsList",
    isLeaf: true,
  );

  final valueIsNamed = dylib.lookupFunction<Bool Function(Pointer<CValue>), bool Function(Pointer<CValue>)>(
    "CValue_IsNamed",
    isLeaf: true,
  );

  final valueGetBytes =
      dylib.lookupFunction<Pointer<Uint8> Function(Pointer<CValue>), Pointer<Uint8> Function(Pointer<CValue>)>(
    "CValue_GetBytes",
    isLeaf: true,
  );

  final valueGetCommand = dylib.lookupFunction<Uint8 Function(Pointer<CValue>), int Function(Pointer<CValue>)>(
    "CValue_GetCommand",
    isLeaf: true,
  );

  final valueGetInteger = dylib.lookupFunction<Int64 Function(Pointer<CValue>), int Function(Pointer<CValue>)>(
    "CValue_GetInteger",
    isLeaf: true,
  );

  final valueGetListElement = dylib
      .lookupFunction<Pointer<CValue> Function(Pointer<CValue>, Size), Pointer<CValue> Function(Pointer<CValue>, int)>(
    "CValue_GetList_Element",
    isLeaf: true,
  );

  final valueGetNamedName =
      dylib.lookupFunction<Pointer<CValue> Function(Pointer<CValue>), Pointer<CValue> Function(Pointer<CValue>)>(
    "CValue_GetNamed_Name",
    isLeaf: true,
  );

  final valueGetNamedValue =
      dylib.lookupFunction<Pointer<CValue> Function(Pointer<CValue>), Pointer<CValue> Function(Pointer<CValue>)>(
    "CValue_GetNamed_Value",
    isLeaf: true,
  );

  final valueGetLength = dylib.lookupFunction<Size Function(Pointer<CValue>), int Function(Pointer<CValue>)>(
    "CValue_GetLength",
    isLeaf: true,
  );

  final valueSetBytes = dylib.lookupFunction<Void Function(Pointer<CValue>, Pointer<Uint8>, Size),
      void Function(Pointer<CValue>, Pointer<Uint8>, int)>(
    "CValue_SetBytes",
    isLeaf: true,
  );

  final valueSetCommand =
      dylib.lookupFunction<Void Function(Pointer<CValue>, Uint8), void Function(Pointer<CValue>, int)>(
    "CValue_SetCommand",
    isLeaf: true,
  );

  final valueSetInteger = dylib.lookupFunction<Void Function(Pointer<CValue>, Int64, Uint8, Bool),
      void Function(Pointer<CValue>, int, int, bool)>(
    "CValue_SetInteger",
    isLeaf: true,
  );

  final valueSetList = dylib.lookupFunction<Void Function(Pointer<CValue>, Pointer<Pointer<CValue>>, Size),
      void Function(Pointer<CValue>, Pointer<Pointer<CValue>>, int)>(
    "CValue_SetList",
    isLeaf: true,
  );

  final valueSetNamed = dylib.lookupFunction<Void Function(Pointer<CValue>, Pointer<CValue>, Pointer<CValue>),
      void Function(Pointer<CValue>, Pointer<CValue>, Pointer<CValue>)>(
    "CValue_SetNamed",
    isLeaf: true,
  );

  //----------------------------------------------------------------------------
  // CValue
  //----------------------------------------------------------------------------

  final typeCreate = dylib.lookupFunction<Pointer<CType> Function(), Pointer<CType> Function()>(
    "CType_Create",
    isLeaf: true,
  );

  final typeDestroyAddress = dylib.lookup<NativeFunction<Void Function(Pointer<CType>)>>("CType_Destroy");
  late final typeDestroy = typeDestroyAddress.asFunction<void Function(Pointer<CType>)>(isLeaf: true);

  //----------------------------------------------------------------------------
  // CStorageDevice
  //----------------------------------------------------------------------------

  final enumerateStorageDevices = dylib.lookupFunction<Pointer<CString> Function(), Pointer<CString> Function()>(
    "CEnumerateStorageDevices",
    isLeaf: true,
  );

  final storageDeviceCreate = dylib.lookupFunction<Pointer<CStorageDevice> Function(Pointer<CString>),
      Pointer<CStorageDevice> Function(Pointer<CString>)>(
    "CStorageDevice_Create",
    isLeaf: true,
  );

  final storageDeviceDestroyAddress =
      dylib.lookup<Pointer<NativeFunction<Void Function(Pointer<CStorageDevice>)>>>("CStorageDevice_Destroy");

  final storageDeviceGetName = dylib.lookupFunction<Pointer<CString> Function(Pointer<CStorageDevice>),
      Pointer<CString> Function(Pointer<CStorageDevice>)>(
    "CStorageDevice_GetName",
    isLeaf: true,
  );

  final storageDeviceGetSerial = dylib.lookupFunction<Pointer<CString> Function(Pointer<CStorageDevice>),
      Pointer<CString> Function(Pointer<CStorageDevice>)>(
    "CStorageDevice_GetSerial",
    isLeaf: true,
  );

  final storageDeviceGetFirmware = dylib.lookupFunction<Pointer<CString> Function(Pointer<CStorageDevice>),
      Pointer<CString> Function(Pointer<CStorageDevice>)>(
    "CStorageDevice_GetFirmware",
    isLeaf: true,
  );

  final storageDeviceGetInterface = dylib.lookupFunction<Pointer<CString> Function(Pointer<CStorageDevice>),
      Pointer<CString> Function(Pointer<CStorageDevice>)>(
    "CStorageDevice_GetInterface",
    isLeaf: true,
  );

  final storageDeviceGetSSCs = dylib.lookupFunction<Pointer<CString> Function(Pointer<CStorageDevice>),
      Pointer<CString> Function(Pointer<CStorageDevice>)>(
    "CStorageDevice_GetSSCs",
    isLeaf: true,
  );

  //----------------------------------------------------------------------------
  // CEncryptedDevice
  //----------------------------------------------------------------------------

  final encryptedDeviceCreate = dylib.lookupFunction<Pointer<CFutureEncryptedDevice> Function(Pointer<CStorageDevice>),
      Pointer<CFutureEncryptedDevice> Function(Pointer<CStorageDevice>)>(
    "CEncryptedDevice_Create",
    isLeaf: true,
  );

  final encryptedDeviceDestroyAddress =
      dylib.lookup<Pointer<NativeFunction<Void Function(Pointer<CEncryptedDevice>)>>>("CEncryptedDevice_Destroy");

  final encryptedDeviceLogin = dylib.lookupFunction<Pointer<CFutureSession> Function(Pointer<CEncryptedDevice>, CUID),
      Pointer<CFutureSession> Function(Pointer<CEncryptedDevice>, int)>(
    "CEncryptedDevice_Login",
    isLeaf: true,
  );

  final encryptedDeviceStackReset = dylib.lookupFunction<Pointer<CFutureVoid> Function(Pointer<CEncryptedDevice>),
      Pointer<CFutureVoid> Function(Pointer<CEncryptedDevice>)>(
    "CEncryptedDevice_StackReset",
    isLeaf: true,
  );

  final encryptedDeviceReset = dylib.lookupFunction<Pointer<CFutureVoid> Function(Pointer<CEncryptedDevice>),
      Pointer<CFutureVoid> Function(Pointer<CEncryptedDevice>)>(
    "CEncryptedDevice_Reset",
    isLeaf: true,
  );

  final encryptedDeviceFindName = dylib.lookupFunction<
      Pointer<CFutureString> Function(Pointer<CEncryptedDevice>, CUID, CUID),
      Pointer<CFutureString> Function(Pointer<CEncryptedDevice>, int, int)>(
    "CEncryptedDevice_FindName",
    isLeaf: true,
  );

  final encryptedDeviceFindUID = dylib.lookupFunction<
      Pointer<CFutureUID> Function(Pointer<CEncryptedDevice>, Pointer<CString>, CUID),
      Pointer<CFutureUID> Function(Pointer<CEncryptedDevice>, Pointer<CString>, int)>(
    "CEncryptedDevice_FindUID",
    isLeaf: true,
  );

  final encryptedDeviceRenderValue = dylib.lookupFunction<
      Pointer<CString> Function(Pointer<CEncryptedDevice>, Pointer<CValue>, Pointer<CType>, CUID),
      Pointer<CString> Function(Pointer<CEncryptedDevice>, Pointer<CValue>, Pointer<CType>, int)>(
    "CEncryptedDevice_RenderValue",
    isLeaf: true,
  );

  final encryptedDeviceParseValue = dylib.lookupFunction<
      Pointer<CValue> Function(Pointer<CEncryptedDevice>, Pointer<CString>, Pointer<CType>, CUID),
      Pointer<CValue> Function(Pointer<CEncryptedDevice>, Pointer<CString>, Pointer<CType>, int)>(
    "CEncryptedDevice_ParseValue",
    isLeaf: true,
  );

  //----------------------------------------------------------------------------
  // CSession
  //----------------------------------------------------------------------------

  final sessionDestroyAddress =
      dylib.lookup<Pointer<NativeFunction<Void Function(Pointer<CSession>)>>>("CSession_Destroy");

  final sessionAuthenticate = dylib.lookupFunction<
      Pointer<CFutureVoid> Function(Pointer<CSession>, CUID, Pointer<Uint8>, Size),
      Pointer<CFutureVoid> Function(Pointer<CSession>, int, Pointer<Uint8>, int)>(
    "CSession_Authenticate",
    isLeaf: true,
  );

  final sessionEnd = dylib.lookupFunction<Pointer<CFutureVoid> Function(Pointer<CSession>),
      Pointer<CFutureVoid> Function(Pointer<CSession>)>(
    "CSession_End",
    isLeaf: true,
  );

  final sessionGetSecurityProvider = dylib.lookupFunction<CUID Function(Pointer<CSession>),
      int Function(Pointer<CSession>)>(
    "CSession_GetSecurityProvider",
    isLeaf: true,
  );

  final sessionGetTableRows = dylib.lookupFunction<Pointer<CStreamUid> Function(Pointer<CSession>, CUID),
      Pointer<CStreamUid> Function(Pointer<CSession>, int)>(
    "CSession_GetTableRows",
    isLeaf: true,
  );

  final sessionGetColumnCount =
      dylib.lookupFunction<Size Function(Pointer<CSession>, CUID), int Function(Pointer<CSession>, int)>(
    "CSession_GetColumnCount",
    isLeaf: true,
  );

  final sessionGetColumnName = dylib.lookupFunction<Pointer<CString> Function(Pointer<CSession>, CUID, Uint32),
      Pointer<CString> Function(Pointer<CSession>, int, int)>(
    "CSession_GetColumnName",
    isLeaf: true,
  );

  final sessionGetColumnType = dylib.lookupFunction<Pointer<CType> Function(Pointer<CSession>, CUID, Uint32),
      Pointer<CType> Function(Pointer<CSession>, int, int)>(
    "CSession_GetColumnType",
    isLeaf: true,
  );

  final sessionGetValue = dylib.lookupFunction<Pointer<CFutureValue> Function(Pointer<CSession>, CUID, Int32),
      Pointer<CFutureValue> Function(Pointer<CSession>, int, int)>(
    "CSession_GetValue",
    isLeaf: true,
  );

  final sessionSetValue = dylib.lookupFunction<
      Pointer<CFutureVoid> Function(Pointer<CSession>, CUID, Int32, Pointer<CValue>),
      Pointer<CFutureVoid> Function(Pointer<CSession>, int, int, Pointer<CValue>)>(
    "CSession_SetValue",
    isLeaf: true,
  );

  final sessionGenMEK = dylib.lookupFunction<Pointer<CFutureVoid> Function(Pointer<CSession>, CUID),
      Pointer<CFutureVoid> Function(Pointer<CSession>, int)>(
    "CSession_GenMEK",
    isLeaf: true,
  );

  final sessionRevert = dylib.lookupFunction<Pointer<CFutureVoid> Function(Pointer<CSession>, CUID),
      Pointer<CFutureVoid> Function(Pointer<CSession>, int)>(
    "CSession_Revert",
    isLeaf: true,
  );

  final sessionActivate = dylib.lookupFunction<Pointer<CFutureVoid> Function(Pointer<CSession>, CUID),
      Pointer<CFutureVoid> Function(Pointer<CSession>, int)>(
    "CSession_Activate",
    isLeaf: true,
  );

  //----------------------------------------------------------------------------
  // Futures
  //----------------------------------------------------------------------------

  Pointer<NativeFunction<Void Function(Pointer<CFuture<T>>)>> lookupFutureDestroy<T extends NativeType>(String suffix) {
    final address = dylib.lookup<NativeFunction<Void Function(Pointer<CFuture<T>>)>>("CFuture${suffix}_Destroy");
    return address;
  }

  void Function(Pointer<CFuture<FutureType>>, Pointer<CFutureCallback<ResultType>>)
      lookupFutureStart<FutureType extends NativeType, ResultType extends NativeType>(String suffix) {
    final address = dylib.lookup<NativeFunction<Void Function(Pointer<CFuture<FutureType>>, Pointer<NativeType>)>>(
        "CFuture${suffix}_Start");
    final function = address
        .asFunction<void Function(Pointer<CFuture<FutureType>>, Pointer<CFutureCallback<ResultType>>)>(isLeaf: false);
    return function;
  }

  Pointer<NativeFunction<Void Function(Pointer<CStream<T>>)>> lookupStreamDestroy<T extends NativeType>(String suffix) {
    final address = dylib.lookup<NativeFunction<Void Function(Pointer<CStream<T>>)>>("CStream${suffix}_Destroy");
    return address;
  }

  void Function(Pointer<CStream<StreamType>>, Pointer<CStreamCallback<ResultType>>)
      lookupStreamAdvance<StreamType extends NativeType, ResultType extends NativeType>(String suffix) {
    final address = dylib.lookup<NativeFunction<Void Function(Pointer<CStream<StreamType>>, Pointer<NativeType>)>>(
        "CStream${suffix}_Advance");
    final function = address
        .asFunction<void Function(Pointer<CStream<StreamType>>, Pointer<CStreamCallback<ResultType>>)>(isLeaf: false);
    return function;
  }
}
