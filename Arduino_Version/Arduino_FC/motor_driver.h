#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H
#include <Arduino.h>

void motor_init();
void motor_set_pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
void motor_stop();

#endif

