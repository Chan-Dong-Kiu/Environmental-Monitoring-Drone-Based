#include "link_manager.h"
#include "gcs_config.h"
#include "thingspeak_client.h"
#include <Arduino.h>

HardwareSerial HC12Serial(2);

void link_manager_init() {
    HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
}

static uint32_t last_rx_time = 0;
static TelemetryData latest_tdata;

void link_manager_update() {
    TelemetryData tdata;
    if (HC12Serial.available() >= sizeof(TelemetryData)) {
        if (HC12Serial.peek() != 0xAA) {
            HC12Serial.read();
            return; // Wait for next sync byte
        }
        
        HC12Serial.readBytes((uint8_t*)&tdata, sizeof(TelemetryData));
        
        latest_tdata = tdata; // Store for web UI
        
        Serial.println("[HC-12 RX] Received Telemetry Data from Payload!");
        last_rx_time = millis();
        if (last_rx_time == 0) last_rx_time = 1; // prevent 0
        
        // DO NOT Push to ThingSpeak synchronously here!
        // It is now handled asynchronously by a FreeRTOS task in Arduino_GCS.ino
        // to prevent network timeouts (-304) from freezing the HC-12 radio.
        
        // DO NOT clear backlog here!
    }
    
    // Timeout check
    if (last_rx_time != 0 && millis() - last_rx_time > 5000) {
        Serial.println("[ERROR] HC-12 Connection Lost! No data from Payload for 5 seconds.");
        last_rx_time = millis(); // Reset to avoid spamming
    } else if (last_rx_time == 0 && millis() > 5000) {
        Serial.println("[WARNING] Waiting for HC-12 telemetry from Payload...");
        last_rx_time = millis();
    }
}

void link_manager_send_cmd(GCSCommand cmd) {
    // [RF Collision Avoidance]
    // If HC12 is currently receiving telemetry from Payload, wait briefly before transmitting.
    // This prevents GCS from talking over the Payload in half-duplex mode.
    if (HC12Serial.available() > 0) {
        delay(20); // Briefly wait for incoming bytes to finish parsing
    }
    
    HC12Serial.write((uint8_t*)&cmd, sizeof(GCSCommand));
    // We send commands constantly (every 100ms), so printing here might spam. 
    // We will only print if it's an atomizer toggle or env mode toggle to reduce spam.
    static uint8_t last_env = 255;
    static uint8_t last_atm = 255;
    if (cmd.env_mode != last_env || cmd.atomizer_state != last_atm) {
        Serial.println("[HC-12 TX] Sent Command (Settings Changed)");
        last_env = cmd.env_mode;
        last_atm = cmd.atomizer_state;
    }
}


TelemetryData link_manager_get_telemetry() {
    return latest_tdata;
}
