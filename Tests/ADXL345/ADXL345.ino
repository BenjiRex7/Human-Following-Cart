#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define SDA_PIN 3
#define SCL_PIN 18

Adafruit_ADXL345_Unified accel;

void setup(void) {
  Serial.begin(9600);
  
  // Set SDA and SCL pins
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);

  if(!accel.begin()) {
    Serial.println("No valid sensor found");
    while(1);
  }
}

void loop(void) {
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");
  Serial.println("m/s^2 ");
  delay(500);
}
