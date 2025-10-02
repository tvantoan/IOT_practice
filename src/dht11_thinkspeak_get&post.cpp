#include "DHT.h"

#define DHTPIN 15

#define DHTTYPE DHT11

#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "LG";
const char *password = "11111112";

String apiKey = "REG9UT9GJU5KI24J";
const char *server = "http://api.thingspeak.com/update";

#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop()
{
  delay(5000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Read DHT data failed!");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" °C | Humi: ");
  Serial.print(h);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(t) + "&field2=" + String(h);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("Server response: " + payload);
    }
    else
    {
      Serial.println("GET failed: " + String(httpCode));
    }
    http.end();

    http.begin(server);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "api_key=" + apiKey + "&field1=" + String(t) + "&field2=" + String(h);
    int httpCode = http.POST(postData);
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("Server response: " + payload);
    }
    else
    {
      Serial.println("POST failed: " + String(httpCode));
    }
    http.end();
  }
}
