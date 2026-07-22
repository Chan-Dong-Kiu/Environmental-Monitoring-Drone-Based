#include "atomizer.h"
#include "sys_config.h"

static bool is_on = false;

void atomizer_init() {
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW);
}

void atomizer_set(bool state) {
    is_on = state;
    digitalWrite(MOSFET_PIN, state ? HIGH : LOW);
}

bool atomizer_get_state() {
    return is_on;
}

