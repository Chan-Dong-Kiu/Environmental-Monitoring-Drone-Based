#include "thingspeak_client.h"
#include "gcs_config.h"
#include <WiFi.h>
#include <ThingSpeak.h>

WiFiClient client;
static uint32_t last_ts_update = 0;

void thingspeak_init() {
    ThingSpeak.begin(client);
}

void thingspeak_update(TelemetryData data) {
    // ThingSpeak rate limit is 15 seconds.
    if (millis() - last_ts_update < 15000) return;
    
    ThingSpeak.setField(1, data.temp_bmp);
    ThingSpeak.setField(2, data.pressure_bmp);
    ThingSpeak.setField(3, data.temp_dht);
    ThingSpeak.setField(4, data.hum_dht);
    ThingSpeak.setField(5, data.distance_srf05);
    
    int x = ThingSpeak.writeFields(TS_CHANNEL_ID, TS_WRITE_API_KEY);
    if(x == 200){
        Serial.println("Channel update successful.");
        last_ts_update = millis();
    }
    else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
}

