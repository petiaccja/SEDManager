import 'dart:async';

import 'package:flutter/material.dart';
import 'package:marquee_widget/marquee_widget.dart';
import 'package:sed_manager_gui/bindings/encrypted_device.dart';
import 'package:sed_manager_gui/bindings/storage_device.dart';
import 'package:sed_manager_gui/interface/components/status_indicator.dart';
import 'package:sed_manager_gui/interface/components/utility.dart';
import 'package:sed_manager_gui/interface/table_cell_view.dart';
import 'components/encrypted_device_builder.dart';
import 'components/session_builder.dart';
import 'components/cached_stream.dart';
import 'components/snapshot_builder.dart';
import 'components/status_page.dart';
import 'table_editor_tools_view.dart';
import 'components/object_dropdown.dart';

class SecurityProviderDropdown extends StatelessWidget {
  SecurityProviderDropdown({
    this.onSelected,
    super.key,
  });

  SecurityProviderDropdown.fetch(
    EncryptedDevice encryptedDevice, {
    this.onSelected,
    super.key,
  }) {
    () async {
      final admin = await encryptedDevice.findUid("SP::Admin");
      final session = await encryptedDevice.login(admin);
      try {
        update(await getSecurityProviders(encryptedDevice, session));
      } finally {
        await session.end();
      }
    }()
        .ignore();
  }

  final void Function(UID)? onSelected;
  final _securityProviderStream = StreamController<List<(UID, String)>>();

  void update(List<(UID, String)> securityProviders) {
    _securityProviderStream.add(securityProviders);
  }

  static Future<bool> _isSecurityProviderActive(UID subjectSp, Session session) async {
    const issued = 0;
    const disabled = 1;
    const manufactured = 9;
    const manufacturedDisabled = 10;
    try {
      final lifeCycleState = (await session.getValue(subjectSp, 6)).getInteger();
      final canOpenSession = <int>{issued, disabled, manufactured, manufacturedDisabled}.contains(lifeCycleState);
      return canOpenSession;
    } catch (ex) {
      return true;
    }
  }

  static Future<List<(UID, String)>> getSecurityProviders(EncryptedDevice encryptedDevice, Session session) async {
    final table = await encryptedDevice.findUid("SP");
    final securityProviders = <(UID, String)>[];
    await for (final sp in session.getTableRows(table)) {
      if (await _isSecurityProviderActive(sp, session)) {
        securityProviders.add((sp, await getDisplayName(sp, encryptedDevice)));
      }
    }
    return securityProviders;
  }

  @override
  Widget build(BuildContext context) {
    return StreamBuilder(
      stream: _securityProviderStream.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          waiting: (context) => const ObjectDropdown(
            [],
            hintText: "Loading...",
            enabled: false,
            width: 240,
          ),
          success: (context, sps) => ObjectDropdown(
            sps!,
            hintText: "Select security provider",
            width: 240,
            onSelected: onSelected,
          ),
          error: (context, error) => Tooltip(
            message: error.toString(),
            child: const ObjectDropdown(
              [],
              enabled: false,
              hintText: "Error",
              width: 240,
            ),
          ),
        );
      },
    );
  }
}

class TableRowListView extends StatelessWidget {
  const TableRowListView(
    this.encryptedDevice,
    this.session, {
    this.onSelected,
    super.key,
  });

  final EncryptedDevice encryptedDevice;
  final Session session;
  final void Function(UID)? onSelected;

  Future<List<(UID, String)>> _getTables() async {
    final tableTable = await encryptedDevice.findUid("Table", securityProvider: session.securityProvider);

    List<(UID, String)> tables = [];
    try {
      await for (final tableDesc in session.getTableRows(tableTable)) {
        final table = tableDesc << 32;
        try {
          final name = await encryptedDevice.findName(table);
          tables.add((table, name));
        } catch (ex) {
          tables.add((table, table.toRadixString(16).padLeft(16, '0')));
        }
      }
      return tables;
    } catch (ex) {
      rethrow;
    }
  }

  Widget _buildBody(BuildContext context, List<(UID, String)> tables) {
    final entries = tables.map((table) {
      return NavigationDrawerDestination(
        icon: Icon(IconData(table.$2.codeUnits[0])),
        label: Text(table.$2),
      );
    }).toList();

    final header = Container(
      margin: const EdgeInsets.all(6),
      child: Center(
        child: Text(
          "Tables",
          style: TextStyle(
            fontSize: 18,
            fontWeight: FontWeight.bold,
            color: Theme.of(context).colorScheme.primary,
          ),
        ),
      ),
    );

    final selected = StreamController<int>();
    return StreamBuilder(
      stream: selected.stream,
      builder: (context, snapshot) {
        return NavigationDrawer(
          tilePadding: const EdgeInsets.symmetric(horizontal: 8),
          onDestinationSelected: (value) {
            onSelected?.call(tables[value].$1);
            selected.add(value);
          },
          selectedIndex: snapshot.data,
          children: [header, ...entries],
        );
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _getTables(),
      builder: (context, snapshot) {
        return SizedBox(
          width: 200,
          child: SnapshotBuilder(
            snapshot,
            waiting: (context) => const StatusIndicator.waiting(
              message: "Loading tables...",
              iconSize: 48,
              messagePosition: AxisDirection.down,
            ),
            error: (context, error) => StatusIndicator.waiting(
              message: "Error: $error",
              iconSize: 48,
              messagePosition: AxisDirection.down,
            ),
            success: (context, data) => _buildBody(context, data!),
          ),
        );
      },
    );
  }
}

class TableEditorPage extends StatelessWidget {
  TableEditorPage(this.storageDevice, {super.key});
  final StorageDevice storageDevice;
  final securityProviderStream = StreamController<UID>();

