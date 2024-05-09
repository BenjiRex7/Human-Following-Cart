#include <WiFi.h>

// Replace with your network credentials (STATION)
const char* ssid = "Hello";
const char* password = "12345600";
// Motor control pins
#define EnA 19  //Right B
#define IN1 18  //Left B
#define IN2 5   //Left F
#define IN3 4   //Right B
#define IN4 2   //Right F
#define EnB 15  //Right F

// Distance thresholds for obstacle detection

#define MAX_DISTANCE 6
#define MIN_DISTANCE 3

// Maximum and minimum motor speeds

#define MAX_SPEED 150
#define MIN_SPEED 75

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  // Set motor control pins as outputs
  Serial.begin(115200);
  pinMode(IN3, OUTPUT);  //left motors forward
  pinMode(IN4, OUTPUT);  //left motors reverse
  pinMode(IN1, OUTPUT);  //right motors forward
  pinMode(IN2, OUTPUT);  //right motors reverse
  pinMode(EnA, OUTPUT);
  pinMode(EnB, OUTPUT);
  initWiFi();
}

void loop() {
  Serial.print("RRSI: ");
  Serial.print(WiFi.RSSI());
  float distance = 100 * (pow(10, ((-69 - WiFi.RSSI()) / (10 * 2.4))));  // Example distance estimation formula
  Serial.print(" | distance: ");
  Serial.println(distance);
  // Find the sensor with the smallest distance
  if (distance < MIN_DISTANCE) {
    moveBackward();
    Serial.println("backward");
  } else if (distance > MAX_DISTANCE) {
    moveForward();
    Serial.println("forward");
  } else {
    stop();
    Serial.println("stop");
  }

  delay(100);  // Delay for stability and to avoid excessive readings
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
