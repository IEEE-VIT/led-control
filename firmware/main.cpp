#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* serverIP = "http://<your_ip_address>:5000";

// LED pins and IDs
const int LED_PINS[] = {2, 4, 5};
const char* LED_IDS[] = {"led1", "led2", "led3"};
const int NUM_LEDS = sizeof(LED_PINS)/sizeof(LED_PINS[0]);

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    for (int i = 0; i < NUM_LEDS; i++) {
      String url = String(serverIP) + "/led/" + LED_IDS[i];
      http.begin(url);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        String payload = http.getString();
        Serial.println("Response for " + String(LED_IDS[i]) + ": " + payload);

        DynamicJsonDocument doc(256);
        if (!deserializeJson(doc, payload)) continue;

        String state = doc["state"];
        if (state == "ON") digitalWrite(LED_PINS[i], HIGH);
        else digitalWrite(LED_PINS[i], LOW);

      } else {
        Serial.println("Error on HTTP request for " + String(LED_IDS[i]) + ": " + httpResponseCode);
      }
      http.end();
    }

    delay(5000); // poll every 5 seconds
  }
}
