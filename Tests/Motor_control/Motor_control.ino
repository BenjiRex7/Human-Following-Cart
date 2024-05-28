
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
char value;
#define R1 18
#define R2 19

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  // Initialize Bluetooth serial communication
  SerialBT.begin("Sparky");  // Bluetooth device name
  Serial.println("The device started, now you can pair it with Bluetooth!");

  // Initialize the relay pins as output
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  // Initially turn on both relays
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);
}

void loop() {
  // Check for incoming Bluetooth data
  if (SerialBT.available()) {
    value = SerialBT.read();
    Serial.println(value);
  }

  // Control the motors based on received Bluetooth commands
  if (value == 'U') {  // Forward
    forward();
  } else if (value == 'D') {  // Backward
    stop();
  } else if (value == 'L') {  // Left
    left();
  } else if (value == 'R') {  // Right
    right();
  } else {  // Stop
    stop();
  }
}

void forward() {
  Serial.println("Moving forward");
  digitalWrite(R1, LOW);  // Turn off R1
  digitalWrite(R2, LOW);  // Turn off R2
}

void right() {
  Serial.println("Turning right");
  digitalWrite(R1, LOW);   // Turn off R1
  digitalWrite(R2, HIGH);  // Turn on R2
}

void left() {
  Serial.println("Turning left");
  digitalWrite(R1, HIGH);  // Turn on R1
  digitalWrite(R2, LOW);   // Turn off R2
}

void stop() {
  Serial.println("Stopping");
  digitalWrite(R1, HIGH);  // Turn on R1
  digitalWrite(R2, HIGH);  // Turn on R2
}
