#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"
#include "pace_engine.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nConnected to: ");
  Serial.println(WiFi.localIP());

  engineSetup();
  webServerSetup();
}

void loop() {
  webServerLoop();
  engineLoop();
}