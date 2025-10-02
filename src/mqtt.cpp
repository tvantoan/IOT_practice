#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT11

// Thông tin WiFi
const char *ssid = "Tầng 5";
const char *password = "ANH12345678";

// Thông tin MQTT Broker (máy anh chạy Mosquitto)
const char *mqtt_server = "192.168.0.103"; // IP máy tính của anh trong LAN
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Lặp đến khi kết nối lại được broker
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
      // Có thể subscribe topic tại đây nếu cần
      client.subscribe("test/topic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  char payload[50];
  snprintf(payload, sizeof(payload), "Temp: %.2f °C, Hum: %.2f %%", t, h);

  Serial.print("Publishing message: ");
  Serial.println(payload);

  client.publish("home/room1/dht", payload); // publish lên topic này

  delay(5000);
}
