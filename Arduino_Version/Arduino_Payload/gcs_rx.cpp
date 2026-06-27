#include "gcs_rx.h"
#include "sys_config.h"
#include "protocol.h"
#include "atomizer.h"
#include "fc_bridge.h"
#include <Arduino.h>

HardwareSerial HC12Serial(2);
uint8_t g_env_mode = 0;

void gcs_rx_init() {
    HC12Serial.begin(9600, SERIAL_8N1, HC12_RX_PIN, HC12_TX_PIN);
}

void gcs_rx_task(void* pvParameters) {
    GCSCommand cmd;
    while(1) {
        if (HC12Serial.available() >= sizeof(GCSCommand)) {
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
            
            // Clear backlog if any
            while(HC12Serial.available()) {
                HC12Serial.read();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Yield
    }
}

