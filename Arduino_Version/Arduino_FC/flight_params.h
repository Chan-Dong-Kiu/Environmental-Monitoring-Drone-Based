#ifndef FLIGHT_PARAMS_H
#define FLIGHT_PARAMS_H

// PID Constants (Tuning required)
#define PID_ROLL_KP 1.2f
#define PID_ROLL_KI 0.04f
#define PID_ROLL_KD 18.0f

#define PID_PITCH_KP 1.2f
#define PID_PITCH_KI 0.04f
#define PID_PITCH_KD 18.0f

#define PID_YAW_KP 2.0f
#define PID_YAW_KI 0.02f
#define PID_YAW_KD 0.0f

// Limits
#define MAX_ROLL_PITCH_ANGLE 30.0f
#define MAX_THROTTLE 1800 // max 2000 for standard ESC
#define MIN_THROTTLE 1000 // idle
#define FAILSAFE_THROTTLE 1100 // slow descent

#define FAILSAFE_TIMEOUT_MS 200

#endif // FLIGHT_PARAMS_H

