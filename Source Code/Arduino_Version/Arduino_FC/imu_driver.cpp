#include "imu_driver.h"
#include "pins.h"
#include <Wire.h>
#include "MPU6050.h"

MPU6050 mpu;

bool imu_init() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000);
    mpu.initialize();
    if (!mpu.testConnection()) {
        return false;
    }
    // Set offsets if known, or calibrate here
    return true;
}

void imu_read(float* roll, float* pitch, float* yaw, float* gyro_x, float* gyro_y, float* gyro_z) {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // Very simple complementary filter placeholder. 
    float accX = ax / 16384.0;
    float accY = ay / 16384.0;
    float accZ = az / 16384.0;
    
    *gyro_x = gx / 131.0;
    *gyro_y = gy / 131.0;
    *gyro_z = gz / 131.0;
    
    *roll = atan2(accY, accZ) * 180.0 / PI;
    *pitch = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * 180.0 / PI;
    *yaw = 0; // Yaw drift without mag
}

