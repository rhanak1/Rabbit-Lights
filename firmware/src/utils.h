#pragma once
#include <Arduino.h>
#include "state.h"

float paceToSpeed(float pace);
const char* stateToString(EngineState state);