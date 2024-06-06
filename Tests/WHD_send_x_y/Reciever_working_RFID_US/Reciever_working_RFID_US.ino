#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <NewPing.h>

const char* WIFI_SSID = "WaterTribe";
const char* WIFI_PASS = "Katara123";

WebServer server(80);

#define R1 33
#define R2 32

#define W_LED 2
#define GREEN_LED 15  // Green LED pin
#define RED_LED 13    // Red LED pin

#define SS_PIN 21
#define RST_PIN 22

#define trigPin 27
#define echoPin 14
#define maxDistance 200  // Maximum distance in centimeters (adjust as needed)
#define minDistance 50   // Minimum distance in centimeters (adjust as needed)

NewPing sonar(trigPin, echoPin, maxDistance);  // NewPing setup of pins and maximum distance

MFRC522 rfid(SS_PIN, RST_PIN);                      // Create MFRC522 instance
bool motorsUnlocked = false;                        // Flag to control motor lock state
byte authorizedUID[] = { 0xF2, 0x8C, 0xBB, 0x33 };  // Replace with your card's UID

unsigned long lastReadTime = 0;            // Timestamp of the last RFID read
const unsigned long debounceDelay = 1000;  // 1 second debounce delay

void setup() {
  Serial.begin(115200);
  pinMode(W_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(W_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);  // Turn off green LED
  digitalWrite(RED_LED, LOW);    // Turn off red LED

  // Initialize WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(W_LED, LOW);
  }
  delay(100);
  digitalWrite(W_LED, HIGH);
  delay(100);
  digitalWrite(W_LED, LOW);
  delay(100);
  digitalWrite(W_LED, HIGH);
  delay(100);
  digitalWrite(W_LED, LOW);
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize relay pins as output
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  // Initially turn off both relays
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);

  // Initialize SPI bus and RFID reader
  SPI.begin();
  rfid.PCD_Init();

  // Start the web server
  server.on("/object-coordinates", handleCoordinates);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  readRFID();
}

void readRFID() {
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Check debounce time
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime < debounceDelay) {
    return;  // Ignore if within debounce delay
  }
  lastReadTime = currentTime;  // Update last read time

  // Show UID on serial monitor
  Serial.print("UID tag: ");
  String content = "";
  byte readUID[4];  // Array to store the UID of the read card
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
    readUID[i] = rfid.uid.uidByte[i];
  }
  Serial.println();

  // Check if the UID matches the authorized card
  bool authorized = true;
  for (byte i = 0; i < sizeof(authorizedUID); i++) {
    if (readUID[i] != authorizedUID[i]) {
      authorized = false;
      break;
    }
  }

  if (authorized) {
    motorsUnlocked = !motorsUnlocked;  // Toggle the lock state
    if (motorsUnlocked) {
      Serial.println("Motors unlocked");
      digitalWrite(GREEN_LED, HIGH);  // Turn on green LED
      digitalWrite(RED_LED, LOW);     // Turn off red LED
    } else {
      Serial.println("Motors locked");
      digitalWrite(GREEN_LED, LOW);  // Turn off green LED
      delay(250);
      digitalWrite(GREEN_LED, HIGH);  // Turn on green LED
      delay(250);
      digitalWrite(GREEN_LED, LOW);  // Turn off green LED
      delay(250);
      digitalWrite(RED_LED, HIGH);  // Turn on red LED
      delay(250);
      digitalWrite(RED_LED, LOW);  // Turn off red LED
      delay(250);
      stopMotors();
    }
  } else {
    Serial.println("Unauthorized card");
    motorsUnlocked = false;
    digitalWrite(GREEN_LED, LOW);  // Turn off green LED
    digitalWrite(RED_LED, HIGH);   // Turn on red LED
    delay(250);
    digitalWrite(RED_LED, LOW);  // Turn off red LED
    delay(250);
    digitalWrite(RED_LED, HIGH);  // Turn on red LED
    delay(250);
    digitalWrite(RED_LED, LOW);  // Turn off red LED
    delay(250);
    stopMotors();
  }
}
void handleCoordinates() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("x") && server.hasArg("y") && server.hasArg("width") && server.hasArg("height")) {
      if (!motorsUnlocked) {
        server.send(403, "text/plain", "Motors are locked");
        return;
      }

      int x = server.arg("x").toInt();
      int y = server.arg("y").toInt();
      int width = server.arg("width").toInt();
      int height = server.arg("height").toInt();

      // Print the received coordinates to serial monitor
      Serial.print("Received coordinates: ");
      Serial.print("x=");
      Serial.print(x);
      Serial.print(", y=");
      Serial.print(y);
      Serial.print(", width=");
      Serial.print(width);
      Serial.print(", height=");
      Serial.print(height);

      // Calculate the center of the detected object
      int objectCenterX = x + width / 2;

      // Calculate the target position for the motors
      int targetPosition = map(objectCenterX, 0, 800, -255, 255);  // Adjusted for 800x600 image
      Serial.print(", targetPosition=");
      Serial.println(targetPosition);

      // Move motors based on the target position
      if (targetPosition > 50) {
        turnRight();
      } else if (targetPosition < -50) {
        turnLeft();
      } else {
        handleUltrasonic();  // Check distance and move forward or stop
      }

      server.send(200, "text/plain", "Motors moved successfully");
    } else {
      stopMotors();  // Stop motors if missing x, y, width, or height parameter
      server.send(400, "text/plain", "Missing x, y, width, or height parameter");
    }
  } else {
    stopMotors();  // Stop motors if method is not POST
    server.send(405, "text/plain", "Method Not Allowed");
  }
}


void handleUltrasonic() {
  int distance = sonar.ping_cm();  // Get the distance in centimeters
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > minDistance && distance <= maxDistance) {
    forward();  // Move forward if within distance range
  } else {
    stopMotors();  // Stop if outside distance range
  }
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

void forward() {
  Serial.println("Forward");
  digitalWrite(R1, LOW);  // Turn on R1
  digitalWrite(R2, LOW);  // Turn on R2
}
