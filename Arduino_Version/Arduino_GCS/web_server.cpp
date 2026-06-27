#include "web_server.h"
#include "index_html.h"
#include "link_manager.h"
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);

void web_server_init() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", INDEX_HTML);
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/cmd", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject jsonObj = json.as<JsonObject>();
        
        GCSCommand cmd;
        cmd.cmd_type = 1;
        cmd.roll = jsonObj["roll"] | 0.0f;
        cmd.pitch = jsonObj["pitch"] | 0.0f;
        cmd.yaw = jsonObj["yaw"] | 0.0f;
        cmd.throttle = jsonObj["throttle"] | 1000.0f;
        cmd.atomizer_state = jsonObj["atomizer"] | 0;
        cmd.env_mode = jsonObj["env_mode"] | 0;
        cmd.timestamp = millis();
        
        link_manager_send_cmd(cmd);
        
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.addHandler(handler);
    server.begin();
    Serial.println("Web Server started.");
}

