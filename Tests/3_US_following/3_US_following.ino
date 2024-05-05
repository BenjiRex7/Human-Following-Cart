// Ultrasonic sensor pins

#define S1Trig 13  //front
#define S1Echo 12

#define S2Echo 14  //left
#define S2Trig 27

#define S3Trig 26  //right
#define S3Echo 25

// Motor control pins
#define EnA 19  //Right B
#define IN1 18  //Left B
#define IN2 5   //Left F
#define IN3 4   //Right B
#define IN4 2   //Right F
#define EnB 15  //Right F

// Distance thresholds for obstacle detection

#define MAX_DISTANCE 20
#define MIN_DISTANCE 7

// Maximum and minimum motor speeds

#define MAX_SPEED 150
#define MIN_SPEED 75

void setup() {
  // Set motor control pins as outputs
  pinMode(IN3, OUTPUT);  //left motors forward
  pinMode(IN4, OUTPUT);  //left motors reverse
  pinMode(IN1, OUTPUT);  //right motors forward
  pinMode(IN2, OUTPUT);  //right motors reverse
  pinMode(EnA, OUTPUT);
  pinMode(EnB, OUTPUT);
  // Set the Trig pins as output pins
  pinMode(S1Trig, OUTPUT);
  pinMode(S2Trig, OUTPUT);
  pinMode(S3Trig, OUTPUT);

  // Set the Echo pins as input pins
  pinMode(S1Echo, INPUT);
  pinMode(S2Echo, INPUT);
  pinMode(S3Echo, INPUT);

  // Initialize the serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  int frontDistance = sensorOne();
  int leftDistance = sensorTwo();
  int rightDistance = sensorThree();

  Serial.print("Front: ");
  Serial.print(frontDistance);
  Serial.print(" cm, Left: ");
  Serial.print(leftDistance);
  Serial.print(" cm, Right: ");
  Serial.print(rightDistance);
  Serial.println(" cm");

  // Find the sensor with the smallest distance
  if (frontDistance < MIN_DISTANCE) {
    moveBackward();
    Serial.println("backward");
  } else if (frontDistance < leftDistance && frontDistance < rightDistance && frontDistance < MAX_DISTANCE) {
    moveForward();
    Serial.println("forward");
  } else if (leftDistance < rightDistance && leftDistance < MAX_DISTANCE) {
    turnLeft();
    Serial.println("left");
  } else if (rightDistance < MAX_DISTANCE) {
    turnRight();
    Serial.println("right");
  } else {
    stop();
    Serial.println("stop");
  }

  delay(100);  // Delay for stability and to avoid excessive readings
}

// Function to measure the distance using an ultrasonic sensor
int sensorOne() {
  // Pulse output
  digitalWrite(S1Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(S1Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(S1Trig, LOW);
  long t = pulseIn(S1Echo, HIGH);  // Get the pulse
  int cm = t / 29 / 2;             // Convert time to distance
  return cm;                       // Return the values from the sensor
}

// Get the sensor values
int sensorTwo() {
  // Pulse output
  digitalWrite(S2Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(S2Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(S2Trig, LOW);
  long t = pulseIn(S2Echo, HIGH);  // Get the pulse
  int cm = t / 29 / 2;             // Convert time to distance
  return cm;                       // Return the values from the sensor
}

// Get the sensor values
int sensorThree() {
  // Pulse output
  digitalWrite(S3Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(S3Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(S3Trig, LOW);
  long t = pulseIn(S3Echo, HIGH);  // Get the pulse
  int cm = t / 29 / 2;             // Convert time to distance
  return cm;                       // Return the values from the sensor
}

// Motor control functions
void moveForward() {
  analogWrite(EnA, 120);
  analogWrite(EnB, 120);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Forward");
}

void moveBackward() {
  analogWrite(EnA, 107);
  analogWrite(EnB, 107);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Reverse");
}

void turnRight() {
  analogWrite(EnA, 107);
  analogWrite(EnB, 107);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Right");
}

void turnLeft() {
  analogWrite(EnA, 107);
  analogWrite(EnB, 107);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Left");
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
