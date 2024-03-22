import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';

class ObjectDropdown extends StatelessWidget {
  const ObjectDropdown(
    this.objects, {
    this.width,
    this.onSelected,
    this.hintText,
    this.enabled = true,
    super.key,
  });

  final List<(UID, String)> objects;
  final void Function(UID object)? onSelected;
  final double? width;
  final String? hintText;
  final bool enabled;

  @override
  Widget build(BuildContext context) {
    final items = objects.map((sp) {
      return DropdownMenuEntry<UID>(value: sp.$1, label: sp.$2);
    }).toList();

    return DropdownMenu(
      onSelected: (int? value) {
        if (value != null) {
          onSelected?.call(value);
        }
      },
      dropdownMenuEntries: items,
      controller: SearchController(),
      hintText: objects.isNotEmpty ? hintText : "Empty",
      enabled: enabled && objects.isNotEmpty,
      width: width,
    );
  }
}
