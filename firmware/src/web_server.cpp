#include "web_server.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "pace_engine.h"
#include "utils.h"

static WebServer server(80);

static void addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS, GET");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

static void handleOptions() {
  addCorsHeaders();
  server.send(204);
}

static void handleStatus() {
  addCorsHeaders();

  Config config = getConfig();
  Runtime runtime = getRuntime();
  EngineState state = getEngineState();

  String response = "{";
  response += "\"state\":\"" + String(stateToString(state)) + "m\",";
  response += "\"position\":" + String(runtime.ledPosition, 2) + "m,";
  response += "\"runnerPosition\":" + String(runtime.runnerPosition, 2) + "m,";
  response += "\"distanceLeft\":" + String(config.distance - runtime.runnerPosition, 2) + "m";
  response += "}";

  server.send(200, "application/json", response);
}

static void handleSetValues() {
  addCorsHeaders();

  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"Missing JSON body\"}");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
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

  float pace = doc["pace"].as<float>();
  float distance = doc["distance"].as<float>();

  if (pace <= 0.0f || distance <= 0.0f) {
    server.send(400, "application/json", "{\"error\":\"Values must be greater than zero\"}");
    return;
  }

  startEngine(pace, distance);
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void webServerSetup() {
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/status", HTTP_OPTIONS, handleOptions);
  server.on("/set", HTTP_OPTIONS, handleOptions);
  server.on("/set", HTTP_POST, handleSetValues);
  server.begin();
}

void webServerLoop() {
  server.handleClient();
}