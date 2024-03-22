import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/interface/components/session_builder.dart';
import '../utility.dart';

void testSessionBuilder() {
  group("SessionBuilder", () {
    testWidgets('with mock SD', (WidgetTester tester) async {
      final sd = mockSD();
      final encryptedDevice = await EncryptedDevice.create(sd);
      await tester.pumpWidget(standalone(SessionBuilder(
        encryptedDevice,
        await encryptedDevice.findUid("SP::Admin"),
        builder: (context, snapshot) => const Text("BUILT"),
      )));
      expect(find.text('BUILT'), findsOneWidget);
    });
  });
}
