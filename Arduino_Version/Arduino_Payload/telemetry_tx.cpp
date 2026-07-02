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
        float distance = 0.0f;
        float pressure = 0.0f;
        float bmp_altitude = 0.0f;
        
        // Trigger SRF05 in all cases to check if we are below 5m
        altimeter_trigger();
        vTaskDelay(pdMS_TO_TICKS(20)); // wait for echo
        distance = altimeter_get_distance();
        
        if (g_env_mode) {
            barometer_read(&tdata.temp_bmp, &tdata.pressure_bmp);
            humidity_read(&tdata.temp_dht, &tdata.hum_dht);
            pressure = tdata.pressure_bmp;
            
            // Calculate BMP280 Altitude
            bmp_altitude = 44330.0f * (1.0f - pow(pressure / 1013.25f, 0.1903f));
            
            if (distance > 0 && distance < 500.0f) {
                tdata.altitude = distance; // Altitude = HY-SRF05
            } else {
                tdata.altitude = bmp_altitude; // Altitude = BMP280
            }
            
            Serial.println("--- SENSOR DATA (ENV ON) ---");
            Serial.print("BMP280  - Temp: "); Serial.print(tdata.temp_bmp); Serial.print(" C, Press: "); Serial.print(tdata.pressure_bmp); Serial.println(" hPa");
            Serial.print("DHT11   - Temp: "); Serial.print(tdata.temp_dht); Serial.print(" C, Hum: "); Serial.print(tdata.hum_dht); Serial.println(" %");
            Serial.print("Altitude: "); Serial.print(tdata.altitude); Serial.println(distance < 500.0f ? " cm (SRF05)" : " cm (BMP280)");
            Serial.println("-------------------");
        } else {
            // Environment Mode Off
            tdata.temp_bmp = 0;
            tdata.pressure_bmp = 0;
            tdata.temp_dht = 0;
            tdata.hum_dht = 0;
            
            if (distance > 0 && distance < 500.0f) {
                tdata.altitude = distance; // Altitude = HY-SRF05
            } else {
                // BMP280 Measures Pressure Only To Calculate Altitude
                float dummy_temp;
                barometer_read(&dummy_temp, &pressure);
                bmp_altitude = 44330.0f * (1.0f - pow(pressure / 1013.25f, 0.1903f));
                tdata.altitude = bmp_altitude; // Altitude = BMP280
            }
            
            Serial.println("--- SENSOR DATA (ENV OFF) ---");
            Serial.print("Altitude: "); Serial.print(tdata.altitude); Serial.println(distance < 500.0f ? " cm (SRF05)" : " cm (BMP280)");
            Serial.println("-------------------");
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

