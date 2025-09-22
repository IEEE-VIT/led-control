
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";
const char *serverBaseURL = "http://<your_ip_address>:5000/led/"; // Update <your_ip_address>

// Map each LED to a GPIO pin
const int ledPins[] = {2, 4, 5}; // Example: LED 1 on GPIO2, LED 2 on GPIO4, LED 3 on GPIO5
const int numLEDs = sizeof(ledPins) / sizeof(ledPins[0]);

void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < numLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

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
    for (int i = 0; i < numLEDs; i++) {
      HTTPClient http;
      String url = String(serverBaseURL) + String(i + 1); // LED IDs are "1", "2", "3"
      http.begin(url);

      int httpResponseCode = http.GET();
      if (httpResponseCode > 0)
      {
        String payload = http.getString();
        Serial.print("LED ");
        Serial.print(i + 1);
        Serial.print(" state: ");
        Serial.println(payload);

        if (payload == "ON")
        {
          digitalWrite(ledPins[i], HIGH);
        }
        else if (payload == "OFF")
        {
          digitalWrite(ledPins[i], LOW);
        }
      }
      else
      {
        Serial.print("Error on HTTP request for LED ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(httpResponseCode);
      }
      http.end();
      delay(200); // Small delay between requests
    }
    delay(10000); // Wait before next polling cycle
  }
}
