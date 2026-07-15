#include "barometer.h"
#include "sys_config.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;
float base_altitude = 0.0f;

bool barometer_init() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
        return false;
    }
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
                    
    // Lấy mốc độ cao tại mặt đất lúc khởi động (trung bình 10 lần đo)
    float sum_alt = 0;
    for(int i = 0; i < 10; i++) {
        sum_alt += bmp.readAltitude(1013.25f);
        delay(50);
    }
    base_altitude = sum_alt / 10.0f;
    return true;
}

void barometer_read(float* temp, float* pressure) {
    *temp = bmp.readTemperature();
    *pressure = bmp.readPressure() / 100.0f; // Chuyển từ Pa sang hPa
}

float barometer_get_relative_altitude() {
    float current_alt = bmp.readAltitude(1013.25f);
    return current_alt - base_altitude;
}

