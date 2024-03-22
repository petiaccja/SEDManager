import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/interface/components/session_builder.dart';
import '../utility.dart';

void testSessionBuilder() {
  group("SessionBuilder", () {
    testWidgets('with mock SD', (WidgetTester tester) async {
      final sd = mockSD();
      final maybeEncryptedDevice = await tester.runAsync(() async => await EncryptedDevice.create(sd));
      final maybeAdminSp = await tester.runAsync(() async => await maybeEncryptedDevice!.findUid("SP::Admin"));
      await tester.pumpWidget(standalone(SessionBuilder(
        maybeEncryptedDevice!,
        maybeAdminSp!,
        builder: (context, snapshot) => const Text("BUILT"),
      )));
      expect(find.text('BUILT'), findsOneWidget);
    });
  });
}
