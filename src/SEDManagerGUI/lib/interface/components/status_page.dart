import 'package:flutter/material.dart';
import 'status_indicator.dart';

class StatusPage extends StatelessWidget {
  const StatusPage.waiting({
    this.message,
    this.onClose,
    this.messagePosition = AxisDirection.down,
    this.iconSize = 48.0,
    this.style,
    super.key,
  }) : _status = Status.waiting;

  const StatusPage.success({
    this.message,
    this.onClose,
    this.messagePosition = AxisDirection.down,
    this.iconSize = 48.0,
    this.style,
    super.key,
  }) : _status = Status.success;

  const StatusPage.error({
    this.message,
    this.onClose,
    this.messagePosition = AxisDirection.down,
    this.iconSize = 48.0,
    this.style,
    super.key,
  }) : _status = Status.error;

  final Status _status;
  final String? message;
  final void Function()? onClose;
  final AxisDirection messagePosition;
  final double iconSize;
  final TextStyle? style;

  String _closeButtonText() {
    switch (_status) {
      case Status.waiting:
        return "Cancel";
      case Status.error:
        return "Back";
      case Status.success:
        return "Done";
      default:
        return "Close";
    }
  }

  @override
  Widget build(BuildContext context) {
    final indicator = StatusIndicator(
      _status,
      message: message,
      iconSize: iconSize,
      messagePosition: messagePosition,
      style: style,
    );

    final closeButton = FilledButton(
      onPressed: onClose,
      child: Text(_closeButtonText()),
    );

    final elements = onClose == null
        ? <Widget>[indicator]
        : <Widget>[
            Flexible(flex: 1, child: FractionallySizedBox(widthFactor: 0.8, child: Center(child: indicator))),
            Align(alignment: Alignment.centerRight, child: closeButton),
          ];

    return Container(
      margin: const EdgeInsets.all(20),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        mainAxisSize: MainAxisSize.max,
        children: elements,
      ),
    );
  }
}
