import 'package:sed_manager_gui/bindings/encrypted_device.dart';

Future<String> getDisplayName(UID object, EncryptedDevice encryptedDevice, {UID? securityProvider}) async {
  try {
    return await encryptedDevice.findName(object, securityProvider: securityProvider ?? 0);
  } catch (ex) {
    return object.toRadixString(16).padLeft(16, '0');
  }
}

Future<List<(UID, String)>> enumerateTable(UID table, EncryptedDevice encryptedDevice, Session session) async {
  return session
      .getTableRows(table)
      .asyncMap((object) async => (object, await getDisplayName(object, encryptedDevice)))
      .toList();
}
