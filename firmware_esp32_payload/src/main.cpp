#include <Arduino.h>
#include "config/sys_config.h"
#include "sensors/altimeter.h"
#include "sensors/barometer.h"
#include "sensors/humidity.h"
#include "actuators/atomizer.h"
#include "comms/fc_bridge.h"
#include "comms/gcs_rx.h"
#include "comms/telemetry_tx.h"

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
