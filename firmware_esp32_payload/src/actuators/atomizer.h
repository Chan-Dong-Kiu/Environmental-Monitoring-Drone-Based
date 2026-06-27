#ifndef ATOMIZER_H
#define ATOMIZER_H
#include <Arduino.h>

void atomizer_init();
void atomizer_set(bool state);
bool atomizer_get_state();

#endif
