import 'dart:async';
import 'package:flutter/material.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/interface/components/snapshot_builder.dart';
import 'package:table_sticky_headers/table_sticky_headers.dart';
import 'package:sed_manager_gui/bindings/type.dart';
import 'components/status_indicator.dart';

class TableCell extends StatelessWidget {
  const TableCell({
    this.fill = false,
    required this.child,
    super.key,
  });

  final bool fill;
  final Widget child;

  @override
  Widget build(BuildContext context) {
    final colorScheme = Theme.of(context).colorScheme;

    final border = Border.all(color: colorScheme.outlineVariant);

    return Container(
      width: double.infinity,
      height: double.infinity,
      decoration: BoxDecoration(
        border: border,
        color: fill ? colorScheme.secondary : colorScheme.surface,
      ),
      child: Container(
        margin: const EdgeInsets.all(2),
        child: child,
      ),
    );
  }
}

class TableHeaderCell extends StatelessWidget {
  const TableHeaderCell(this.name, {super.key});

  final String name;

  @override
  Widget build(BuildContext context) {
    final colorScheme = Theme.of(context).colorScheme;

    return TableCell(
      fill: true,
      child: Text(
        name,
        textAlign: TextAlign.center,
        style: TextStyle(
          fontWeight: FontWeight.bold,
          color: colorScheme.onPrimary,
        ),
        overflow: TextOverflow.ellipsis,
      ),
    );
  }
}

