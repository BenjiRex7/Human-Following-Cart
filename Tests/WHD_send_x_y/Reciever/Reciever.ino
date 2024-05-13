#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "HUAWEI-8NBn";
const char* WIFI_PASS = "PakistaN123";

WebServer server(80);

int x_min = 0;
int y_min = 0;

void handleCoordinates() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("x_min") && server.hasArg("y_min")) {
      x_min = server.arg("x_min").toInt();
      y_min = server.arg("y_min").toInt();
      Serial.print("Received x_min: ");
      Serial.print(x_min);
      Serial.print(" | Received y_min: ");
      Serial.println(y_min);
      server.send(200, "text/plain", "Coordinates received successfully");
    } else {
      server.send(400, "text/plain", "Missing x_min or y_min parameter");
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/object-coordinates", handleCoordinates);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
