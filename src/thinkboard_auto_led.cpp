#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT11
#define LEDPIN 5

const char *ssid = "Tầng5";
const char *password = "ANH12345678";
const char *thingsboardServer = "demo.thingsboard.io";
const int tbPort = 1883;
const char *accessToken = "ql5cfm5x95y9poq87eyu";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

bool autoMode = true;
bool manualLEDState = false;

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String msg;
  for (int i = 0; i < length; i++)
    msg += (char)payload[i];
  msg.trim();
  Serial.print("RPC received: ");
  Serial.println(msg);

  if (msg.indexOf("setValue") >= 0)
  {
    if (msg.indexOf("true") >= 0)
    {
      manualLEDState = true;
      digitalWrite(LEDPIN, HIGH);
      autoMode = false;
      Serial.println("Switch ON -> LED ON, AutoMode OFF");
    }
    else
    {
      manualLEDState = false;
      digitalWrite(LEDPIN, LOW);
      autoMode = false;
      Serial.println("Switch OFF -> LED OFF, AutoMode OFF");
    }
  }

  if (msg == "AUTO_ON")
  {
    autoMode = true;
    Serial.println("AUTO mode enabled");
  }
  if (msg == "AUTO_OFF")
  {
    autoMode = false;
    Serial.println("AUTO mode disabled");
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32", accessToken, NULL))
    //    if (client.connect(accessToken)) -> ko đúng, sẽ chạy vào lỗi vì truyền k đủ tham số, broker nhận nhầm accessToken là clientID
    {
      Serial.println("Connected");
      client.subscribe("v1/devices/me/rpc/request/+");
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
  dht.begin();
  setup_wifi();
  client.setServer(thingsboardServer, tbPort);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum))
    return;

  if (autoMode)
  {
    if (temp > 30 || hum > 70)
      digitalWrite(LEDPIN, HIGH);
    else
      digitalWrite(LEDPIN, LOW);
  }

  String payload = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}";
  client.publish("v1/devices/me/telemetry", payload.c_str());

  delay(2000);
}
