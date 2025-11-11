 ### Wearable Device for Temporary Memory Loss Patient

### 💡 Overview

This project is an IoT-based wearable system designed to help patients suffering from temporary memory loss. 
It continuously monitors vital signs, tracks location, detects falls, and reminds the patient to take medication through a smart pill box.
All data is uploaded to Firebase Realtime Database and viewed via a connected Android app for caregivers.

---

## ⚙️ System Architecture

### 🧩 Components

| Module             | Function                                     |
| ------------------ | -------------------------------------------- |
| **ESP32-32**       | Main controller for wearable device          |
| **MAX30100**       | Heart rate & SpO₂ measurement                |
| **LM75A**          | Body temperature sensor                      |
| **Neo-6M GPS**     | Location tracking                            |
| **MPU6050**        | Fall detection                               |
| **OLED**           | Displaying medication reminder & vitals      |
| **Buzzer**         | Alerts and medication reminders              |
| **ESP8266**        | Smart pill box controller                    |
| **Firebase**       | Cloud storage for vitals, alerts, and logs   |
| **Android App**    | Live data visualization and caregiver alerts |

---

## 🌟 Features

### 1. 🩺 **Health Monitoring**

* Measures **temperature**, heart rate, temperature and SpO₂ in real time.
* Sends live data to **Firebase** every few seconds.
* Displays vitals on OLED and Android app.

### 2. 🚨 **Fall Detection**

* Detects sudden acceleration using MPU6050.
* Sends a “Fall Detected” alert to Firebase and the caregiver app.
* Triggers an immediate buzzer sound.

### 3. 📍 **GPS Tracking & Safe Zone Alert**

* Tracks patient’s location using **Neo-6M GPS**.
* Displays a connecting animation (“✨ Connecting to GPS ✨”) until fix acquired.
* Alerts when the patient moves outside a predefined safe zone (e.g., 10 meters).

### 4. 💊 **Medication Reminder & Smart Pill Box**

* Scheduled medication times (e.g., 10:55 AM, 1:00 PM, 9:00 PM).
* OLED + buzzer alerts to remind the patient.
* **Pill box (ESP8266)** has buttons for morning, afternoon, and evening pills.
* Caregiver can see pill status and medication compliance in the app.

---

## 🖥️ System Components

### 🧠 **1. ESP32 Wearable Code**

* Sends live vitals, GPS, and fall alerts to Firebase.
* Displays data on OLED.
* Handles geofencing and medication reminders.

### 💊 **2. ESP8266 Smart Pill Box Code**

* Each button press marks a pill (morning/afternoon/evening) as “taken.”
* Continuous buzzer alert until the pill is confirmed.
* Syncs pill intake logs to Firebase.

### 📱 **3. Android App**

* Built in Android Studio.
* Displays:
  * Real-time vitals (Temp, HR, SpO₂)
  * Location (Safe/Danger Zone)
  * Fall status
  * Pill reminder alerts
* Connects with Firebase Realtime Database using API key & project URL.
* Provides a simple caregiver dashboard for live updates.



## 👩‍💻 Team
Meghana M 
Prerana A Shetty
Princia D'Almeida
Sana 
