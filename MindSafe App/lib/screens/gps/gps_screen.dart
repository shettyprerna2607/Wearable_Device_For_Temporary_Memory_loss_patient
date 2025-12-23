import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';
import 'gps_controller.dart';

class GpsScreen extends StatefulWidget {
  const GpsScreen({Key? key}) : super(key: key);

  @override
  State<GpsScreen> createState() => _GpsScreenState();
}

class _GpsScreenState extends State<GpsScreen> {
  final GpsController _controller = GpsController();

  String? _deviceMac;
  String? _alertMessage;
  double? _latestLat;
  double? _latestLng;

  final LatLng _safeZoneCenter = const LatLng(13.254087, 74.784485);
  final double _safeZoneRadius = 300;

  LatLng? get _currentLocation =>
      (_latestLat != null && _latestLng != null) ? LatLng(_latestLat!, _latestLng!) : null;

  void _checkGeofence(LatLng loc) {
    final distance = Distance().as(LengthUnit.Meter, _safeZoneCenter, loc);
    setState(() {
      _alertMessage = (distance > _safeZoneRadius)
          ? "⚠️ Patient out of safe zone!"
          : null;
    });
  }

  @override
  void initState() {
    super.initState();

    _controller.deviceMacStream.listen((mac) {
      setState(() {
        _deviceMac = mac;
      });
    });

    _controller.alertStream.listen((alert) {
      setState(() {
        _alertMessage = alert;
      });
    });

    _controller.latitudeStream.listen((lat) {
      setState(() {
        _latestLat = lat;
      });
      if (lat != null && _latestLng != null) {
        _checkGeofence(LatLng(lat, _latestLng!));
      }
    });

    _controller.longitudeStream.listen((lng) {
      setState(() {
        _latestLng = lng;
      });
      if (_latestLat != null && lng != null) {
        _checkGeofence(LatLng(_latestLat!, lng));
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    final Color purpleStart = const Color(0xFFDAD7FE);
    final Color purpleEnd = const Color(0xFFF5E9FC);
    final Color purpleAccent = Colors.deepPurpleAccent;
    const String geoapifyApiKey = " ";

    return Scaffold(
      appBar: AppBar(
        title: const Text('Device GPS Tracking'),
        backgroundColor: purpleAccent,
      ),
      body: Container(
        decoration: BoxDecoration(
          gradient: LinearGradient(
            colors: [purpleStart, purpleEnd],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
        ),
        padding: const EdgeInsets.all(20),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            if (_deviceMac != null)
              Container(
                padding: const EdgeInsets.symmetric(vertical: 12, horizontal: 18),
                margin: const EdgeInsets.only(bottom: 20),
                decoration: BoxDecoration(
                  color: purpleAccent.withOpacity(0.14),
                  borderRadius: BorderRadius.circular(15),
                  boxShadow: [
                    BoxShadow(
                      color: Colors.deepPurpleAccent.withOpacity(0.09),
                      blurRadius: 12,
                      spreadRadius: 1,
                      offset: const Offset(0, 4),
                    ),
                  ],
                ),
                child: Text(
                  'Device MAC: $_deviceMac',
                  style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.black87),
                ),
              ),
            Expanded(
              child: _currentLocation == null
                  ? Center(
                child: Text(
                  'Waiting for location...',
                  style: TextStyle(color: purpleAccent),
                ),
              )
                  : FlutterMap(
                options: MapOptions(
                  center: _currentLocation,
                  zoom: 16,
                ),
                children: [
                  TileLayer(
                    urlTemplate:
                    "https://maps.geoapify.com/v1/tile/osm-bright/{z}/{x}/{y}.png?apiKey=$geoapifyApiKey",
                    userAgentPackageName: 'com.example.patient_watch_app',
                  ),
                  CircleLayer(
                    circles: [
                      CircleMarker(
                        point: _safeZoneCenter,
                        radius: _safeZoneRadius,
                        useRadiusInMeter: true,
                        color: Colors.blueAccent.withOpacity(0.1),
                        borderStrokeWidth: 3,
                        borderColor: Colors.blueAccent,
                      )
                    ],
                  ),
                  MarkerLayer(
                    markers: [
                      Marker(
                        point: _currentLocation!,
                        width: 40,
                        height: 40,
                        child: const Icon(
                          Icons.location_on,
                          size: 40,
                          color: Colors.redAccent,
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
            if (_alertMessage != null)
              Container(
                width: double.infinity,
                padding: const EdgeInsets.all(18),
                margin: const EdgeInsets.only(top: 10),
                decoration: BoxDecoration(
                  color: Colors.red.withOpacity(0.2),
                  borderRadius: BorderRadius.circular(15),
                ),
                child: Row(
                  children: [
                    const Icon(Icons.warning, color: Colors.red, size: 28),
                    const SizedBox(width: 12),
                    Expanded(
                      child: Text(
                        _alertMessage!,
                        style: const TextStyle(
                            color: Colors.red,
                            fontWeight: FontWeight.bold,
                            fontSize: 18),
                      ),
                    ),
                  ],
                ),
              ),
            if (_alertMessage == null)
              Container(
                padding: const EdgeInsets.symmetric(vertical: 18),
                child: const Text(
                  'Patient is within safe zone',
                  style: TextStyle(
                    color: Colors.green,
                    fontWeight: FontWeight.w600,
                    fontSize: 18,
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }
}
