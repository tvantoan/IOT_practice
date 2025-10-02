

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const float ACCEL_SCALE = 16384.0; // ±2g
const float GYRO_SCALE = 131.0;    // ±250°/s

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA = 21, SCL = 22 trên ESP32

  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (mpu.testConnection())
  {
    Serial.println("MPU6050 connected successfully!");
  }
  else
  {
    Serial.println("MPU6050 connection failed!");
    while (1)
      ;
  }
}

void loop()
{
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Chuyển raw sang g và °/s
  float Ax = ax / ACCEL_SCALE;
  float Ay = ay / ACCEL_SCALE;
  float Az = az / ACCEL_SCALE;

  float Gx = gx / GYRO_SCALE;
  float Gy = gy / GYRO_SCALE;
  float Gz = gz / GYRO_SCALE;

  Serial.print("Accel (g) -> X: ");
  Serial.print(Ax, 2);
  Serial.print(" | Y: ");
  Serial.print(Ay, 2);
  Serial.print(" | Z: ");
  Serial.println(Az, 2);

  Serial.print("Gyro (°/s) -> X: ");
  Serial.print(Gx, 2);
  Serial.print(" | Y: ");
  Serial.print(Gy, 2);
  Serial.print(" | Z: ");
  Serial.println(Gz, 2);

  Serial.println("----------------------------");
  delay(500);
}
