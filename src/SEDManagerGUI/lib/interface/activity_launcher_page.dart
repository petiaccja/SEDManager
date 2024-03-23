import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/storage_device.dart';
import 'package:sed_manager_gui/interface/locking_wizard.dart';
import 'factory_reset_wizard.dart';
import 'stack_reset_page.dart';
import 'table_editor_page.dart';

class ActivityLauncherPage extends StatelessWidget {
  const ActivityLauncherPage(this.device, {super.key});

  final StorageDevice device;

  void _launchActivity(
    BuildContext context,
    Widget Function(BuildContext) builder,
  ) {
    Navigator.of(context).push<void>(
      MaterialPageRoute<void>(
        builder: builder,
      ),
    );
  }

  void _launchTableEditor(BuildContext context) {
    _launchActivity(context, (BuildContext context) => TableEditorPage(device));
  }

  void _launchLockingWizard(BuildContext context) {
    _launchActivity(context, (BuildContext context) => LockingWizard(device));
  }

  void _launchStackReset(BuildContext context) {
    _launchActivity(context, (BuildContext context) => StackResetPage(device));
  }

  void _launchFactoryReset(BuildContext context) {
    _launchActivity(context, (BuildContext context) => FactoryResetWizard(device));
  }

  Widget _buildIcon(BuildContext context, String caption, IconData icon, void Function()? callback) {
    var face = Container(
      width: 90,
      height: 86,
      margin: const EdgeInsets.fromLTRB(4, 4, 4, 4),
      child: Column(
        children: [
          Icon(
            icon,
            size: 45,
          ),
          Text(caption, maxLines: 2, overflow: TextOverflow.ellipsis, textAlign: TextAlign.center),
        ],
      ),
    );
    final style = ButtonStyle(
      shape: MaterialStatePropertyAll(
        RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(8),
        ),
      ),
    );
    return TextButton(onPressed: callback, style: style, child: face);
  }

  @override
  Widget build(BuildContext context) {
    late final String title = "${device.getName()} - ${device.getSerial()}";

    final icons = <Widget>[
      _buildIcon(context, "Table editor", Icons.table_chart_outlined, () => _launchTableEditor(context)),
      _buildIcon(context, "Locking wizard", Icons.lock_outline_rounded, () => _launchLockingWizard(context)),
      _buildIcon(context, "Change password", Icons.password, null),
      _buildIcon(context, "Stack reset", Icons.restart_alt_rounded, () => _launchStackReset(context)),
      _buildIcon(context, "Factory reset", Icons.clear_rounded, () => _launchFactoryReset(context)),
    ];

    return Scaffold(
      appBar: AppBar(title: Text(title)),
      body: Container(
        margin: const EdgeInsets.all(12),
        child: Wrap(
          runSpacing: 6,
          spacing: 6,
          crossAxisAlignment: WrapCrossAlignment.center,
          alignment: WrapAlignment.center,
          direction: Axis.horizontal,
          children: icons,
        ),
      ),
    );
  }
}
