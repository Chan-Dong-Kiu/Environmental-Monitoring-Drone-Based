#include "telemetry_tx.h"
#include "sys_config.h"
#include "protocol.h"
#include "altimeter.h"
#include "barometer.h"
#include "humidity.h"
#include "atomizer.h"
#include "gcs_rx.h"
#include <Arduino.h>

extern HardwareSerial HC12Serial; // Declared in gcs_rx.cpp

void telemetry_tx_task(void* pvParameters) {
    TelemetryData tdata;
    
    while(1) {
        if (g_env_mode) {
            barometer_read(&tdata.temp_bmp, &tdata.pressure_bmp);
            humidity_read(&tdata.temp_dht, &tdata.hum_dht);
            
            altimeter_trigger();
            vTaskDelay(pdMS_TO_TICKS(20)); // wait for echo
            tdata.distance_srf05 = altimeter_get_distance();
            
            Serial.println("--- SENSOR DATA ---");
            Serial.print("BMP280  - Temp: "); Serial.print(tdata.temp_bmp); Serial.print(" C, Press: "); Serial.print(tdata.pressure_bmp); Serial.println(" hPa");
            Serial.print("DHT11   - Temp: "); Serial.print(tdata.temp_dht); Serial.print(" C, Hum: "); Serial.print(tdata.hum_dht); Serial.println(" %");
            Serial.print("SRF05   - Dist: "); Serial.print(tdata.distance_srf05); Serial.println(" cm");
            Serial.println("-------------------");
        } else {
            tdata.temp_bmp = 0;
            tdata.pressure_bmp = 0;
            tdata.temp_dht = 0;
            tdata.hum_dht = 0;
            tdata.distance_srf05 = 0;
        }
        
        tdata.atomizer_state = atomizer_get_state();
        tdata.env_mode = g_env_mode;
        
        tdata.roll = 0;
        tdata.pitch = 0;
        tdata.yaw = 0;
        
        HC12Serial.write((uint8_t*)&tdata, sizeof(TelemetryData));
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Send telemetry every 1 second
    }
}

