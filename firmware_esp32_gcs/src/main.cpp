#include <Arduino.h>
#include "network/wifi_manager.h"
#include "network/thingspeak_client.h"
#include "network/web_server.h"
#include "radio/link_manager.h"

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
