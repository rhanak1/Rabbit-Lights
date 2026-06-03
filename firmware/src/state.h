#pragma once
#include <Arduino.h>

enum class EngineState {
  Idle,
  Running
};

struct Config {
  float pace = 0.0f;
  float distance = 0.0f;
};

struct Runtime {
  unsigned long startTime = 0;
  float runnerPosition = 0.0f;
  float ledPosition = 0.0f;
  int ledIndex = 0;
};