#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Library for parsing JSON (install ArduinoJson library)

const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";
const char *serverURL = "http://<your_ip_address>/leds";  // Updated endpoint

// Define multiple LEDs
#define LED1_PIN 2
#define LED2_PIN 4
#define LED3_PIN 5

void setup()
{
  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverURL);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0)
    {
      String payload = http.getString();
      Serial.println("Server response: " + payload);

      // Parse JSON response
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error)
      {
        // Control LED1
        if (doc.containsKey("LED1"))
        {
          digitalWrite(LED1_PIN, (doc["LED1"] == "ON") ? HIGH : LOW);
        }

        // Control LED2
        if (doc.containsKey("LED2"))
        {
          digitalWrite(LED2_PIN, (doc["LED2"] == "ON") ? HIGH : LOW);
        }

        // Control LED3
        if (doc.containsKey("LED3"))
        {
          digitalWrite(LED3_PIN, (doc["LED3"] == "ON") ? HIGH : LOW);
        }
      }
      else
      {
        Serial.println("Failed to parse JSON");
      }
    }
    else
    {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    delay(5000); // Poll every 5 seconds
  }
}
