#include <Arduino.h>
#include "link_manager.h"
#include <ArduinoJson.h>

/*
 * =========================================================
 * HARDWARE WIRING WARNING (HC-12)
 * =========================================================
 * To prevent the "only sent, not received" error or garbage data:
 * 1. The HC-12 TX pin MUST be connected to the ESP32 RX pin (16).
 * 2. The HC-12 RX pin MUST be connected to the ESP32 TX pin (17).
 * Do NOT connect TX to TX or RX to RX!
 * 3. The HC-12 VCC should be connected to the ESP32 5V (VIN) pin for stable RF power.
 * 4. The HC-12 SET pin MUST be left completely disconnected during flight.
 * =========================================================
 */

void setup() {
    // We use 115200 baud for USB Serial connection to the Node.js backend
    Serial.begin(115200);
    
    // Send a startup message as a JSON object so the Node.js backend can parse it safely
    StaticJsonDocument<256> doc;
    doc["type"] = "log";
    doc["message"] = "GCS SYSTEM STARTING... USB RADIO BRIDGE MODE ENABLED";
    serializeJson(doc, Serial);
    Serial.println();
    
    link_manager_init();
    
    doc["message"] = "Payload HC-12 Radio Initialized";
    serializeJson(doc, Serial);
    Serial.println();
    
    doc["message"] = "System Ready. Operating as USB Radio Bridge.";
    serializeJson(doc, Serial);
    Serial.println();
}

void loop() {
    link_manager_update();
    // Do NOT use blocking delays here!
    // delay(10) is removed to ensure maximum UART throughput.
}
