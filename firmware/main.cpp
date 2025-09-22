#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";
const char *serverURL = "http://<your_ip_address>/led";

#define LED_PIN 2

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

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

      if (payload == "ON")
      {
        digitalWrite(LED_PIN, HIGH);
      }
      else if (payload == "OFF")
      {
        digitalWrite(LED_PIN, LOW);
      }
    }
    else
    {
      Serial.println("Error on HTTP request: "); // add http error code
    }
    delay(10000);
  }
};
