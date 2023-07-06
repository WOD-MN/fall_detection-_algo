#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const float AM_lower = 1.5;
const float AM_upper = 3.0;
const float orientation_range = 45.0;

unsigned long current_time;
unsigned long fall_start_time;
unsigned long orientation_change_time;
unsigned long immobilized_start_time;

bool fall_detected = false;
bool orientation_change_detected = false;
bool immobilized_detected = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float AM = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));

  current_time = millis();

  if (AM < AM_lower) {
    fall_detected = false;
    orientation_change_detected = false;
    immobilized_detected = false;
  }
  else {
    if (AM >= AM_upper && !fall_detected) {
      orientation_change_detected = true;
      orientation_change_time = current_time;
    }

    if (orientation_change_detected && (current_time - orientation_change_time) <= 500) {
      int16_t gx, gy, gz;
      mpu.getRotation(&gx, &gy, &gz);

      float orientation_change = abs(gy) * (250.0 / 32768.0);

      if (orientation_change <= orientation_range) {
        immobilized_detected = true;
        immobilized_start_time = current_time;
      }
    }

    if (immobilized_detected && (current_time - immobilized_start_time) >= 10000) {
      fall_detected = true;
      
      Serial.println("Fall detected!");
    }
  }
}
