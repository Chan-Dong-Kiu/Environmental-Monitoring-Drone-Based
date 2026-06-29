#include <Arduino.h>
#include "wifi_manager.h"
#include "thingspeak_client.h"
#include "web_server.h"
#include "link_manager.h"

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

