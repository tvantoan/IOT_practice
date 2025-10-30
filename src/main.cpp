#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Arduino.h>

#define LED_PIN 2

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Cấu hình
const int SAMPLE_HZ = 10;
const int WINDOW_SIZE = 100;
const int BATCH_PRINT = 10;
const float ALPHA_GRAV = 0.98;
const float ON_THRESH = 0.08;
const float OFF_THRESH = 0.06;

float gx = 0, gy = 0, gz = 0;
bool gravityInit = false;

// Buffer trượt
float buf[WINDOW_SIZE];
int bufIndex = 0;
int bufCount = 0;
double sumSq = 0.0; // double cho an toàn số học

int batchCount = 0;
bool ledState = false;

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin();

  if (!accel.begin())
  {
    Serial.println("ADXL345 not found!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_2_G);
  Serial.println("Running RMS measurement...");
}

void loop()
{
  unsigned long start = millis();

  sensors_event_t e;
  accel.getEvent(&e);

  float ax = e.acceleration.x;
  float ay = e.acceleration.y;
  float az = e.acceleration.z;

  // Khởi tạo gravity
  if (!gravityInit)
  {
    gx = ax;
    gy = ay;
    gz = az;
    gravityInit = true;
  }

  // Low-pass gravity
  gx = ALPHA_GRAV * gx + (1 - ALPHA_GRAV) * ax;
  gy = ALPHA_GRAV * gy + (1 - ALPHA_GRAV) * ay;
  gz = ALPHA_GRAV * gz + (1 - ALPHA_GRAV) * az;

  // Dynamic part
  float dx = ax - gx;
  float dy = ay - gy;
  float dz = az - gz;
  float dynSq = dx * dx + dy * dy + dz * dz;

  // Debug chi tiết
  // Serial.print("ax: ");
  // Serial.print(ax, 6);
  // Serial.print(",");
  // Serial.print(ay, 6);
  // Serial.print(",");
  // Serial.print(az, 6);
  // Serial.print(",");
  // Serial.println(dynSq, 6);
  if (dynSq < 0.011477)
  {
    dynSq = 0.0; // lọc nhiễu nhỏ
  }
  // Cập nhật running sum
  if (bufCount < WINDOW_SIZE)
  {
    buf[bufIndex] = dynSq;
    sumSq += dynSq;
    bufCount++;
  }
  else
  {
    sumSq -= buf[bufIndex]; // trừ mẫu cũ
    buf[bufIndex] = dynSq;
    sumSq += dynSq; // cộng mẫu mới
  }

  bufIndex = (bufIndex + 1) % WINDOW_SIZE;

  // In RMS định kỳ
  batchCount++;
  if (batchCount >= BATCH_PRINT && bufCount > 0)
  {
    float rms = sqrt(sumSq / bufCount);

    // Serial.print("RMS(g): ");
    Serial.println(rms, 6);

    // Logic bật tắt LED với hysteresis
    if (!ledState && rms > ON_THRESH)
    {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
    }
    else if (ledState && rms < OFF_THRESH)
    {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
    }

    batchCount = 0;
  }

  long wait = (1000 / SAMPLE_HZ) - (millis() - start);
  if (wait > 0)
    delay(wait);
  // delay(2000);
}
