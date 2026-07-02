#ifndef LINK_MANAGER_H
#define LINK_MANAGER_H
#include "protocol.h"

void link_manager_init();
void link_manager_update();
void link_manager_send_cmd(GCSCommand cmd);
TelemetryData link_manager_get_telemetry();

#endif

