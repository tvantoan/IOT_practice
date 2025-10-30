#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Arduino.h>

#define LED_PIN 2

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
const int WINDOW_SIZE = 100;
float magBuffer[WINDOW_SIZE];
int indexBuffer = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Wire.begin(); // dùng SDA=21, SCL=22 mặc định ESP32
  if (!accel.begin())
  {
    Serial.println("ADXL345 not found!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_2_G);
}

void loop()
{
  sensors_event_t event;
  accel.getEvent(&event);

  // hiển thị từng trục với 6 chữ số thập phân
  Serial.print("X: ");
  Serial.print(event.acceleration.x, 6);
  Serial.print(" | Y: ");
  Serial.print(event.acceleration.y, 6);
  Serial.print(" | Z: ");
  Serial.println(event.acceleration.z, 6);

  float mag = sqrt(event.acceleration.x * event.acceleration.x +
                   event.acceleration.y * event.acceleration.y +
                   event.acceleration.z * event.acceleration.z);

  // lưu buffer để tính RMS
  magBuffer[indexBuffer] = mag;
  indexBuffer = (indexBuffer + 1) % WINDOW_SIZE;

  float sum = 0;
  for (int i = 0; i < WINDOW_SIZE; i++)
    sum += sq(magBuffer[i] - 1.0); // trừ 1g
  float rms = sqrt(sum / WINDOW_SIZE);

  // bật LED nếu rung vượt threshold
  if (rms > 0.05)
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("RMS: ");
    Serial.println(rms, 6);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }

  delay(5); // ~200Hz
}
