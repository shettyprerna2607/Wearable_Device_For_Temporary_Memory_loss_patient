/***********************************************************************************************
   ESP8266 Pillbox
************************************************************************************************/

#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <TimeLib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ---------------- Firebase Config ----------------
#define WIFI_SSID " "
#define WIFI_PASSWORD " "
#define API_KEY " "
#define DATABASE_URL " "

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- Pins ----------------
#define BTN_MORNING D1
#define BTN_AFTERNOON D2
#define BTN_EVENING D3
#define BUZZER D6

bool morningTaken = false;
bool afternoonTaken = false;
bool eveningTaken = false;

// ---------------- NTP Time Setup ----------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  
void setup() {
  Serial.begin(115200);

  pinMode(BTN_MORNING, INPUT_PULLUP);
  pinMode(BTN_AFTERNOON, INPUT_PULLUP);
  pinMode(BTN_EVENING, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // ---------------- Connect to WiFi ----------------
  Serial.println("🌐 Connecting to WiFi...");
  int dotCount = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    dotCount++;
    if (dotCount > 3) {
      Serial.print("\b\b\b   \b\b\b");
      dotCount = 0;
    } 
  }
  Serial.println("\n✅ WiFi Connected");

  // ---------------- Connect to Firebase ----------------
  Serial.println("🔗 Connecting to Firebase...");
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.signUp(&config, &auth, "", "");  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("✅ Firebase Connected");

  // ---------------- NTP ----------------
  timeClient.begin();
  timeClient.update();
  Serial.println("⏱ NTP Time Client Initialized");
}

void loop() {
  // ---------------- Update Time ----------------
  timeClient.update();
  int h = timeClient.getHours();
  int m = timeClient.getMinutes();

  // ---------------- Check Buttons ----------------
  if (digitalRead(BTN_MORNING) == LOW && !morningTaken) {
    pillTaken("morning");
    morningTaken = true;
  }
  if (digitalRead(BTN_AFTERNOON) == LOW && !afternoonTaken) {
    pillTaken("afternoon");
    afternoonTaken = true;
  }
  if (digitalRead(BTN_EVENING) == LOW && !eveningTaken) {
    pillTaken("evening");
    eveningTaken = true;
  }

  if ((h == 10 && m == 55 && !morningTaken) || (h == 13 && m == 0 && !afternoonTaken) || (h == 21 && m == 0 && !eveningTaken)) {
    digitalWrite(BUZZER, HIGH); 
    Serial.println("💊 Medication time! Press button to stop buzzer.");
  } else {
    digitalWrite(BUZZER, LOW); 
  }

  delay(200);  
}

void pillTaken(const char* pillName) {
  Firebase.RTDB.setBool(&fbdo, String("/pillbox/") + pillName, true);
  Firebase.RTDB.setString(&fbdo, "/wearable/alert", String("💊 ") + pillName + " pill taken");
  digitalWrite(BUZZER, LOW); 
  Serial.printf("✅ %s pill marked as taken.\n", pillName);
}
