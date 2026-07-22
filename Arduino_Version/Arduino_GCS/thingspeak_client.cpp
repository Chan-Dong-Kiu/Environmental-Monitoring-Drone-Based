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
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[ERROR] Cannot update ThingSpeak. WiFi is disconnected!");
        last_ts_update = millis(); // Reset to avoid spamming
        return;
    }
    
    Serial.println("[INFO] Attempting to push data to ThingSpeak...");
    
    ThingSpeak.setField(1, data.temp_bmp > 0 ? data.temp_bmp : data.temp_dht); // Temperature
    ThingSpeak.setField(2, data.pressure_bmp); // Pressure
    ThingSpeak.setField(3, data.altitude); // Height
    ThingSpeak.setField(4, data.hum_dht); // Humidity
    
    int x = ThingSpeak.writeFields(TS_CHANNEL_ID, TS_WRITE_API_KEY);
    if(x == 200){
        Serial.println("[SUCCESS] ThingSpeak channel updated successfully!");
        last_ts_update = millis();
    }
    else {
        Serial.print("[ERROR] Problem updating ThingSpeak. HTTP error code: ");
        Serial.println(x);
        if (x == -301) Serial.println("-> Hint: Lỗi sai Channel ID hoặc API Key.");
        else if (x == -304) Serial.println("-> Hint: Lỗi Timeout (Mạng yếu, chập chờn hoặc rớt mạng).");
        last_ts_update = millis();
    }
}

