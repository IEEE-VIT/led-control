#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* serverURL = "http://<your_ip_address>/led";

#define LED_PIN 2

String mode = "OFF";
int interval = 500; // default for blink/fade
unsigned long lastUpdate = 0;
int brightness = 0; // for fade
bool ledOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void fetchLEDState() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    int code = http.GET();
    if (code > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(256);
      if (!deserializeJson(doc, payload)) return;

      mode = doc["mode"] | "OFF";
      interval = doc["interval"] | 500;
    }
    http.end();
  }
}

void loop() {
  static unsigned long lastFetch = 0;
  unsigned long now = millis();

  if (now - lastFetch > 5000) { // fetch every 5 sec
    fetchLEDState();
    lastFetch = now;
  }

  if (mode == "ON") {
    digitalWrite(LED_PIN, HIGH);
  } else if (mode == "OFF") {
    digitalWrite(LED_PIN, LOW);
  } else if (mode == "BLINK") {
    if (now - lastUpdate >= interval) {
      ledOn = !ledOn;
      digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
      lastUpdate = now;
    }
  } else if (mode == "FADE") {
    if (now - lastUpdate >= interval) {
      analogWrite(LED_PIN, brightness);
      brightness += 5;
      if (brightness > 255) brightness = 0;
      lastUpdate = now;
    }
  }
}
