import 'package:flutter/material.dart';
import 'package:sed_manager_gui/interface/components/snapshot_builder.dart';
import 'package:sed_manager_gui/interface/components/status_indicator.dart';

class _WizardPageRoute extends PageRoute {
  _WizardPageRoute({required this.builder});

  final Widget Function(BuildContext context) builder;

  @override
  Color? get barrierColor {
    if (navigator != null) {
      return Theme.of(navigator!.context).colorScheme.background;
    }
    return null;
  }

  @override
  String? get barrierLabel => null;

  @override
  Widget buildPage(BuildContext context, Animation<double> animation, Animation<double> secondaryAnimation) {
    return builder(context);
  }

  @override
  Widget buildTransitions(
      BuildContext context, Animation<double> animation, Animation<double> secondaryAnimation, Widget child) {
    final position = Tween<Offset>(
      begin: const Offset(1.0, 0.0),
      end: Offset.zero,
    ).animate(animation);
    return SlideTransition(position: position, child: child);
  }

  @override
  bool get maintainState => true;

  @override
  Duration get transitionDuration => const Duration(milliseconds: 200);
}

class _ValidationDialog extends StatelessWidget {
  const _ValidationDialog(this._validate, {this.next, super.key});

  final Future<void> Function() _validate;
  final Widget Function(BuildContext context)? next;

  void _stayOnPage(BuildContext context) {
    Navigator.of(context).pop(); // Pops this dialog.
  }

  void _goNextPage(BuildContext context) {
    Navigator.of(context).pop(); // Pops this dialog.
    if (next != null) {
      Navigator.of(context).push(_WizardPageRoute(builder: (context) {
        return next!(context);
      }));
    } else {
      // Pops until the first wizard page.
      Navigator.of(context).popUntil((route) => route.runtimeType != _WizardPageRoute);
      Navigator.of(context).pop(); // Pops the first wizard page.
    }
  }

  Future<void> _validatePage(BuildContext context) async {
    await _validate();
    if (context.mounted) {
      _goNextPage(context);
    }
  }

  Widget _buildContent(BuildContext context, Widget status, void Function(BuildContext)? action) {
    final actionButton = FilledButton(
      onPressed: action != null ? () => action(context) : null,
      child: const Text("OK"),
    );

    return Container(
      margin: const EdgeInsets.all(12),
      width: 280,
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          const SizedBox(height: 12),
          status,
          const SizedBox(height: 12),
          Align(alignment: Alignment.centerRight, child: actionButton),
        ],
      ),
    );
  }

  Widget _buildError(BuildContext context, Object error) {
    return _buildContent(
      context,
      StatusIndicator.error(
        message: "Error: ${error.toString()}",
        iconSize: 48,
        messagePosition: AxisDirection.down,
      ),
      _stayOnPage,
    );
  }

  Widget _buildWaiting(BuildContext context) {
    return _buildContent(
      context,
      const StatusIndicator.waiting(
        iconSize: 48,
        messagePosition: AxisDirection.down,
      ),
      null,
    );
  }

  @override
  Widget build(BuildContext context) {
    return Dialog(
      child: FutureBuilder(
        future: _validatePage(context),
        builder: (context, snapshot) {
          return SnapshotBuilder(
            snapshot,
            waiting: _buildWaiting,
            error: _buildError,
          );
        },
      ),
    );
  }
}

class WizardPage extends StatelessWidget {
  const WizardPage({
    required this.title,
    required this.onValidate,
    this.onNext,
    required this.child,
    super.key,
  });

  final String title;
  final Future<void> Function() onValidate;
  final Widget Function(BuildContext context)? onNext;
  final Widget child;

  void _onCancel(BuildContext context) {
    Navigator.of(context).popUntil((route) => route.runtimeType != _WizardPageRoute);
    Navigator.of(context).pop();
  }

  void _onNext(BuildContext context) {
    showDialog(
      barrierDismissible: false,
      context: context,
      builder: (context) => _ValidationDialog(onValidate, next: onNext),
    );
  }

  void _onFinish(BuildContext context) {
    showDialog(
      barrierDismissible: false,
      context: context,
      builder: (context) => _ValidationDialog(onValidate),
    );
  }

  @override
  Widget build(BuildContext context) {
    final cancelButton = FilledButton(onPressed: () => _onCancel(context), child: const Text("Cancel"));
    final nextButton = FilledButton(onPressed: () => _onNext(context), child: const Text("Next"));
    final finishButton = FilledButton(onPressed: () => _onFinish(context), child: const Text("Finish"));

    return Scaffold(
      appBar: AppBar(title: Text(title)),
      body: Column(
        children: [
          Expanded(child: child),
          Container(
            margin: const EdgeInsets.fromLTRB(20, 6, 20, 20),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.end,
              children: [cancelButton, const SizedBox(width: 6), onNext != null ? nextButton : finishButton],
            ),
          ),
        ],
      ),
    );
  }
}
