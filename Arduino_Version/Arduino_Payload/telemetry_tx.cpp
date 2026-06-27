#include "telemetry_tx.h"
#include "sys_config.h"
#include "protocol.h"
#include "altimeter.h"
#include "barometer.h"
#include "humidity.h"
#include "atomizer.h"
#include <Arduino.h>

extern HardwareSerial HC12Serial; // Declared in gcs_rx.cpp

void telemetry_tx_task(void* pvParameters) {
    TelemetryData tdata;
    
    while(1) {
        barometer_read(&tdata.temp_bmp, &tdata.pressure_bmp);
        humidity_read(&tdata.temp_dht, &tdata.hum_dht);
        
        altimeter_trigger();
        vTaskDelay(pdMS_TO_TICKS(20)); // wait for echo
        tdata.distance_srf05 = altimeter_get_distance();
        
        tdata.atomizer_state = atomizer_get_state();
        
        tdata.roll = 0;
        tdata.pitch = 0;
        tdata.yaw = 0;
        
        HC12Serial.write((uint8_t*)&tdata, sizeof(TelemetryData));
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Send telemetry every 1 second
    }
}

