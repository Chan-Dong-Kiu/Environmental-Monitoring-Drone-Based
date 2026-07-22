#include <Arduino.h>
#include "wifi_manager.h"
#include "thingspeak_client.h"
#include "web_server.h"
#include "link_manager.h"
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
    Serial.begin(115200);
    Serial.println("================================");
    Serial.println("     GCS SYSTEM STARTING...     ");
    Serial.println("================================");
    
    wifi_init();
    if (wifi_is_connected()) {
        thingspeak_init();
        Serial.println("[OK] ThingSpeak Client Initialized");
    } else {
        Serial.println("[ERROR] ThingSpeak disabled due to WiFi failure");
    }
    
    link_manager_init();
    Serial.println("[OK] Payload HC-12 Radio Initialized");
    
    web_server_init();
    Serial.println("System Ready. Entering main loop...");
}

void loop() {
    link_manager_update();
    delay(10);
}

