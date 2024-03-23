import 'package:flutter/material.dart';

enum Status {
  none,
  waiting,
  success,
  error,
}

class StatusIndicator extends StatelessWidget {
  const StatusIndicator(
    this._status, {
    this.message,
    this.messagePosition = AxisDirection.left,
    this.iconSize = 16.0,
    this.style,
    super.key,
  });

  const StatusIndicator.none({
    this.message,
    this.messagePosition = AxisDirection.left,
    this.iconSize = 16.0,
    this.style,
    super.key,
  }) : _status = Status.none;

  const StatusIndicator.waiting({
    this.message,
    this.messagePosition = AxisDirection.left,
    this.iconSize = 16.0,
    this.style,
    super.key,
  }) : _status = Status.waiting;

  const StatusIndicator.success({
    this.message,
    this.messagePosition = AxisDirection.left,
    this.iconSize = 16.0,
    this.style,
    super.key,
  }) : _status = Status.success;

  const StatusIndicator.error({
    this.message,
    this.messagePosition = AxisDirection.left,
    this.iconSize = 16.0,
    this.style,
    super.key,
  }) : _status = Status.error;

  final Status _status;
  final String? message;
  final AxisDirection messagePosition;
  final double iconSize;
  final TextStyle? style;

  Widget _buildIcon(BuildContext context) {
    if (_status == Status.error) {
      return Icon(Icons.error_outline, size: iconSize, color: Colors.red);
    } else if (_status == Status.success) {
      return Icon(Icons.check_circle_outline, size: iconSize, color: Colors.green);
    } else if (_status == Status.none) {
      return Icon(Icons.question_mark_outlined, size: iconSize, color: Colors.transparent);
    } else {
      return SizedBox(width: iconSize, height: iconSize, child: const CircularProgressIndicator());
    }
  }

  Widget _buildMessage() {
    return Text(
      message ?? "",
      style: style,
      maxLines: 1,
      overflow: TextOverflow.ellipsis,
    );
  }

  @override
  Widget build(BuildContext context) {
    final iconWidget = _buildIcon(context);
    final messageWidget = _buildMessage();
    final tooltipWidget = message != null ? Tooltip(message: message, child: messageWidget) : messageWidget;

    const spacer = SizedBox(width: 6, height: 6);
    var children = <Widget>[];
    if (messagePosition == AxisDirection.left || messagePosition == AxisDirection.down) {
      children = [iconWidget, spacer, Flexible(child: tooltipWidget)];
    } else {
      children = [Flexible(child: tooltipWidget), spacer, iconWidget];
    }

    if (messagePosition == AxisDirection.left || messagePosition == AxisDirection.right) {
      return Row(
        crossAxisAlignment: CrossAxisAlignment.center,
        mainAxisAlignment: MainAxisAlignment.center,
        mainAxisSize: MainAxisSize.min,
        children: children,
      );
    } else {
      return Column(
        crossAxisAlignment: CrossAxisAlignment.center,
        mainAxisAlignment: MainAxisAlignment.center,
        mainAxisSize: MainAxisSize.min,
        children: children,
      );
    }
  }
}
