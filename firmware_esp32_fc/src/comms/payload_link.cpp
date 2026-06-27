#include "payload_link.h"
#include "../config/pins.h"
#include "../config/flight_params.h"

HardwareSerial PayloadSerial(2);

static PayloadToFCData current_data = {0, 0, 0, MIN_THROTTLE, 0};
static uint32_t last_packet_time = 0;

void payload_link_init() {
    PayloadSerial.begin(115200, SERIAL_8N1, PAYLOAD_RX_PIN, PAYLOAD_TX_PIN);
    last_packet_time = millis();
}

void payload_link_update() {
    if (PayloadSerial.available() >= sizeof(PayloadToFCData)) {
        PayloadSerial.readBytes((uint8_t*)&current_data, sizeof(PayloadToFCData));
        last_packet_time = millis();
        // Clear buffer if we got backlogged
        while(PayloadSerial.available()) {
            PayloadSerial.read();
        }
    }
}

bool payload_link_is_safe() {
    return (millis() - last_packet_time) < FAILSAFE_TIMEOUT_MS;
}

PayloadToFCData payload_link_get_data() {
    if (!payload_link_is_safe()) {
        PayloadToFCData safe_data = current_data;
        safe_data.throttle = FAILSAFE_THROTTLE;
        safe_data.roll = 0;
        safe_data.pitch = 0;
        safe_data.yaw = 0;
        return safe_data;
    }
    return current_data;
}
