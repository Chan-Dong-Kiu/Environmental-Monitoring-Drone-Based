#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

bool imu_init();
void imu_read(float* roll, float* pitch, float* yaw, float* gyro_x, float* gyro_y, float* gyro_z);

#endif
