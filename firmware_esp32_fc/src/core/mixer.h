#ifndef MIXER_H
#define MIXER_H
#include <stdint.h>

void mixer_compute(float throttle, float pid_roll, float pid_pitch, float pid_yaw, 
                   uint16_t* m1, uint16_t* m2, uint16_t* m3, uint16_t* m4);

#endif
