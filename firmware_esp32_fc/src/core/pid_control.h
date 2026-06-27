#ifndef PID_CONTROL_H
#define PID_CONTROL_H

struct PIDState {
    float kp, ki, kd;
    float integral;
    float prev_error;
};

void pid_init(PIDState* pid, float kp, float ki, float kd);
float pid_compute(PIDState* pid, float setpoint, float measured, float dt);

#endif
