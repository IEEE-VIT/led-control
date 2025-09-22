// main.cpp - ESP32 firmware to support multiple LEDs via GET /leds (JSON)
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- CONFIG: update these ---
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASS";
// Point to your flask server's /leds endpoint, e.g. http://192.168.1.100:5000/leds
const char* serverURL = "http://<FLASK_IP>:5000/leds";
// -----

// Map LED indexes to ESP32 GPIO pins
const int ledPins[] = {2, 4, 13}; // change to your pins
const int LED_COUNT = sizeof(ledPins) / sizeof(ledPins[0]);

// How often to poll the server (ms)
const unsigned long POLL_INTERVAL = 2000UL;

unsigned long lastPoll = 0;

void setupWifi() {
  Serial.print("Connecting to WiFi ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 60) { // ~30s timeout
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect failed");
  }
}

void setupPins() {
  for (int i = 0; i < LED_COUNT; ++i) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void applyStateForIndex(int idx, const String &stateStr) {
  if (idx < 0 || idx >= LED_COUNT) return;
  String s = stateStr;
  s.trim();
  s.toUpperCase();
  if (s == "ON") digitalWrite(ledPins[idx], HIGH);
  else if (s == "OFF") digitalWrite(ledPins[idx], LOW);
  else {
    // try to interpret numeric brightness (0-255)
    bool isNum = true;
    for (char c : s) if (!isDigit(c)) { isNum = false; break; }
    if (isNum) {
      int val = s.toInt();
      // if you want PWM, attach led to PWM channel and analogWrite-like function.
      // For now, treat nonzero as ON:
      digitalWrite(ledPins[idx], val > 0 ? HIGH : LOW);
    }
  }
}

void parseAndApplyJson(const String &payload) {
  // Try two formats:
  // 1) {"0":"ON","1":"OFF"} (simple)
  // 2) {"0":{"state":"ON","brightness":200}, "1":{"state":"OFF"}}
  // We'll attempt to deserialize into JsonObject and handle keys.

  StaticJsonDocument<1024> doc; // adjust size if you add many LEDs
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    // Not JSON - maybe plain "ON"/"OFF" for single LED; fallback
    String p = payload;
    p.trim();
    p.toUpperCase();
    Serial.println("Fallback payload (not JSON): " + p);
    // apply to LED 0 by default
    applyStateForIndex(0, p);
    return;
  }

  JsonObject obj = doc.as<JsonObject>();
  for (JsonPair kv : obj) {
    String key = kv.key().c_str(); // e.g. "0"
    // ensure key is numeric
    bool keyIsNum = true;
    for (char c : key) if (!isDigit(c)) { keyIsNum = false; break; }
    if (!keyIsNum) continue;
    int idx = key.toInt();
    // Value might be a string or an object
    if (kv.value().is<const char*>()) {
      String val = kv.value().as<const char*>();
      applyStateForIndex(idx, val);
      Serial.printf("Set LED %d => %s\n", idx, val.c_str());
    } else if (kv.value().is<JsonObject>()) {
      JsonObject cfg = kv.value().as<JsonObject>();
      if (cfg.containsKey("state")) {
        String s = cfg["state"].as<const char*>();
        applyStateForIndex(idx, s);
        Serial.printf("Set LED %d => %s\n", idx, s.c_str());
      } else if (cfg.containsKey("brightness")) {
        // if you later enable PWM, apply brightness here
        int b = cfg["brightness"].as<int>();
        // For now, treat brightness>0 as ON
        applyStateForIndex(idx, b > 0 ? "ON" : "OFF");
        Serial.printf("Set LED %d => brightness %d\n", idx, b);
      }
    } else {
      // other types
    }
  }
}

void loopPoll() {
  if ((millis() - lastPoll) < POLL_INTERVAL) return;
  lastPoll = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting reconnect...");
    setupWifi();
    return;
  }

  HTTPClient http;
  http.begin(serverURL);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    payload.trim();
    Serial.printf("HTTP %d, payload: %s\n", httpCode, payload.c_str());
    if (httpCode == 200) {
      parseAndApplyJson(payload);
    } else {
      Serial.printf("Server returned code %d\n", httpCode);
    }
  } else {
    Serial.printf("HTTP GET failed, err: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  setupPins();
  setupWifi();
  lastPoll = 0;
}

void loop() {
  loopPoll();
}