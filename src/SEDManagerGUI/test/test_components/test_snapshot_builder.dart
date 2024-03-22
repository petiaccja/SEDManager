import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:sed_manager_gui/interface/components/snapshot_builder.dart';
import '../utility.dart';

Widget snapshotBuilder(AsyncSnapshot<void> snapshot) {
  return SnapshotBuilder(
    snapshot,
    none: (context) => const Text("NONE"),
    waiting: (context) => const Text("WAITING"),
    success: (context, value) => const Text("SUCCESS"),
    error: (context, object) => const Text("ERROR"),
  );
}

void testSnapshotBuilder() {
  group("SnapshotBuilder", () {
    testWidgets('none', (WidgetTester tester) async {
      const snapshot = AsyncSnapshot.nothing();
      await tester.pumpWidget(standalone(snapshotBuilder(snapshot)));
      expect(find.text('NONE'), findsOneWidget);
    });

    testWidgets('waiting', (WidgetTester tester) async {
      const snapshot = AsyncSnapshot.waiting();
      await tester.pumpWidget(standalone(snapshotBuilder(snapshot)));
      expect(find.text('WAITING'), findsOneWidget);
    });

    testWidgets('success', (WidgetTester tester) async {
      const snapshot = AsyncSnapshot.withData(ConnectionState.done, null);
      await tester.pumpWidget(standalone(snapshotBuilder(snapshot)));
      expect(find.text('SUCCESS'), findsOneWidget);
    });

    testWidgets('error', (WidgetTester tester) async {
      const snapshot = AsyncSnapshot.withError(ConnectionState.done, "<error>");
      await tester.pumpWidget(standalone(snapshotBuilder(snapshot)));
      expect(find.text('ERROR'), findsOneWidget);
    });
  });
}
