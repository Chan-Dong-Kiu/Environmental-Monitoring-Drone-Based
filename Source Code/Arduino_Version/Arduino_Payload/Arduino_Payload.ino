#include <Arduino.h>
#include "sys_config.h"
#include "altimeter.h"
#include "barometer.h"
#include "humidity.h"
#include "atomizer.h"
#include "fc_bridge.h"
#include "gcs_rx.h"
#include "telemetry_tx.h"
/*
 * =========================================================
 * HARDWARE WIRING WARNING (HC-12)
 * =========================================================
 * To prevent the "only sent, not received" error or garbage data:
 * 1. The HC-12 TX pin MUST be connected to the ESP32 RX pin (33).
 * 2. The HC-12 RX pin MUST be connected to the ESP32 TX pin (32).
 * Do NOT connect TX to TX or RX to RX!
 * 3. The HC-12 VCC should be connected to the ESP32 5V (VIN) pin for stable RF power.
 * 4. The HC-12 SET pin MUST be left completely disconnected during flight.
 * =========================================================
 */

void setup() {
    Serial.begin(115200);
    Serial.println("================================");
    Serial.println("   PAYLOAD SYSTEM STARTING...   ");
    Serial.println("================================");
    
    // Init Actuators
    atomizer_init();
    Serial.println("[OK] Atomizer Initialized");
    
    // Init Sensors
    altimeter_init();
    Serial.println("[OK] Altimeter (SRF05) Initialized");
    
    if (barometer_init()) {
        Serial.println("[OK] Barometer (BMP280) Initialized");
    } else {
        Serial.println("[ERROR] BMP280 initialization failed! Check wiring.");
    }
    
    humidity_init();
    Serial.println("[OK] Humidity (DHT11) Initialized");
    
    // Init Comms
    fc_bridge_init();
    Serial.println("[OK] FC UART Bridge Initialized");
    
    gcs_rx_init();
    Serial.println("[OK] GCS HC-12 Radio Initialized");
    
    Serial.println("System Ready. Starting FreeRTOS Tasks...");
    
    // Start FreeRTOS Tasks
    xTaskCreatePinnedToCore(
        gcs_rx_task, 
        "GCS_RX", 
        4096, 
        NULL, 
        TASK_PRIO_GCS_RX, 
        NULL, 
        1); // Core 1
        
    xTaskCreatePinnedToCore(
        telemetry_tx_task, 
        "TELEMETRY_TX", 
        4096, 
        NULL, 
        TASK_PRIO_TELEMETRY, 
        NULL, 
        1); // Core 1
}

void loop() {
    // FreeRTOS is handling the tasks.
    // We can delete the setup/loop task to free memory
    vTaskDelete(NULL);
}

