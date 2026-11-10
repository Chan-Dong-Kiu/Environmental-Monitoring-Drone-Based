#include "humidity.h"
#include "sys_config.h"
#include <DHT.h>

DHT dht(DHT_PIN, DHT11);

void humidity_init() {
    dht.begin();
}

void humidity_read(float* temp, float* hum) {
    *temp = dht.readTemperature();
    *hum = dht.readHumidity();
}

