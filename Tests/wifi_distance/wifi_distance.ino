/*
  Complete details at https://RandomNerdTutorials.com/esp32-useful-wi-fi-functions-arduino/
*/

#include <WiFi.h>

// Replace with your network credentials (STATION)
const char* ssid = "Hello";
const char* password = "12345600";

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
  Serial.begin(115200);
  initWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("RRSI: ");
  Serial.print(WiFi.RSSI());
  double distancem = pow(10, ((-69 - WiFi.RSSI()) / (10 * 2.4)));  // Example distance estimation formula
  double distancecm = 100*distancem;
  Serial.print(" | ");
  Serial.print(distancem);
  Serial.print("m | ");
  Serial.print(distancecm);
  Serial.println("cm");
  delay(100);
}