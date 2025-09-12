#include <Arduino.h>

#define TRIG_PIN 5  // GPIO18 cho Trig
#define ECHO_PIN 18 // GPIO19 cho Echo

void setup()
{
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop()
{
  // Phát xung trigger 10 microsecond
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Đo thời gian phản hồi từ Echo
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Tính khoảng cách (tốc độ âm thanh 340 m/s)
  float distance = duration * 0.0343 / 2; // cm

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
