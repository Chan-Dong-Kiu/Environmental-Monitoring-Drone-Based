#include "motor_driver.h"
#include "pins.h"

// ESCs typically expect 50Hz (20ms period) to 400Hz. Let's use 250Hz.
// We'll use ledc from ESP32 framework
#define PWM_FREQ 250
#define PWM_RES 11 // 11-bit resolution (0-2047)

void motor_init() {
    ledcSetup(0, PWM_FREQ, PWM_RES);
    ledcSetup(1, PWM_FREQ, PWM_RES);
    ledcSetup(2, PWM_FREQ, PWM_RES);
    ledcSetup(3, PWM_FREQ, PWM_RES);

    ledcAttachPin(MOTOR1_PIN, 0);
    ledcAttachPin(MOTOR2_PIN, 1);
    ledcAttachPin(MOTOR3_PIN, 2);
    ledcAttachPin(MOTOR4_PIN, 3);
    
    motor_stop();
}

// Input expected 1000 - 2000 microseconds
void motor_set_pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    auto calc_duty = [](uint16_t us) {
        if (us < 1000) us = 1000;
        if (us > 2000) us = 2000;
        return (us * 2048) / 4000;
    };

    ledcWrite(0, calc_duty(m1));
    ledcWrite(1, calc_duty(m2));
    ledcWrite(2, calc_duty(m3));
    ledcWrite(3, calc_duty(m4));
}

void motor_stop() {
    motor_set_pwm(1000, 1000, 1000, 1000);
}