  void _onSecurityProvider(UID securityProvider) {
    securityProviderStream.add(securityProvider);
  }

  static Stream<Set<UID>> _accumulateAuthorities(Stream<UID> authorities) async* {
    Set<UID> collection = {};
    await for (final authority in authorities) {
      collection.add(authority);
      yield collection;
    }
  }

  static Stream<List<String>> _stringifyAuthoritySets(
    EncryptedDevice encryptedDevice,
    Stream<Set<UID>> authoritySets,
    UID securityProvider,
  ) async* {
    await for (final authoritySet in authoritySets) {
      final nameSet = <String>[];
      for (final authority in authoritySet) {
        try {
          nameSet.add(await encryptedDevice.findName(authority, securityProvider: securityProvider));
        } catch (ex) {
          nameSet.add(authority.toRadixString(16).padLeft(16, '0'));
        }
      }
      nameSet.sort();
      yield nameSet;
    }
  }

  static Widget _buildSessionContent(
    BuildContext context,
    EncryptedDevice encryptedDevice,
    Session session, {
    void Function(UID authority)? onAuthenticated,
    void Function(UID securityProvider)? onActivated,
  }) {
    final tableStream = StreamController<UID>();
    final cachedTableStream = CachedStream(tableStream.stream);
    final authorityStream = StreamController<UID>();
    final accumulatedAuthorityStream = _stringifyAuthoritySets(
      encryptedDevice,
      _accumulateAuthorities(authorityStream.stream),
      session.securityProvider,
    );

    final authoritiesView = StreamBuilder(
      stream: accumulatedAuthorityStream,
      builder: (context, snapshot) {
        final text = (snapshot.data ?? <String>["Anybody"]).join("   ");
        return Marquee(child: Text(text, style: const TextStyle(color: Colors.green)));
      },
    );

    final tableListView = TableRowListView(
      encryptedDevice,
      session,
      onSelected: (table) {
        tableStream.add(table);
      },
    );

    final tableView = StreamBuilder(
      stream: cachedTableStream.stream,
      builder: (context, snapshot) {
        if (snapshot.hasData) {
          final table = snapshot.data!;
          return TableCellView(
            encryptedDevice,
            session,
            table,
            key: ObjectKey((session, table)),
          );
        }
        return const Center(child: Text("Select a table."));
      },
    );

    final toolsView = TableEditorToolsView(
      encryptedDevice,
      session,
      onAuthenticated: (authority) {
        authorityStream.add(authority);
        if (cachedTableStream.latest != null) {
          tableStream.add(cachedTableStream.latest!);
        }
        onAuthenticated?.call(authority);
      },
      onActivated: onActivated,
    );

    return Row(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        tableListView,
        const SizedBox(width: 12),
        Expanded(
            child:
                Column(children: [Expanded(flex: 1, child: tableView), SizedBox(height: 32, child: authoritiesView)])),
        const SizedBox(width: 12),
        Center(child: toolsView),
      ],
    );
  }

  static Widget _buildSession(
    BuildContext context,
    EncryptedDevice encryptedDevice,
    UID securityProvider, {
    void Function(UID authority)? onAuthenticated,
    void Function(UID securityProvider)? onActivated,
  }) {
    return SessionBuilder(
      encryptedDevice,
      securityProvider,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          waiting: (context) => const StatusPage.waiting(message: "Starting session..."),
          error: (context, error) => StatusPage.error(message: "Session failed: $error"),
          success: (context, data) => _buildSessionContent(
            context,
            encryptedDevice,
            snapshot.data!,
            onAuthenticated: onAuthenticated,
            onActivated: onActivated,
          ),
        );
      },
    );
  }

  Widget _buildBody(EncryptedDevice encryptedDevice) {
    final securityProviderDropdown = SecurityProviderDropdown.fetch(
      encryptedDevice,
      onSelected: _onSecurityProvider,
    );

    const selectionPrompt = Center(child: Text("Select a security provider."));

    final sessionPanel = StreamBuilder(
      stream: securityProviderStream.stream,
      builder: (context, snapshot) {
        return SnapshotBuilder(
          snapshot,
          none: (context) => selectionPrompt,
          waiting: (context) => selectionPrompt,
          error: (context, error) => const StatusIndicator.error(message: "no security providers available."),
          success: (context, securityProvider) {
            return SessionBuilder(
              encryptedDevice,
              securityProvider!,
              builder: (context, snapshot) {
                return SnapshotBuilder(
                  snapshot,
                  waiting: (context) => const StatusPage.waiting(message: "Starting session..."),
                  error: (context, error) => StatusPage.error(message: "Session failed: $error"),
                  success: (context, session) => _buildSessionContent(
                    context,
                    encryptedDevice,
                    session!,
                    onAuthenticated: null,
                    onActivated: (securityProvider) async => securityProviderDropdown
                        .update(await SecurityProviderDropdown.getSecurityProviders(encryptedDevice, session)),
                  ),
                );
              },
            );
          },
        );
      },
    );

    return Center(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          const SizedBox(height: 7),
          securityProviderDropdown,
          const Divider(height: 13, thickness: 1, indent: 8, endIndent: 8),
          Expanded(child: sessionPanel),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Table editor")),
      body: EncryptedDeviceBuilder(
        storageDevice,
        builder: (context, snapshot) {
          return SnapshotBuilder(
            snapshot,
            error: (context, error) => StatusPage.error(
              message: "Failed to open device: $error",
              onClose: () => Navigator.of(context).pop(),
            ),
            waiting: (context) => StatusPage.waiting(
              message: "Opening device...",
              onClose: () => Navigator.of(context).pop(),
            ),
            success: (context, data) => _buildBody(data!),
          );
        },
      ),
    );
  }
}
