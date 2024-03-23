import 'dart:async';
import 'package:flutter/material.dart';
import "package:sed_manager_gui/bindings/encrypted_device.dart";

class SessionBuilder extends StatefulWidget {
  SessionBuilder(
    this.encryptedDevice,
    this.securityProvider, {
    required this.builder,
  }) : super(key: UniqueKey());
  final EncryptedDevice encryptedDevice;
  final UID securityProvider;
  final Widget Function(BuildContext context, AsyncSnapshot<Session> session) builder;

  @override
  State<SessionBuilder> createState() => _SessionBuilderState();
}

class _SessionBuilderState extends State<SessionBuilder> {
  Session? _session;

  @override
  void deactivate() {
    _session?.end().ignore();
    super.deactivate();
  }

  Future<Session> _getSession() async {
    _session = await widget.encryptedDevice.login(widget.securityProvider);
    return _session!;
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _getSession(),
      builder: widget.builder,
    );
  }
}
