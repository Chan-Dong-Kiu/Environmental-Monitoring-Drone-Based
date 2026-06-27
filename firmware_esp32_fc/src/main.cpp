#include <Arduino.h>
#include "config/pins.h"
#include "config/flight_params.h"
#include "hal/motor_driver.h"
#include "hal/imu_driver.h"
#include "core/pid_control.h"
#include "core/mixer.h"
#include "comms/payload_link.h"

PIDState pid_roll, pid_pitch, pid_yaw;
uint32_t last_time;

void setup() {
    motor_init();
    imu_init();
    payload_link_init();
    
    pid_init(&pid_roll, PID_ROLL_KP, PID_ROLL_KI, PID_ROLL_KD);
    pid_init(&pid_pitch, PID_PITCH_KP, PID_PITCH_KI, PID_PITCH_KD);
    pid_init(&pid_yaw, PID_YAW_KP, PID_YAW_KI, PID_YAW_KD);
    
    last_time = micros();
}

void loop() {
    payload_link_update();
    
    uint32_t now = micros();
    float dt = (now - last_time) / 1000000.0f;
    
    // Only run flight loop at ~250Hz (4000us)
    if (dt >= 0.004f) {
        last_time = now;
        
        float roll, pitch, yaw;
        float gx, gy, gz;
        imu_read(&roll, &pitch, &yaw, &gx, &gy, &gz);
        
        PayloadToFCData cmd = payload_link_get_data();
        
        float roll_out = pid_compute(&pid_roll, cmd.roll, roll, dt);
        float pitch_out = pid_compute(&pid_pitch, cmd.pitch, pitch, dt);
        float yaw_out = pid_compute(&pid_yaw, cmd.yaw, gz, dt);
        
        uint16_t m1, m2, m3, m4;
        
        if (cmd.throttle > MIN_THROTTLE + 50) {
            mixer_compute(cmd.throttle, roll_out, pitch_out, yaw_out, &m1, &m2, &m3, &m4);
        } else {
            m1 = m2 = m3 = m4 = MIN_THROTTLE;
            pid_roll.integral = 0;
            pid_pitch.integral = 0;
            pid_yaw.integral = 0;
        }
        
        motor_set_pwm(m1, m2, m3, m4);
    }
}
