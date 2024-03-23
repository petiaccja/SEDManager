import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:sed_manager_gui/interface/components/status_indicator.dart';
import '../utility.dart';

void testStatusIndicator() {
  group("StatusIndicator", () {
    testWidgets('message / none', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.none(message: "STATUS")));
      expect(find.text('STATUS'), findsOneWidget);
    });

    testWidgets('message / waiting', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.waiting(message: "STATUS")));
      expect(find.text('STATUS'), findsOneWidget);
    });

    testWidgets('message / success', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.success(message: "STATUS")));
      expect(find.text('STATUS'), findsOneWidget);
    });

    testWidgets('message / error', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.error(message: "STATUS")));
      expect(find.text('STATUS'), findsOneWidget);
    });

    testWidgets('icon / none', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.none(iconSize: 64)));
      final iconFinder = find.byType(Icon);
      expect(iconFinder, findsOneWidget);
      expect(iconFinder.found.first.size!.height, 64);
    });

    testWidgets('icon / waiting', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.waiting(iconSize: 64)));
      final iconFinder = find.byType(CircularProgressIndicator);
      expect(iconFinder, findsOneWidget);
      expect(iconFinder.found.first.size!.height, 64);
    });

    testWidgets('icon / success', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.success(iconSize: 64)));
      final iconFinder = find.byType(Icon);
      expect(iconFinder, findsOneWidget);
      expect(iconFinder.found.first.size!.height, 64);
    });

    testWidgets('icon / error', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.error(iconSize: 64)));
      final iconFinder = find.byType(Icon);
      expect(iconFinder, findsOneWidget);
      expect(iconFinder.found.first.size!.height, 64);
    });

    testWidgets('text position down', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.success(
        iconSize: 64,
        message: "STATUS",
        messagePosition: AxisDirection.down,
      )));
      final iconFinder = find.byType(Icon);
      final textFinder = find.byType(Text);
      expect(iconFinder, findsOneWidget);
      expect(textFinder, findsOneWidget);
      expect(tester.getCenter(iconFinder).dy, lessThan(tester.getCenter(textFinder).dy));
    });

    testWidgets('text position up', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.success(
        iconSize: 64,
        message: "STATUS",
        messagePosition: AxisDirection.up,
      )));
      final iconFinder = find.byType(Icon);
      final textFinder = find.byType(Text);
      expect(iconFinder, findsOneWidget);
      expect(textFinder, findsOneWidget);
      expect(tester.getCenter(iconFinder).dy, greaterThan(tester.getCenter(textFinder).dy));
    });

    testWidgets('text position left', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.success(
        iconSize: 64,
        message: "STATUS",
        messagePosition: AxisDirection.left,
      )));
      final iconFinder = find.byType(Icon);
      final textFinder = find.byType(Text);
      expect(iconFinder, findsOneWidget);
      expect(textFinder, findsOneWidget);
      expect(tester.getCenter(iconFinder).dx, lessThan(tester.getCenter(textFinder).dx));
    });

    testWidgets('text position right', (WidgetTester tester) async {
      await tester.pumpWidget(standalone(const StatusIndicator.success(
        iconSize: 64,
        message: "STATUS",
        messagePosition: AxisDirection.right,
      )));
      final iconFinder = find.byType(Icon);
      final textFinder = find.byType(Text);
      expect(iconFinder, findsOneWidget);
      expect(textFinder, findsOneWidget);
      expect(tester.getCenter(iconFinder).dx, greaterThan(tester.getCenter(textFinder).dx));
    });
  });
}
