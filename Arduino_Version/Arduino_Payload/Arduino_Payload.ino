#include <Arduino.h>
#include "sys_config.h"
#include "altimeter.h"
#include "barometer.h"
#include "humidity.h"
#include "atomizer.h"
#include "fc_bridge.h"
#include "gcs_rx.h"
#include "telemetry_tx.h"

void setup() {
    Serial.begin(115200);
    
    // Init Actuators
    atomizer_init();
    
    // Init Sensors
    altimeter_init();
    barometer_init();
    humidity_init();
    
    // Init Comms
    fc_bridge_init();
    gcs_rx_init();
    
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

