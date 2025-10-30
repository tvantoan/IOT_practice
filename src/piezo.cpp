#include <Arduino.h>
#define PIEZO_PIN 34 // A0 trên ESP32
#define LED_PIN 2

float lastPiezo = 0;
float filteredPiezo = 0;
const float HPF_ALPHA = 0.05; // high-pass filter

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  int rawPiezo = analogRead(PIEZO_PIN);
  filteredPiezo = HPF_ALPHA * (filteredPiezo + rawPiezo - lastPiezo);
  lastPiezo = rawPiezo;

  if (abs(filteredPiezo) > 50)
  { // threshold thử nghiệm
    digitalWrite(LED_PIN, HIGH);
    Serial.print("RUNG Piezo! Value: ");
    Serial.println(filteredPiezo);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }

  delay(5); // ~200Hz
}
