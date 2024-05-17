#include <WiFi.h>
#include <WebServer.h>
#include <Servo.h>

const char* WIFI_SSID = "HUAWEI-8NBn";
const char* WIFI_PASS = "PakistaN123";

WebServer server(80);
Servo servoX;
Servo servoY;

int centerX = 160; // Center X coordinate of the screen (assuming 320x240 resolution)
int centerY = 120; // Center Y coordinate of the screen (assuming 320x240 resolution)

void handleCoordinates() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("x") && server.hasArg("y") && server.hasArg("width") && server.hasArg("height")) {
      int x = server.arg("x").toInt();
      int y = server.arg("y").toInt();
      int width = server.arg("width").toInt();
      int height = server.arg("height").toInt();
      
      // Calculate the target X and Y servo positions to keep the person in the center
      int targetX = map(x + width / 2, 0, 320, 0, 180); // Assuming x and width are in the range of 0-320 pixels
      int targetY = map(y + height / 2, 0, 240, 0, 180); // Assuming y and height are in the range of 0-240 pixels
      
      // Calculate the difference between current and target servo positions
      int deltaX = targetX - servoX.read();
      int deltaY = targetY - servoY.read();
      
      // Adjust servo positions based on the calculated difference
      servoX.write(servoX.read() + deltaX);
      servoY.write(servoY.read() + deltaY);
      
      Serial.print("Servo X Position: ");
      Serial.print(servoX.read());
      Serial.print(" | Servo Y Position: ");
      Serial.println(servoY.read());
      
      server.send(200, "text/plain", "Servos moved successfully");
    } else {
      server.send(400, "text/plain", "Missing x, y, width, or height parameter");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(115200);
  servoX.attach(26); // Attach servo to pin 26
  servoY.attach(27); // Attach servo to pin 27
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/object-coordinates", handleCoordinates);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
