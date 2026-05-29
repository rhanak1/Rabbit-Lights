#include <Arduino.h>
#include <math.h>
#include <WiFi.h>

// ===================== CONFIG =====================
const float TRACK_LENGTH = 400.0;     // standard track
const int NUM_LEDS = 12000;               // change to your strip length

// Example pace: 4:00 mile
float paceSecondsPerMile = 280.0;

// ===================== STATE =====================
unsigned long startMillis = 0;

// smoothing (optional but recommended)
float smoothedPosition = 0.0;
const float alpha = 0.2;  // 0.1–0.3 typical

// ===================== CONVERSION =====================
float paceToMetersPerSecond(float paceSecPerMile) {
    return 1609.34 / paceSecPerMile;
}

// ===================== SETUP =====================
void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin("Samsung Galaxy S10_1533", "Hanak524");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("\nConnected to: ");
    Serial.println(WiFi.localIP());


    startMillis = millis();

    Serial.println("Pacing system started");
}

// ===================== MAIN LOOP =====================
void loop() {

    // 1. Get elapsed time in seconds (FLOAT for precision)
    float elapsedSec = (millis() - startMillis) / 1000.0;

    // 2. Convert pace → speed (m/s)
    float speed = paceToMetersPerSecond(paceSecondsPerMile);

    // 3. Compute raw position (meters)
    float position = speed * elapsedSec;

    // 4. Wrap around track length
    position = fmod(position, TRACK_LENGTH);
    if (position < 0) position += TRACK_LENGTH;

    // 5. Smooth position (prevents jitter)
    //smoothedPosition = alpha * position + (1.0 - alpha) * smoothedPosition;

    // 6. Map position → LED index
    int ledIndex = (int)((position / TRACK_LENGTH) * NUM_LEDS);

    // clamp safety
    if (ledIndex < 0) ledIndex = 0;
    if (ledIndex >= NUM_LEDS) ledIndex = NUM_LEDS - 1;

    // 7. Output (replace this later with FastLED)
    Serial.print("Time: ");
    Serial.print(elapsedSec);

    Serial.print("s | Position: ");
    Serial.print(position);

    Serial.print("m | LED: ");
    Serial.println(ledIndex);

    // 8. Run loop at stable update rate (~20–50Hz)
    delay(20);
}