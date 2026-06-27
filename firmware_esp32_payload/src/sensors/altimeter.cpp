#include "altimeter.h"
#include "../config/sys_config.h"

static volatile uint32_t echo_start = 0;
static volatile uint32_t echo_duration = 0;
static volatile bool echo_done = false;

void IRAM_ATTR echo_isr() {
    if (digitalRead(ECHO_PIN) == HIGH) {
        echo_start = micros();
    } else {
        echo_duration = micros() - echo_start;
        echo_done = true;
    }
}

void altimeter_init() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echo_isr, CHANGE);
}

void altimeter_trigger() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
}

float altimeter_get_distance() {
    if (echo_done) {
        // Distance in cm = (duration / 2) / 29.1
        float dist = (echo_duration / 2.0f) * 0.0343f;
        echo_done = false;
        return dist;
    }
    return -1.0f; // Data not ready
}
