#include "link_manager.h"
#include "../config/gcs_config.h"
#include "../network/thingspeak_client.h"
#include <Arduino.h>

HardwareSerial HC12Serial(2);

void link_manager_init() {
    HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
}

void link_manager_update() {
    TelemetryData tdata;
    if (HC12Serial.available() >= sizeof(TelemetryData)) {
        HC12Serial.readBytes((uint8_t*)&tdata, sizeof(TelemetryData));
        
        // Push to ThingSpeak
        thingspeak_update(tdata);
        
        while(HC12Serial.available()) {
            HC12Serial.read();
        }
    }
}

void link_manager_send_cmd(GCSCommand cmd) {
    HC12Serial.write((uint8_t*)&cmd, sizeof(GCSCommand));
}
