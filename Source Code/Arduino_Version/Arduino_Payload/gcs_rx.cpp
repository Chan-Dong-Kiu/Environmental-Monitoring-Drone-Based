#include "gcs_rx.h"
#include "sys_config.h"
#include "protocol.h"
#include "atomizer.h"
#include "fc_bridge.h"
#include <Arduino.h>

HardwareSerial HC12Serial(2);
uint8_t g_env_mode = 1; // Default to ON so sensors send data immediately

void gcs_rx_init() {
    HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
}

void gcs_rx_task(void* pvParameters) {
    GCSCommand cmd;
    uint32_t last_rx_time = 0;
    uint32_t last_print_time = 0;
    while(1) {
        if (HC12Serial.available() >= sizeof(GCSCommand)) {
            // Sync frame: cmd_type must be 1. If not, drop 1 byte and try again.
            if (HC12Serial.peek() != 1) {
                HC12Serial.read(); 
                continue;
            }

            HC12Serial.readBytes((uint8_t*)&cmd, sizeof(GCSCommand));
            
            // Forward flight commands to FC
            PayloadToFCData fc_data;
            fc_data.roll = cmd.roll;
            fc_data.pitch = cmd.pitch;
            fc_data.yaw = cmd.yaw;
            fc_data.throttle = cmd.throttle;
            fc_data.timestamp = cmd.timestamp;
            fc_bridge_send(fc_data);
            
            // Handle payload commands
            if (cmd.atomizer_state != atomizer_get_state()) {
                atomizer_set(cmd.atomizer_state > 0);
            }
            g_env_mode = cmd.env_mode;
            
            last_rx_time = millis();
            // DO NOT clear backlog. Let the UART buffer handle incoming bytes.
        }

        // Print warning if no signal for 3 seconds
        if (millis() - last_rx_time > 3000 && millis() - last_print_time > 3000) {
            Serial.println("[WARNING] HC-12 connection lost! No signal from GCS.");
            last_print_time = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Yield
    }
}

