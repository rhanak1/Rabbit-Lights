#include <Arduino.h>
#include <math.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "secrets.h"

const float TRACK_LENGTH = 400.0;
const int NUM_LEDS = 12000;


WebServer server(80);
enum SimState { IDLE, RUNNING };
SimState simulationState = IDLE;

float paceSecondsPerMile;
float distanceMeters;
float runnerPosition;
float ledPosition;
unsigned long startMillis = 0;


float paceToMetersPerSecond(float paceSecPerMile) {
  return 1609.34 / paceSecPerMile;
}

void addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS, GET");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleOptions() {
  addCorsHeaders();
  server.send(204);
}

String stateToString() {
  switch (simulationState) {
    case IDLE: return "idle";
    case RUNNING: return "running";
  }
  return "idle";
}

void handleStatus() {
  addCorsHeaders();

  String response = "{";
  response += "\"state\":\"" + stateToString() + "\",";
  response += "\"position\":" + String(ledPosition, 2) + ",";
  response += "\"distance left\":" + String(distanceMeters - runnerPosition, 2);
  response += "}";

  server.send(200, "application/json", response);
}

void handleSetValues() {
  addCorsHeaders();

  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"Missing JSON body\"}");
    return;
  }

  String body = server.arg("plain");

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  if (!doc["pace"].is<float>() && !doc["pace"].is<int>()) {
    server.send(400, "application/json", "{\"error\":\"Missing pace\"}");
    return;
  }

  if (!doc["distance"].is<float>() && !doc["distance"].is<int>()) {
    server.send(400, "application/json", "{\"error\":\"Missing distance\"}");
    return;
  }

  paceSecondsPerMile = doc["pace"].as<float>();
  distanceMeters = doc["distance"].as<float>();

  if (paceSecondsPerMile <= 0.0 || distanceMeters <= 0.0) {
    server.send(400, "application/json", "{\"error\":\"Values must be greater than zero\"}");
    return;
  }

  simulationState = RUNNING;
  startMillis = millis();

  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

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

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/status", HTTP_OPTIONS, handleOptions);

  server.on("/set", HTTP_OPTIONS, handleOptions);
  server.on("/set", HTTP_POST, handleSetValues);

  server.begin();
  Serial.println("Waiting for JSON config...");
}

void loop() {
  server.handleClient();

  if (simulationState != RUNNING) {
    delay(10);
    return;
  }

  float elapsedSec = (millis() - startMillis) / 1000.0;
  float speed = paceToMetersPerSecond(paceSecondsPerMile);

  runnerPosition = speed * elapsedSec;

  if (runnerPosition >= distanceMeters) {
    simulationState = IDLE;
    Serial.println("Simulation complete; waiting for new input.");
    return;
  }

  ledPosition = fmod(runnerPosition, TRACK_LENGTH);
  if (ledPosition < 0) {
    ledPosition += TRACK_LENGTH;
  }

  int ledIndex = (int)((ledPosition / TRACK_LENGTH) * NUM_LEDS);
  if (ledIndex < 0) ledIndex = 0;
  if (ledIndex >= NUM_LEDS) ledIndex = NUM_LEDS - 1;

  Serial.print("Time: ");
  Serial.print(elapsedSec);
  Serial.print("s | State: ");
  Serial.print(stateToString());
  Serial.print(" | Pace: ");
  Serial.print(paceSecondsPerMile);
  Serial.print(" | Distance: ");
  Serial.print(distanceMeters);
  Serial.print("m | Position: ");
  Serial.print(runnerPosition);
  Serial.print("m | LED: ");
  Serial.println(ledIndex);
}