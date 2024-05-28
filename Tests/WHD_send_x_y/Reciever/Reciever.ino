#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "FireNation";
const char* WIFI_PASS = "KingZuko3";

WebServer server(80);

int centerX = 160; // Center X coordinate of the screen (assuming 320x240 resolution)
int centerY = 120; // Center Y coordinate of the screen (assuming 320x240 resolution)

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
      Serial.println(height);
      
      // Calculate the target X and Y servo positions to keep the person in the center
      int targetX = map(x + width / 2, 0, 320, 0, 180); // Assuming x and width are in the range of 0-320 pixels
      int targetY = map(y + height / 2, 0, 240, 0, 180); // Assuming y and height are in the range of 0-240 pixels
      
      // Calculate the difference between current and target servo positions
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

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Set static IP address
  IPAddress staticIP(192, 168, 100, 20);
  IPAddress gateway(192, 168, 100, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(staticIP, gateway, subnet);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/object-coordinates", handleCoordinates);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
