#include "pid_control.h"

void pid_init(PIDState* pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0;
    pid->prev_error = 0;
}

float pid_compute(PIDState* pid, float setpoint, float measured, float dt) {
    float error = setpoint - measured;
    pid->integral += error * dt;
    
    // anti-windup
    if(pid->integral > 400) pid->integral = 400;
    if(pid->integral < -400) pid->integral = -400;
    
    float derivative = (error - pid->prev_error) / dt;
    pid->prev_error = error;
    
    return (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative);
}

