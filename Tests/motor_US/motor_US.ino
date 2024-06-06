const int trigPin = 2; // Ultrasonic sensor trigger pin
const int echoPin = 15; // Ultrasonic sensor echo pin
const int R1 = 18; // Relay 1 control pin (connected to left motor)
const int R2 = 19; // Relay 2 control pin (connected to right motor)

long duration;
int distance;
int maxDistance = 50;  // Maximum distance threshold in cm
int minDistance = 20;  // Minimum distance threshold in cm

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

  // Print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Control relays based on distance
  if (distance < minDistance) {
    stopMotors(); // If an object is detected within the minDistance, stop the motors
  } else if (distance >= minDistance && distance < maxDistance) {
    Forward(); // If the object is within the range, move forward
  } else {
    // Optionally, you can add actions for distances greater than maxDistance
    stopMotors(); // Example: Stop the motors if the distance is greater than maxDistance
  }

  delay(100);
}

void turnRight() {
  Serial.println("Turning right");
  digitalWrite(R1, LOW);   // Turn off R1
  digitalWrite(R2, HIGH);  // Turn on R2 (assuming R1 is connected to the left motor and R2 is connected to the right motor)
}

void turnLeft() {
  Serial.println("Turning left");
  digitalWrite(R1, HIGH);  // Turn on R1
  digitalWrite(R2, LOW);   // Turn off R2
}

void stopMotors() {
  Serial.println("Stopping");
  digitalWrite(R1, HIGH);  // Turn off R1
  digitalWrite(R2, HIGH);  // Turn off R2
}

void Forward() {
  Serial.println("Forward");
  digitalWrite(R1, LOW);  // Turn off R1
  digitalWrite(R2, LOW);  // Turn off R2
}
