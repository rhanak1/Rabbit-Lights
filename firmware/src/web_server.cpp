#include "web_server.h"
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "pace_engine.h"
#include "utils.h"

static WebServer server(80);
static WebSocketsServer webSocket(81);

static void addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS, GET");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

static void handleOptions() {
  addCorsHeaders();
  server.send(204);
}

static String buildStatusJson() {
  Config config = getConfig();
  Runtime runtime = getRuntime();
  EngineState state = getEngineState();

  JsonDocument doc;

  doc["type"] = "status";
  doc["state"] = stateToString(state);
  doc["position"] = runtime.ledPosition;
  doc["runnerPosition"] = runtime.runnerPosition;
  doc["elapsedTime"] = runtime.elapsedTime;

  String response;
  serializeJson(doc, response);

  return response;
}

static void handleStatus() {
  addCorsHeaders();
  server.send(200, "application/json", buildStatusJson());
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

static void webSocketEvent(
  uint8_t clientNum,
  WStype_t type,
  uint8_t *payload,
  size_t length
) {
  switch (type) {

    case WStype_CONNECTED: {
      Serial.printf(
        "WS Client %u connected\n",
        clientNum
      );

      String statusJson = buildStatusJson();

      webSocket.sendTXT(
        clientNum,
        statusJson.c_str()
      );

      break;
  }

    case WStype_DISCONNECTED:
      Serial.printf("WS Client %u disconnected\n", clientNum);
      break;

    default:
      break;
  }
}

void broadcastStatus() {
  String statusJson = buildStatusJson();
  webSocket.broadcastTXT(statusJson);
}

void webServerSetup() {
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/status", HTTP_OPTIONS, handleOptions);

  server.on("/set", HTTP_OPTIONS, handleOptions);
  server.on("/set", HTTP_POST, handleSetValues);

  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void webServerLoop() {
  server.handleClient();
  webSocket.loop();
}