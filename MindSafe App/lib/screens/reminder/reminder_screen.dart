import 'package:flutter/material.dart';
import 'reminder_controller.dart';

class ReminderScreen extends StatefulWidget {
  const ReminderScreen({Key? key}) : super(key: key);

  @override
  State<ReminderScreen> createState() => _ReminderScreenState();
}

class _ReminderScreenState extends State<ReminderScreen> {
  final ReminderController _controller = ReminderController();

  Widget _buildReminderCard(String label, bool? taken) {
    final Color takenColor = Colors.greenAccent.shade100;
    final Color pendingColor = Colors.orangeAccent.shade100;
    final Color bgColor = taken == true ? takenColor : pendingColor;
    final IconData icon = taken == true ? Icons.check_circle_rounded : Icons.access_time_rounded;
    final Color iconColor = taken == true ? Colors.green : Colors.orange;

    return Container(
      margin: const EdgeInsets.symmetric(vertical: 12, horizontal: 16),
      decoration: BoxDecoration(
        color: bgColor.withOpacity(0.72),
        borderRadius: BorderRadius.circular(18),
        boxShadow: [
          BoxShadow(
            color: bgColor.withOpacity(0.17),
            blurRadius: 15,
            spreadRadius: 1,
            offset: const Offset(0, 5),
          ),
        ],
      ),
      child: ListTile(
        leading: Icon(icon, color: iconColor, size: 35),
        title: Text(
          label,
          style: TextStyle(
            fontSize: 22,
            fontWeight: FontWeight.bold,
            color: iconColor,
          ),
        ),
        subtitle: Text(
          taken == true ? '$label pill taken' : 'Reminder pending',
          style: TextStyle(
            fontSize: 16,
            color: taken == true ? Colors.green.shade800 : Colors.orange.shade800,
            fontWeight: FontWeight.w500,
            letterSpacing: 0.2,
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final Color purpleStart = const Color(0xFFDAD7FE);
    final Color purpleEnd = const Color(0xFFF5E9FC);
    final Color purpleAccent = Colors.deepPurpleAccent;

    return Scaffold(
      appBar: AppBar(
        title: const Text('Medication Reminder'),
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
        child: Column(
          children: [
            const SizedBox(height: 32),
            _buildHeader(),

            StreamBuilder<bool?>(
              stream: _controller.morningStream,
              builder: (context, snapshot) => _buildReminderCard('Morning', snapshot.data),
            ),
            StreamBuilder<bool?>(
              stream: _controller.afternoonStream,
              builder: (context, snapshot) => _buildReminderCard('Afternoon', snapshot.data),
            ),
            StreamBuilder<bool?>(
              stream: _controller.eveningStream,
              builder: (context, snapshot) => _buildReminderCard('Evening', snapshot.data),
            ),

            const SizedBox(height: 25),

            // Reset button
            ElevatedButton(
              onPressed: () async {
                await _controller.resetAllReminders();
                ScaffoldMessenger.of(context).showSnackBar(
                  const SnackBar(content: Text("Reminders reset successfully")),
                );
              },
              style: ElevatedButton.styleFrom(
                backgroundColor: Colors.deepPurpleAccent,
                padding: const EdgeInsets.symmetric(horizontal: 28, vertical: 14),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(12),
                ),
              ),
              child: const Text(
                "Reset Reminders",
                style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildHeader() {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 12.0, horizontal: 20),
      child: Row(
        children: [
          Container(
            padding: const EdgeInsets.all(10),
            decoration: BoxDecoration(
              color: Colors.deepPurpleAccent.withOpacity(0.14),
              borderRadius: BorderRadius.circular(13),
            ),
            child: const Icon(Icons.medication, size: 28, color: Colors.deepPurpleAccent),
          ),
          const SizedBox(width: 18),
          const Text(
            "Daily Pill Reminders",
            style: TextStyle(
              color: Colors.deepPurpleAccent,
              fontWeight: FontWeight.bold,
              fontSize: 22,
              letterSpacing: 0.5,
            ),
          ),
        ],
      ),
    );
  }
}
