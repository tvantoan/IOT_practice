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
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi WiFi!");
}

void loop()
{
  delay(5000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Doc DHT that bai!");
    return;
  }

  Serial.print("Nhiet do: ");
  Serial.print(t);
  Serial.print(" °C | Do am: ");
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
      Serial.println("Server tra ve: " + payload);
    }
    else
    {
      Serial.println("Loi GET: " + String(httpCode));
    }
    http.end();

    http.begin(server);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "api_key=" + apiKey + "&field1=" + String(t) + "&field2=" + String(h);
    int httpCode = http.POST(postData);
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("Server tra ve: " + payload);
    }
    else
    {
      Serial.println("Loi POST: " + String(httpCode));
    }
    http.end();
  }
}
