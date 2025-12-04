import 'package:firebase_database/firebase_database.dart';

class ReminderController {
  final DatabaseReference _morningRef = FirebaseDatabase.instance.ref('pillbox/morning');
  final DatabaseReference _afternoonRef = FirebaseDatabase.instance.ref('pillbox/afternoon');
  final DatabaseReference _eveningRef = FirebaseDatabase.instance.ref('pillbox/evening');

  Stream<bool?> get morningStream => _morningRef.onValue.map(
        (event) => event.snapshot.value != null ? event.snapshot.value as bool : null,
  );

  Stream<bool?> get afternoonStream => _afternoonRef.onValue.map(
        (event) => event.snapshot.value != null ? event.snapshot.value as bool : null,
  );

  Stream<bool?> get eveningStream => _eveningRef.onValue.map(
        (event) => event.snapshot.value != null ? event.snapshot.value as bool : null,
  );

  Future<void> resetAllReminders() async {
    await FirebaseDatabase.instance.ref('pillbox').update({
      "morning": false,
      "afternoon": false,
      "evening": false,
    });
  }
}
