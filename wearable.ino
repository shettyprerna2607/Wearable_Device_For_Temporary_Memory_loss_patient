/***********************************************************************************************
   ESP32 Wearable Device for Early Stage Amnesia Patient
************************************************************************************************/

#include <WiFi.h>
#include <Wire.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <M2M_LM75A.h>
#include <TimeLib.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <MAX30100_PulseOximeter.h>

// ---------------- Firebase Config ----------------
#define WIFI_SSID " "
#define WIFI_PASSWORD " "
#define API_KEY " "
#define DATABASE_URL " "

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- Sensors ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial GPS_Serial(2); 
M2M_LM75A tempSensor;
PulseOximeter pox; 

#define BUZZER_PIN 25
#define SAFE_LAT 13.254087
#define SAFE_LON 74.784485
#define SAFE_RADIUS 10.0 

float heartRate = 0.0, spo2 = 0.0;
float latitude, longitude;
bool fallDetected = false;

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 3000; 

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);

  GPS_Serial.begin(9600, SERIAL_8N1, 16, 17); 
  Wire.begin(21, 22);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ---------------- OLED ----------------
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED not found!");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  // ---------------- WiFi ----------------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // ---------------- Firebase ----------------
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.signUp(&config, &auth, "", ""); 
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("✅ Firebase connected");

  // ---------------- MPU6050 ----------------
  if (!mpu.begin()) {
    Serial.println("❌ MPU6050 not found!");
    while (true);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  tempSensor.begin();

  // ---------------- MAX30100 ----------------
  Serial.println("Initializing MAX30100 Pulse Oximeter...");
  if (!pox.begin()) {
    Serial.println("❌ MAX30100 not found. Check wiring!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("❌ MAX30100 not found!");
    display.display();
    while (true);
  } else {
    Serial.println("✅ MAX30100 Ready");
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wearable Ready ✅");
  display.display();
}

// ---------------- Loop ----------------
void loop() {
  pox.update(); 
  heartRate = pox.getHeartRate();
  spo2 = pox.getSpO2();

  // ---------------- GPS Read ----------------
  while (GPS_Serial.available()) gps.encode(GPS_Serial.read());

  // ---------------- GPS & Safe Zone ----------------
  static int dotCount = 0;
  if (gps.location.isValid()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();

    double dist = TinyGPSPlus::distanceBetween(latitude, longitude, SAFE_LAT, SAFE_LON);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.printf("Lat: %.6f\nLon: %.6f\n", latitude, longitude);

    if (dist <= SAFE_RADIUS) {
      display.println("✅ Safe Zone");
      Firebase.RTDB.setString(&fbdo, "/wearable/alert", "✅ Within Safe Zone");
    } else {
      display.println("⚠️ OUT OF SAFE ZONE!");
      Firebase.RTDB.setString(&fbdo, "/wearable/alert", "⚠️ OUT OF SAFE ZONE!");
      Firebase.RTDB.setFloat(&fbdo, "/wearable/latitude", latitude);
      Firebase.RTDB.setFloat(&fbdo, "/wearable/longitude", longitude);

      String locMsg = "⚠️ OUT OF SAFE ZONE! Lat: " + String(latitude, 6) + ", Lon: " + String(longitude, 6);
      Firebase.RTDB.setString(&fbdo, "/wearable/locationAlert", locMsg);
    }
    display.display();

  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("✨ Connecting to GPS ✨");
    for (int i = 0; i < dotCount; i++) display.print(".");
    display.display();

    dotCount++;
    if (dotCount > 3) dotCount = 0;
  }

  // ---------------- Send Data ----------------
  if (millis() - sendDataPrevMillis > timerDelay) {
    sendDataPrevMillis = millis();
    readAndSendVitals();
  }

  // ---------------- Fall Detection ----------------
  checkFall();

  // ---------------- Medication Reminder ----------------
  medicationReminder();

  delay(500);
}

// ---------------- Functions ----------------
void readAndSendVitals() {
  float tempC = tempSensor.getTemperature();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Temp: %.1f C\nHR: %.1f bpm\nSpO2: %.1f%%", tempC, heartRate, spo2);
  display.display();

  Firebase.RTDB.setFloat(&fbdo, "/wearable/temperature", tempC);
  Firebase.RTDB.setFloat(&fbdo, "/wearable/heartRate", heartRate);
  Firebase.RTDB.setFloat(&fbdo, "/wearable/spo2", spo2);

  // Abnormal alerts
  if (tempC > 38 || tempC < 36)
    Firebase.RTDB.setString(&fbdo, "/wearable/vitalsAlert", "⚠️ Temperature abnormal!");
  if (heartRate > 100 || heartRate < 60)
    Firebase.RTDB.setString(&fbdo, "/wearable/vitalsAlert", "⚠️ Heart rate abnormal!");
  if (spo2 < 92)
    Firebase.RTDB.setString(&fbdo, "/wearable/vitalsAlert", "⚠️ SpO₂ low!");
}

void checkFall() {
  sensors_event_t a, g, tempEvt;
  mpu.getEvent(&a, &g, &tempEvt);

  float accelMag = sqrt(a.acceleration.x * a.acceleration.x +
                        a.acceleration.y * a.acceleration.y +
                        a.acceleration.z * a.acceleration.z);

  if (accelMag > 25) { 
    if (!fallDetected) {
      fallDetected = true;
      Firebase.RTDB.setBool(&fbdo, "/wearable/fallDetected", true);
      tone(BUZZER_PIN, 1500, 3000);
    }
  } else {
    fallDetected = false;
    Firebase.RTDB.setBool(&fbdo, "/wearable/fallDetected", false);
  }
}

void medicationReminder() {
  int h = hour();
  int m = minute();

  if ((h == 10 && m == 55) || (h == 13 && m == 0) || (h == 21 && m == 0)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("💊 Take Medicine!");
    display.display();
    tone(BUZZER_PIN, 1200, 4000);
    Firebase.RTDB.setString(&fbdo, "/wearable/alert", "💊 Take Medicine!");
  }
}
