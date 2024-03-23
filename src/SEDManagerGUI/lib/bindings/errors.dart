import 'dart:core';
import 'string.dart';
import 'sedmanager_capi.dart';

String getLastErrorMessage() {
  final capi = SEDManagerCAPI();
  final message = StringWrapper(capi.getLastExceptionMessage());
  return message.toDartString();
}

class SEDException implements Exception {
  SEDException(this.message);

  final String message;

  @override
  String toString() {
    return message;
  }
}
