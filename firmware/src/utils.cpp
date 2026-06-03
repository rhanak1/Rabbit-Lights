#include "utils.h"

float paceToSpeed(float pace) {
  return 1609.344f / pace;
}

const char* stateToString(EngineState state) {
  switch (state) {
    case EngineState::Idle: return "idle";
    case EngineState::Running: return "running";
  }
  return "idle";
}