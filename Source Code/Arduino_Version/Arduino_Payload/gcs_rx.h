#ifndef GCS_RX_H
#define GCS_RX_H

#include <stdint.h>

extern uint8_t g_env_mode;

void gcs_rx_init();
void gcs_rx_task(void* pvParameters);

#endif
