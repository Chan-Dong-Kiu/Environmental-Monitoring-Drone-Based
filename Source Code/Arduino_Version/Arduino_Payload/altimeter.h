#ifndef ALTIMETER_H
#define ALTIMETER_H
#include <Arduino.h>

void altimeter_init();
void altimeter_trigger();
float altimeter_get_distance();

#endif

