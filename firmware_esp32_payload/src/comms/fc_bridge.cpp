#include "fc_bridge.h"
#include "../config/sys_config.h"
#include <Arduino.h>

HardwareSerial FCSerial(1);

void fc_bridge_init() {
    FCSerial.begin(115200, SERIAL_8N1, FC_RX_PIN, FC_TX_PIN);
}

void fc_bridge_send(PayloadToFCData data) {
    FCSerial.write((uint8_t*)&data, sizeof(PayloadToFCData));
}
