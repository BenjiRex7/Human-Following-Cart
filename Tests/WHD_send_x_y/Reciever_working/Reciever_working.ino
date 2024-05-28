#include <WiFi.h>
#include <WebServer.h>

// const char* WIFI_SSID = "FireNation";
// const char* WIFI_PASS = "KingZuko3";

const char* WIFI_SSID = "WaterTribe";
const char* WIFI_PASS = "Katara123";

WebServer server(80);

#define R1 18
#define R2 19

void setup() {
  Serial.begin(115200);

  // Initialize WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
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

  // Start the web server
  server.on("/object-coordinates", handleCoordinates);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleCoordinates() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("x") && server.hasArg("y") && server.hasArg("width") && server.hasArg("height")) {
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
      if (targetPosition > 75) {
        turnRight();
      } else if (targetPosition < -75) {
        turnLeft();
      } else {
        stopMotors();
      }

      server.send(200, "text/plain", "Motors moved successfully");
    } else {
      server.send(400, "text/plain", "Missing x, y, width, or height parameter");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
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
