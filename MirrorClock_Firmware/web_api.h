#pragma once
#include <ESP8266WebServer.h>
#include "core_api.h"
#include "web_dashboard.h"

class WebApi {
private:
    ESP8266WebServer server;
    CoreApi& api;

    void handleCors() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(200, "text/plain", "OK");
    }

public:
    WebApi(CoreApi& a) : server(WEB_SERVER_PORT), api(a) {}

    void begin() {
        server.onNotFound([this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            if (server.method() == HTTP_OPTIONS) {
                this->handleCors();
            } else {
                server.send(404, "application/json", api.createJsonResponse(false, "Endpoint not found"));
            }
        });
        
        server.on("/", HTTP_GET, [this]() {
            server.sendHeader("Content-Encoding", "gzip");
            server.send_P(200, "text/html", (const char*)DASHBOARD_HTML_GZ, DASHBOARD_HTML_SIZE);
        });
        
        server.on("/api/status", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", api.createJsonResponse(true, "Status retrieved", api.getStatus()));
        });
        
        server.on("/api/state", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", api.createJsonResponse(true, "State retrieved", api.getState()));
        });
        
        server.on("/api/state", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            if (!server.hasArg("plain")) {
                server.send(400, "application/json", api.createJsonResponse(false, "No JSON body provided"));
                return;
            }
            if (api.updateState(server.arg("plain"))) {
                server.send(200, "application/json", api.createJsonResponse(true, "State updated", api.getState()));
            } else {
                server.send(400, "application/json", api.createJsonResponse(false, "Invalid JSON"));
            }
        });

        server.on("/api/config", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", api.createJsonResponse(true, "Config retrieved", api.getConfig()));
        });
        
        server.on("/api/config", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            if (!server.hasArg("plain")) {
                server.send(400, "application/json", api.createJsonResponse(false, "No JSON body provided"));
                return;
            }
            if (api.updateConfig(server.arg("plain"))) {
                server.send(200, "application/json", api.createJsonResponse(true, "Config updated. Please restart device for some changes to take effect."));
            } else {
                server.send(400, "application/json", api.createJsonResponse(false, "Invalid JSON"));
            }
        });

        server.on("/api/system/reboot", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", api.createJsonResponse(true, "Rebooting..."));
            api.reboot();
        });
        
        server.begin();
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WEB "API server started on port " + String(WEB_SERVER_PORT));
        #endif
    }

    void handleClient() {
        server.handleClient();
    }
};
