#include "link_manager.h"
#include "gcs_config.h"
#include <Arduino.h>
#include <ArduinoJson.h>

HardwareSerial HC12Serial(2);

void link_manager_init() {
    HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
}

void link_manager_update() {
    // 1. READ FROM HC12 (Binary), SEND TO PC (JSON)
    if (HC12Serial.available() >= sizeof(TelemetryData)) {
        if (HC12Serial.peek() != 0xAA) {
            HC12Serial.read();
            return; // Wait for next sync byte
        }
        
        TelemetryData tdata;
        HC12Serial.readBytes((uint8_t*)&tdata, sizeof(TelemetryData));
        
        // Convert to JSON and send over USB Serial
        StaticJsonDocument<256> doc;
        doc["type"] = "telemetry";
        doc["temp_bmp"] = tdata.temp_bmp;
        doc["pressure_bmp"] = tdata.pressure_bmp;
        doc["temp_dht"] = tdata.temp_dht;
        doc["hum_dht"] = tdata.hum_dht;
        doc["altitude"] = tdata.altitude;
        doc["roll"] = tdata.roll;
        doc["pitch"] = tdata.pitch;
        doc["yaw"] = tdata.yaw;
        doc["atomizer_state"] = tdata.atomizer_state;
        doc["env_mode"] = tdata.env_mode;
        
        serializeJson(doc, Serial);
        Serial.println();
    }
    
    // 2. READ FROM PC (JSON), SEND TO HC12 (Binary)
    if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.length() > 0 && line.charAt(0) == '{') {
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, line);
            if (!error && doc["type"] == "cmd") {
                GCSCommand cmd;
                cmd.cmd_type = 1;
                cmd.roll = doc["roll"] | 0.0f;
                cmd.pitch = doc["pitch"] | 0.0f;
                cmd.yaw = doc["yaw"] | 0.0f;
                cmd.throttle = doc["throttle"] | 1000.0f;
                cmd.atomizer_state = doc["atomizer"] | 0;
                cmd.env_mode = doc["env_mode"] | 1;
                cmd.timestamp = millis();
                
                // [RF Collision Avoidance]
                if (HC12Serial.available() > 0) {
                    delay(20);
                }
                HC12Serial.write((uint8_t*)&cmd, sizeof(GCSCommand));
            }
        }
    }
}
