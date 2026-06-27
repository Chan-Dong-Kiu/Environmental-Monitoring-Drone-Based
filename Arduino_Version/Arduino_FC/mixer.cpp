#include "mixer.h"
#include "flight_params.h"

void mixer_compute(float throttle, float pid_roll, float pid_pitch, float pid_yaw, 
                   uint16_t* m1, uint16_t* m2, uint16_t* m3, uint16_t* m4) {
                       
    // X Configuration
    // M4(FL, CW)  M1(FR, CCW)
    //        \  /
    //        /  \
    // M3(RL, CCW) M2(RR, CW)
    
    float motor1 = throttle - pid_roll - pid_pitch + pid_yaw;
    float motor2 = throttle - pid_roll + pid_pitch - pid_yaw;
    float motor3 = throttle + pid_roll + pid_pitch + pid_yaw;
    float motor4 = throttle + pid_roll - pid_pitch - pid_yaw;
    
    auto constrain_pwm = [](float val) -> uint16_t {
        if(val > MAX_THROTTLE) return MAX_THROTTLE;
        if(val < MIN_THROTTLE) return MIN_THROTTLE;
        return (uint16_t)val;
    };
    
    *m1 = constrain_pwm(motor1);
    *m2 = constrain_pwm(motor2);
    *m3 = constrain_pwm(motor3);
    *m4 = constrain_pwm(motor4);
}

