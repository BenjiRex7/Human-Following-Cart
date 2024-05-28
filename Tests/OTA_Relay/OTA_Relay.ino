#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* SSID = "FireNation";
const char* PASS = "KingZuko3";
#define WIFI_LED 2

// YOUR CODE
#define R1 26
#define R2 25

//BETWEEN HERE

void setupAuth() {
  ArduinoOTA.setHostname("Mira");
  ArduinoOTA.setPassword("123");
}

void setup() {
  setupWiFi();
  setupAuth();
  setupOTA();

  // YOUR CODE
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  //BETWEEN HERE
}

void loop() {
  ArduinoOTA.handle();

  digitalWrite(R1, LOW);
  Serial.println("R1 is not working");
  delay(500);
  digitalWrite(R1, HIGH);
  Serial.println("R1 is working");
  delay(500);
  digitalWrite(R2, LOW);
  Serial.println("R2 is not working");
  delay(500);
  digitalWrite(R2, HIGH);
  Serial.println("R2 is  working");
  delay(500);
}


void setupWiFi() {
  Serial.begin(115200);
  pinMode(WIFI_LED, OUTPUT);
  Serial.println("Booting");

  digitalWrite(WIFI_LED, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(WIFI_LED, LOW);
  delay(100);
  digitalWrite(WIFI_LED, HIGH);
  delay(100);
  digitalWrite(WIFI_LED, LOW);
  delay(100);
  digitalWrite(WIFI_LED, HIGH);
  delay(100);
  digitalWrite(WIFI_LED, LOW);
  delay(100);
}

void setupOTA() {

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else  // U_SPIFFS
      type = "filesystem";

    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}
