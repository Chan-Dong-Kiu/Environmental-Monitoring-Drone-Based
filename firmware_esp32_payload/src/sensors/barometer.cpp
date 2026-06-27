#include "barometer.h"
#include "../config/sys_config.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

bool barometer_init() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!bmp.begin(0x76)) {
        return false;
    }
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
    return true;
}

void barometer_read(float* temp, float* pressure) {
    *temp = bmp.readTemperature();
    *pressure = bmp.readPressure();
}
