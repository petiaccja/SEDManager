import 'package:flutter/material.dart';
import 'package:sed_manager_gui/interface/activity_launcher_page.dart';
import 'package:sed_manager_gui/interface/components/status_indicator.dart';
import '../bindings/storage_device.dart';

class StorageDeviceProperties {
  StorageDeviceProperties(this.name, this.serial, this.firmware, this.interface, this.supportedSSCs);
  final String name;
  final String serial;
  final String firmware;
  String interface;
  final List<String> supportedSSCs;
}

class StorageDeviceCard extends StatelessWidget {
  StorageDeviceCard(
    this._storageDevice, {
    required this.onConfigure,
    super.key,
  });

  final StorageDevice _storageDevice;
  final void Function() onConfigure;

  Future<StorageDeviceProperties> _getProperties() async {
    final name = _storageDevice.getName();
    final serial = _storageDevice.getSerial();
    final firmware = _storageDevice.getFirmware();
    final interface = _storageDevice.getInterface();
    final sscs = _storageDevice.getSSCs();
    return StorageDeviceProperties(name, serial, firmware, interface, sscs);
  }

  Widget _buildCard(BuildContext context, Widget child) {
    return SizedBox(
      width: 280,
      height: 180,
      child: Card(
        child: Container(margin: const EdgeInsets.all(8), child: child),
      ),
    );
  }

  Decoration? _getDriveDataDecoration(bool? parity) {
    if (parity != null) {
      return BoxDecoration(
        gradient: LinearGradient(
          colors: <Color>[
            Colors.transparent,
            (parity ? Colors.black : Colors.white).withAlpha(12),
            (parity ? Colors.black : Colors.white).withAlpha(12),
            (parity ? Colors.black : Colors.white).withAlpha(12),
            (parity ? Colors.black : Colors.white).withAlpha(12),
            Colors.transparent,
          ],
        ),
      );
    }
    return null;
  }

  Widget _buildDriveDataRow(String record, String value, {bool? parity}) {
    return Container(
      decoration: _getDriveDataDecoration(parity),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Text("$record:"),
          Text(value, overflow: TextOverflow.ellipsis),
        ],
      ),
    );
  }

  Widget _buildWithData(BuildContext context, StorageDeviceProperties data) {
    const titleStyle = TextStyle(fontSize: 18, fontWeight: FontWeight.bold);

    final sscNames = data.supportedSSCs.isNotEmpty ? data.supportedSSCs.join(', ') : "-";

    final items = <Widget>[
      Center(child: Text(data.name, style: titleStyle)),
      Divider(height: 11, thickness: 1, color: Theme.of(context).colorScheme.onSurface.withAlpha(48)),
      _buildDriveDataRow("Serial", data.serial, parity: true),
      _buildDriveDataRow("Firmware", data.firmware, parity: false),
      _buildDriveDataRow("Encryption", sscNames, parity: true),
      _buildDriveDataRow("Interface", data.interface, parity: false),
    ];

    if (data.supportedSSCs.isNotEmpty) {
      items.add(
        Expanded(
          child: Align(
            alignment: Alignment.bottomRight,
            child: FilledButton(
              onPressed: onConfigure,
              child: const Text("Configure"),
            ),
          ),
        ),
      );
    }

    final content = Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: items,
    );
    return _buildCard(context, content);
  }

  Widget _buildWithError(BuildContext context, Object error) {
    const titleStyle = TextStyle(fontSize: 18, fontWeight: FontWeight.bold);

    final name = _storageDevice.getName();

    final content = Column(
      crossAxisAlignment: CrossAxisAlignment.center,
      children: [
        Text(name, style: titleStyle),
        Expanded(child: Center(child: StatusIndicator.error(message: error.toString()))),
      ],
    );
    return _buildCard(context, content);
  }

  Widget _buildWaiting(BuildContext context) {
    const titleStyle = TextStyle(fontSize: 18, fontWeight: FontWeight.bold);

    final name = _storageDevice.getName();

    final content = Column(
      crossAxisAlignment: CrossAxisAlignment.center,
      children: [
        Text(name, style: titleStyle),
        const Expanded(child: Center(child: SizedBox(width: 48, height: 48, child: CircularProgressIndicator()))),
      ],
    );
    return _buildCard(context, content);
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _getProperties(),
      builder: (context, snapshot) {
        if (snapshot.hasData) {
          return _buildWithData(context, snapshot.data!);
        } else if (snapshot.hasError) {
          return _buildWithError(context, snapshot.error!);
        }
        return _buildWaiting(context);
      },
    );
  }
}

class DriveLauncherPage extends StatefulWidget {
  const DriveLauncherPage(this.onFinished, {super.key});

  final void Function() onFinished;

  @override
  State<DriveLauncherPage> createState() => _DriveLauncherPageState();
}

class _DriveLauncherPageState extends State<DriveLauncherPage> {
  List<StorageDevice> storageDevices = [];

  @override
  void initState() {
    storageDevices = enumerateStorageDevices();
    super.initState();
  }

  void _onConfigure(StorageDevice device) {
    Navigator.of(context).push<void>(
      MaterialPageRoute<void>(
        builder: (BuildContext context) => ActivityLauncherPage(device),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final cards = storageDevices.map((device) {
      return StorageDeviceCard(device, onConfigure: () => _onConfigure(device));
    });

    final appBar = AppBar(
      title: const Text("Storage devices"),
    );

    final body = Container(
      margin: const EdgeInsets.all(12),
      child: Wrap(
        direction: Axis.horizontal,
        spacing: 16,
        children: cards.toList(),
      ),
    );

    return Scaffold(
      appBar: appBar,
      body: body,
    );
  }
}
