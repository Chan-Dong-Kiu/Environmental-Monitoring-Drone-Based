#ifndef PAYLOAD_LINK_H
#define PAYLOAD_LINK_H
#include "protocol.h"
#include <Arduino.h>

void payload_link_init();
void payload_link_update();
bool payload_link_is_safe();
PayloadToFCData payload_link_get_data();

#endif

