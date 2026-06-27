#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H
#include <Arduino.h>

// UART Pins (to FC)
#define FC_TX_PIN 17
#define FC_RX_PIN 16

// HC-12 Radio Pins (Changed from diagram to avoid conflict with FC UART)
#define HC12_TX_PIN 32
#define HC12_RX_PIN 33

// MOSFET Atomizer
#define MOSFET_PIN 26

// BMP280 I2C
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 12

// HY-SRF05
#define TRIG_PIN 18
#define ECHO_PIN 19

// DHT11
#define DHT_PIN 23

// Task Priorities
#define TASK_PRIO_GCS_RX      4
#define TASK_PRIO_FC_BRIDGE   4
#define TASK_PRIO_TELEMETRY   2
#define TASK_PRIO_SENSORS     2
#define TASK_PRIO_DHT11       1

#endif

