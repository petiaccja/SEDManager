import 'package:flutter/material.dart';

class SnapshotBuilder<T> extends StatelessWidget {
  const SnapshotBuilder(
    this.snapshot, {
    this.none,
    this.waiting,
    this.success,
    this.error,
    super.key,
  });

  final AsyncSnapshot<T> snapshot;
  final Widget Function(BuildContext context)? none;
  final Widget Function(BuildContext context)? waiting;
  final Widget Function(BuildContext context, T? data)? success;
  final Widget Function(BuildContext context, Object error)? error;

  @override
  Widget build(BuildContext context) {
    if (snapshot.connectionState == ConnectionState.none) {
      return none?.call(context) ?? const Placeholder();
    } else if (snapshot.connectionState == ConnectionState.waiting) {
      return waiting?.call(context) ?? const Placeholder();
    } else {
      if (snapshot.error != null) {
        return error?.call(context, snapshot.error!) ?? const Placeholder();
      } else {
        return success?.call(context, snapshot.data) ?? const Placeholder();
      }
    }
  }
}
