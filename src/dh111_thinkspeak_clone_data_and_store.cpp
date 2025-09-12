#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

Preferences prefs;

const char *ssid = "wifi-name";
const char *password = "wifi-password";

const char *server = "https://api.thingspeak.com/channels/3071884/feeds.json?api_key=CTM1JI0M3H9PWML5&results=2";

void setup()
{
  Serial.begin(115200);

  prefs.begin("my-app", false);

  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi da ket noi!");

  String lastTemp = prefs.getString("lastTemp", "0");
  String lastHum = prefs.getString("lastHum", "0");
  Serial.println("Gia tri cu da luu: Temp=" + lastTemp + " | Hum=" + lastHum);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(server);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("Du lieu nhan ve: " + payload);

      DynamicJsonDocument doc(4096);
      DeserializationError err = deserializeJson(doc, payload);

      if (err)
      {
        Serial.print("Loi JSON: ");
        Serial.println(err.c_str());
        return;
      }

      JsonArray feeds = doc["feeds"].as<JsonArray>();

      for (JsonObject feed : feeds)
      {
        const char *time = feed["created_at"];
        const char *field1 = feed["field1"];
        const char *field2 = feed["field2"];

        Serial.print("Thoi gian: ");
        Serial.print(time);
        Serial.print(" | Nhiet do: ");
        Serial.print(field1);
        Serial.print(" | Do am: ");
        Serial.println(field2);

        prefs.putString("lastTemp", String(field1));
        prefs.putString("lastHum", String(field2));
      }

      String savedTemp = prefs.getString("lastTemp", "0");
      String savedHum = prefs.getString("lastHum", "0");
      Serial.println("Da luu xuong flash: Temp=" + savedTemp + " | Hum=" + savedHum);
    }
    else
    {
      Serial.println("Loi GET: " + String(httpCode));
    }
    http.end();
  }
  delay(10000);
}
