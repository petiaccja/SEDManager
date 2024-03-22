import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:sed_manager_gui/interface/components/encrypted_device_builder.dart';
import '../utility.dart';

void testEncryptedDeviceBuilder() {
  group("EncryptedDeviceBuilder", () {
    testWidgets('with mock SD', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(EncryptedDeviceBuilder(
        mockSD(),
        builder: (context, snapshot) => const Text("BUILT"),
      )));
      expect(find.text('BUILT'), findsOneWidget);
    });
  });
}
