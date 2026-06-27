#ifndef THINGSPEAK_CLIENT_H
#define THINGSPEAK_CLIENT_H
#include "protocol.h"

void thingspeak_init();
void thingspeak_update(TelemetryData data);

#endif

