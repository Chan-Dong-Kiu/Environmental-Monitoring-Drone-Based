#include <Arduino.h>
#include "wifi_manager.h"
#include "thingspeak_client.h"
#include "web_server.h"
#include "link_manager.h"

void setup() {
    Serial.begin(115200);
    
    wifi_init();
    if (wifi_is_connected()) {
        thingspeak_init();
    }
    
    link_manager_init();
    web_server_init();
}

void loop() {
    link_manager_update();
    delay(10);
}

