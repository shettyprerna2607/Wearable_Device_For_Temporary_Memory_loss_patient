# 🧠 Mind Safe

**Mind Safe** is a Flutter-based mobile application designed to assist caregivers in monitoring patients with **early-stage amnesia**.  
The app connects with IoT-based wearable devices and Firebase to provide real-time health data and safety alerts.

---

## ✨ Features

❤️ **Real-time Health Monitoring**  
Displays live vitals such as **heart rate**, **SpO₂**, and **body temperature** fetched directly from Firebase in real-time.

📍 **GPS Tracking Alerts**  
Tracks the patient’s live location and sends alerts if they move outside a designated **safe zone**.

🛑 **Fall Detection System**  
Automatically detects patient falls and immediately notifies caregivers.  
Includes an **emergency contact list** for quick calls or messages during emergencies.

💊 **Medication Reminders**  
Schedules and tracks medication times, ensuring patients take their medicines on time.  
Displays indicators for missed or completed doses.

---

## 🛠️ Tech Stack

| 🧩 Technology          | ⚙️ Purpose                                     |
|------------------------|-----------------------------------------------|
| **Flutter**            | Cross-platform mobile development              |
| **Firebase Auth**      | User authentication                            |
| **Firebase Firestore** | Cloud database for storing patient data        |
| **flutter_map + OSM**  | GPS tracking and live map visualization        |
| **Provider**           | State management                               |
| **IoT Integration**    | Real-time data from wearable devices           |

---
