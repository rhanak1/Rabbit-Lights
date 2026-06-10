#pragma once
#include <Arduino.h>
#include "state.h"

void engineSetup();
void startEngine(float pace, float distance);
void engineLoop();
void updateTelemetry();

EngineState getEngineState();
Config getConfig();
Runtime getRuntime();