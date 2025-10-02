#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHT_PIN 15

// uint8_t peerMac[] = {0x6C, 0xC8, 0x40, 0x8C, 0x77, 0x08};
uint8_t peerMac[] = {0x48, 0xE7, 0x29, 0xB4, 0x80, 0xCC};
DHT dht(DHT_PIN, DHT11);

typedef struct struct_message
{
  float temperature;
  float humidity;
} struct_message;

struct_message incomingMessage;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  Serial.printf("Received from %02X:%02X:%02X:%02X:%02X:%02X → T=%.2f °C, H=%.2f %%\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                incomingMessage.temperature, incomingMessage.humidity);
}

void addPeerUntilSuccess()
{
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  while (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer, retrying...");
    delay(1000);
  }

  Serial.println("Peer added successfully");
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  dht.begin();

  Serial.println("ESP-NOW Communication Example");
  Serial.print("This ESP32 STA MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  addPeerUntilSuccess();
}

void loop()
{
  struct_message myData;
  myData.temperature = dht.readTemperature();
  myData.humidity = dht.readHumidity();

  Serial.printf("Temp: %.2f °C | Humi: %.2f %%\n",
                myData.temperature, myData.humidity);

  esp_err_t result = esp_now_send(peerMac, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Message sent successfully");
  }
  else
  {
    Serial.println("Error sending the data");
  }

  delay(1000);
}