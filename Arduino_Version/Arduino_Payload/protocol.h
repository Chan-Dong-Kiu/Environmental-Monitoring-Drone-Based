#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#pragma pack(push, 1)

// Command from GCS -> Payload (via HC-12)
struct GCSCommand {
    uint8_t cmd_type; 
    float roll;
    float pitch;
    float yaw;
    float throttle;
    uint8_t atomizer_state;
    uint8_t env_mode;
    uint32_t timestamp;
};

// Telemetry from Payload -> GCS (via HC-12)
struct TelemetryData {
    float temp_bmp;
    float pressure_bmp;
    float temp_dht;
    float hum_dht;
    float distance_srf05;
    float roll;
    float pitch;
    float yaw; // From FC
    uint8_t atomizer_state;
    uint8_t env_mode;
};

// Command from Payload -> FC (via UART)
struct PayloadToFCData {
    float roll;
    float pitch;
    float yaw;
    float throttle;
    uint32_t timestamp;
};

#pragma pack(pop)

#endif // PROTOCOL_H

