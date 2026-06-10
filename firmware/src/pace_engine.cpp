#include "pace_engine.h"
#include "config.h"
#include "utils.h"
#include "web_server.h"
#include <math.h>

static Config config;
static Runtime runtime;
static EngineState engineState = EngineState::Idle;
static unsigned long lastBroadcast = 0;

void updateTelemetry() {
  unsigned long now = millis();

  if (now - lastBroadcast >= 200) {
    lastBroadcast = now;
    broadcastStatus();
  }
}

void engineSetup() {
  engineState = EngineState::Idle;
  config = Config{};
  runtime = Runtime{};
}

void startEngine(float pace, float distance) {
  config.pace = pace;
  config.distance = distance;
  runtime.startTime = millis();
  runtime.runnerPosition = 0.0f;
  runtime.elapsedTime = 0.0f;
  runtime.ledPosition = 0.0f;
  runtime.ledIndex = 0;
  engineState = EngineState::Running;
}

void engineLoop() {
  if (engineState != EngineState::Running) return;

  runtime.elapsedTime = (millis() - runtime.startTime) / 1000.0f;
  float speed = paceToSpeed(config.pace);

  runtime.runnerPosition = speed * runtime.elapsedTime;

  if (runtime.runnerPosition >= config.distance) {
    runtime.runnerPosition = config.distance;
    engineState = EngineState::Idle;
    Serial.println("Simulation complete; waiting for new input.");
    return;
  }

  runtime.ledPosition = fmod(runtime.runnerPosition, TRACK_LENGTH);
  if (runtime.ledPosition < 0) {
    runtime.ledPosition += TRACK_LENGTH;
  }

  runtime.ledIndex = (int)((runtime.ledPosition / TRACK_LENGTH) * NUM_LEDS);
  if (runtime.ledIndex < 0) runtime.ledIndex = 0;
  if (runtime.ledIndex >= NUM_LEDS) runtime.ledIndex = NUM_LEDS - 1;

  Serial.print("Time: ");
  Serial.print(runtime.elapsedTime);
  Serial.print(" | Distance: ");
  Serial.print(config.distance);
  Serial.print("m | Position: ");
  Serial.print(runtime.runnerPosition);
  Serial.print("m | LED: ");
  Serial.println(runtime.ledIndex);
}

EngineState getEngineState() {
  return engineState;
}

Config getConfig() {
  return config;
}

Runtime getRuntime() {
  return runtime;
}