class TableUIDCell extends StatelessWidget {
  const TableUIDCell(
    this.encryptedDevice,
    this.object,
    this.securityProvider, {
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final UID object;
  final UID securityProvider;

  Future<String?> _getFriendlyName() async {
    try {
      return await encryptedDevice.findName(object, securityProvider: securityProvider);
    } catch (ex) {
      return null;
    }
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _getFriendlyName(),
      builder: (context, snapshot) {
        final text = snapshot.data ?? object.toRadixString(16).padLeft(16, '0');
        return Tooltip(
          waitDuration: Durations.long4,
          message: text,
          child: TableCell(
            child: Text(text, overflow: TextOverflow.ellipsis),
          ),
        );
      },
    );
  }
}

class TableCellEditDialog extends StatefulWidget {
  const TableCellEditDialog(
    this.encryptedDevice,
    this.session,
    this.object,
    this.column,
    this.type,
    this.initialValue, {
    this.onFinished,
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;
  final UID object;
  final int column;
  final Type type;
  final String initialValue;
  final void Function()? onFinished;

  @override
  State<TableCellEditDialog> createState() => _TableCellEditDialogState();
}

class _TableCellEditDialogState extends State<TableCellEditDialog> {
  var _snapshot = const AsyncSnapshot<bool>.waiting();
  late final _controller = TextEditingController(text: widget.initialValue);

  Future<void> _setValue(String value) async {
    try {
      final parsed = widget.encryptedDevice.parseValue(value, widget.type, widget.session.securityProvider);
      await widget.session.setValue(widget.object, widget.column, parsed);
      setState(() {
        _snapshot = const AsyncSnapshot<bool>.withData(ConnectionState.done, true);
      });
    } catch (ex) {
      setState(() {
        _snapshot = AsyncSnapshot<bool>.withError(ConnectionState.done, ex);
      });
    }
  }

  void _set() {
    _setValue(_controller.text);
  }

  void _close() {
    Navigator.of(context).pop();
    widget.onFinished?.call();
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final colorScheme = Theme.of(context).colorScheme;

    const title = Text("Edit cell value", style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold));

    final textEdit = TextField(
      textAlign: TextAlign.left,
      textAlignVertical: TextAlignVertical.top,
      style: const TextStyle(fontSize: 13, fontFamily: "CascadiaCode"),
      expands: true,
      maxLines: null,
      controller: _controller,
      decoration: InputDecoration(
        border: OutlineInputBorder(
          borderRadius: const BorderRadius.all(Radius.circular(6)),
          borderSide: BorderSide(color: colorScheme.outlineVariant),
        ),
        contentPadding: const EdgeInsets.all(2),
      ),
    );

    final errorStrip = _snapshot.hasData
        ? const StatusIndicator.success()
        : _snapshot.hasError
            ? StatusIndicator.error(message: _snapshot.error!.toString())
            : const StatusIndicator.none();

    final setButton = OutlinedButton(onPressed: _set, child: const Text("Set"));

    final closeButton = OutlinedButton(
      onPressed: _close,
      child: const Text("Close"),
    );

    return Dialog(
      child: FractionallySizedBox(
        widthFactor: 0.66,
        heightFactor: 0.60,
        child: Container(
          margin: const EdgeInsets.symmetric(horizontal: 18, vertical: 8),
          child: Column(
            children: [
              const Center(child: title),
              const SizedBox(height: 6),
              Expanded(child: textEdit),
              const SizedBox(height: 6),
              Row(
                mainAxisAlignment: MainAxisAlignment.end,
                children: [
                  Expanded(child: errorStrip),
                  const SizedBox(width: 6),
                  setButton,
                  const SizedBox(width: 6),
                  closeButton,
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class TableValueCell extends StatelessWidget {
  TableValueCell(
    this.encryptedDevice,
    this.session,
    this.object,
    this.column,
    this.type, {
    super.key,
  }) {
    _getValue().ignore();
  }

  final EncryptedDevice encryptedDevice;
  final Session session;
  final UID object;
  final int column;
  final Type type;
  final _controller = TextEditingController();
  final _currentValue = StreamController<String>();

  Future<void> _showFailureDialog(BuildContext context, String message) {
    return showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text("Could not set value"),
          content: Text(message),
          actions: <Widget>[
            TextButton(
              child: const Text('OK'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

  Future<void> _getValue() async {
    try {
      final value = await session.getValue(object, column);
      final rendered = value.hasValue ? encryptedDevice.renderValue(value, type, session.securityProvider) : "";
      _currentValue.add(rendered);
    } catch (ex) {
      _currentValue.addError(ex);
    }
  }

  Future<void> _setValue(BuildContext context, String value) async {
    try {
      final parsed = encryptedDevice.parseValue(value, type, session.securityProvider);
      await session.setValue(object, column, parsed);
      _getValue().ignore();
    } catch (ex) {
      if (context.mounted) {
        _showFailureDialog(context, ex.toString());
      }
    }
  }

  Widget _buildExpandable(BuildContext context, {required String initialValue, required Widget child}) {
    return GestureDetector(
      child: child,
      onDoubleTap: () {
        showDialog(
          context: context,
          builder: (context) {
            return TableCellEditDialog(
              encryptedDevice,
              session,
              object,
              column,
              type,
              initialValue,
              onFinished: () => _getValue().ignore(),
            );
          },
        );
      },
    );
  }

  Widget _buildContentWithData(BuildContext context, String data) {
    _controller.text = data;

    return TextField(
      textAlign: TextAlign.left,
      textAlignVertical: TextAlignVertical.center,
      autocorrect: false,
      readOnly: false,
      maxLines: 1,
      controller: _controller,
      decoration: const InputDecoration(
        border: OutlineInputBorder(borderRadius: BorderRadius.zero, borderSide: BorderSide.none),
        contentPadding: EdgeInsets.all(0),
      ),
      onSubmitted: (value) => _setValue(context, value).ignore(),
    );
  }

  Widget _buildContentWithError(Object error) {
    return Tooltip(
      message: error.toString(),
      child: const SizedBox(width: 14, height: 14, child: Icon(Icons.error_rounded)),
    );
  }

  Widget _buildContentWaiting() {
    return const Center(
      child: SizedBox(
        width: 16,
        height: 16,
        child: CircularProgressIndicator(),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return StreamBuilder(
      stream: _currentValue.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          waiting: (context) => TableCell(child: _buildContentWaiting()),
          success: (context, data) {
            final child = _buildContentWithData(context, data!);
            return TableCell(child: _buildExpandable(context, initialValue: data, child: child));
          },
          error: (context, error) {
            final child = _buildContentWithError(error);
            return TableCell(child: _buildExpandable(context, initialValue: "", child: child));
          },
        );
      },
    );
  }
}

class ColumnDesc {
  ColumnDesc(this.name, this.type);
  final String name;
  final Type type;
}

class TableCellView extends StatelessWidget {
  const TableCellView(
    this.encryptedDevice,
    this.session,
    this.table, {
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;
  final UID table;

  Future<(List<UID>, List<ColumnDesc>)> _getLayout() async {
    final rows = await session.getTableRows(table).toList();
    final columns = <ColumnDesc>[];
    for (int column = 0; column < session.getColumnCount(table); ++column) {
      columns.add(ColumnDesc(
        session.getColumnName(table, column),
        session.getColumnType(table, column),
      ));
    }
    return (rows, columns);
  }

  Widget _buildWithData(
    BuildContext context,
    List<UID> rows,
    List<ColumnDesc> columns,
  ) {
    Widget headerBuilder(columnIdx) {
      return TableHeaderCell(columns[columnIdx + 1].name);
    }

    Widget rowBuilder(rowIdx) {
      return TableUIDCell(encryptedDevice, rows[rowIdx], session.securityProvider);
    }

    Widget valueBuilder(columnIdx, rowIdx) {
      return TableValueCell(
        encryptedDevice,
        session,
        rows[rowIdx],
        columnIdx + 1,
        columns[columnIdx + 1].type,
      );
    }

    final cellDimensions = CellDimensions.variableColumnWidth(
      columnWidths: List<double>.filled(columns.length - 1, 120.0),
      contentCellHeight: 26,
      stickyLegendWidth: 256,
      stickyLegendHeight: 26,
    );

    return StickyHeadersTable(
      columnsLength: columns.length - 1,
      rowsLength: rows.length,
      columnsTitleBuilder: headerBuilder,
      rowsTitleBuilder: rowBuilder,
      contentCellBuilder: valueBuilder,
      legendCell: headerBuilder(-1),
      showHorizontalScrollbar: true,
      showVerticalScrollbar: true,
      cellDimensions: cellDimensions,
    );
  }

  Widget _buildWithError(Object error) {
    return Text(error.toString());
  }

  Widget _buildWaiting() {
    return const Column(children: [
      SizedBox(
        width: 48,
        height: 48,
        child: CircularProgressIndicator(),
      ),
      Text("Loading layout..."),
    ]);
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _getLayout(),
      builder: (context, snapshot) {
        if (snapshot.hasData) {
          final rows = snapshot.data!.$1;
          final columns = snapshot.data!.$2;
          return _buildWithData(context, rows, columns);
        } else if (snapshot.hasError) {
          return _buildWithError(snapshot.error!);
        }
        return _buildWaiting();
      },
    );
  }
}
