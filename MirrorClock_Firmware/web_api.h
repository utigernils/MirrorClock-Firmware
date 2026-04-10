#pragma once
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "wifi_connection.h"
#include "time_sync.h"
#include "led_driver.h"
#include "light_sensor.h"

class WebApi {
private:
    ESP8266WebServer server;
    NetworkManager& network;
    TimeManager& timeMgr;
    LedDriver& leds;
    LightSensor& sensor;

    String createJsonResponse(bool success, String message = "", String data = "") {
        DynamicJsonDocument doc(1024);
        doc["success"] = success;
        doc["timestamp"] = timeMgr.getEpochTime();
        
        if (message != "") {
            doc["message"] = message;
        }
        
        if (data != "") {
            DynamicJsonDocument dataDoc(512);
            deserializeJson(dataDoc, data);
            doc["data"] = dataDoc;
        }
        
        String response;
        serializeJson(doc, response);
        return response;
    }

    void handleStatus() {
        DynamicJsonDocument doc(2048);
        
        doc["system"]["uptime"] = millis();
        doc["system"]["free_heap"] = ESP.getFreeHeap();
        doc["system"]["chip_id"] = ESP.getChipId();
        doc["system"]["flash_size"] = ESP.getFlashChipSize();
        doc["system"]["cpu_freq"] = ESP.getCpuFreqMHz();
        
        doc["wifi"]["connected"] = network.isConnected();
        doc["wifi"]["ssid"] = network.getSSID();
        doc["wifi"]["ip"] = network.getIP();
        doc["wifi"]["rssi"] = network.getRSSI();
        doc["wifi"]["mac"] = network.getMacAddress();
        
        doc["time"]["hour"] = timeMgr.getHour();
        doc["time"]["minute"] = timeMgr.getMinute();
        doc["time"]["epoch"] = timeMgr.getEpochTime();
        doc["time"]["ntp_server"] = NTP_SERVER;
        doc["time"]["timezone"] = TIMEZONE;
        
        doc["led"]["enabled"] = LED_ENABLED;
        doc["led"]["brightness"] = LED_BRIGHTNESS;
        doc["led"]["auto_brightness"] = LED_AUTO_BRIGHTNESS;
        doc["led"]["color"]["r"] = LED_R;
        doc["led"]["color"]["g"] = LED_G;
        doc["led"]["color"]["b"] = LED_B;
        doc["led"]["count"] = LED_COUNT;
        doc["led"]["pin"] = LED_PIN;
        
        doc["light_sensor"]["lux"] = sensor.getLightLevel();
        doc["light_sensor"]["auto_brightness"] = AUTO_BRIGHTNESS_ENABLED;
        doc["light_sensor"]["calculated_brightness"] = sensor.getBrightness();
        
        doc["config"]["update_interval"] = UPDATE_INTERVAL;
        doc["config"]["web_port"] = WEB_SERVER_PORT;
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }

    void handlePower() {
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WEB "API call: /api/power");
        #endif
        
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
            return;
        }
        
        String body = server.arg("plain");
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, body);
        
        if (error) {
            server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
            return;
        }
        
        if (!doc.containsKey("enabled")) {
            server.send(400, "application/json", createJsonResponse(false, "Missing 'enabled' parameter"));
            return;
        }
        
        LED_ENABLED = doc["enabled"];
        
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_LED "Power state changed: " + String(LED_ENABLED ? "ON" : "OFF"));
        #endif
        
        if (!LED_ENABLED) {
            leds.clear();
            leds.show();
        } else {
            extern int lastHour, lastMinute;
            lastHour = -1;
            lastMinute = -1;
        }
        
        DynamicJsonDocument responseData(128);
        responseData["enabled"] = LED_ENABLED;
        String dataStr;
        serializeJson(responseData, dataStr);
        
        server.send(200, "application/json", createJsonResponse(true, "Power state updated", dataStr));
    }

    void handleSetBrightness() {
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WEB "API call: /api/brightness");
        #endif
        
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
            return;
        }
        
        String body = server.arg("plain");
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, body);
        
        if (error) {
            server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
            return;
        }
        
        if (!doc.containsKey("brightness")) {
            server.send(400, "application/json", createJsonResponse(false, "Missing 'brightness' parameter"));
            return;
        }
        
        if (doc["brightness"].is<String>()) {
            String brightnessStr = doc["brightness"];
            if (brightnessStr.equalsIgnoreCase("auto")) {
                LED_AUTO_BRIGHTNESS = true;
                
                DynamicJsonDocument responseData(128);
                responseData["brightness"] = "auto";
                responseData["auto_brightness"] = LED_AUTO_BRIGHTNESS;
                String dataStr;
                serializeJson(responseData, dataStr);
                
                server.send(200, "application/json", createJsonResponse(true, "Auto brightness enabled", dataStr));
                return;
            } else {
                server.send(400, "application/json", createJsonResponse(false, "Invalid brightness value"));
                return;
            }
        }
        
        int brightness = doc["brightness"];
        if (brightness < 0 || brightness > 255) {
            server.send(400, "application/json", createJsonResponse(false, "Brightness must be between 0 and 255"));
            return;
        }
        
        LED_AUTO_BRIGHTNESS = false;  
        LED_BRIGHTNESS = brightness;
        if (LED_ENABLED) {
            leds.setBrightness(LED_BRIGHTNESS);
            leds.show();
        }
        
        DynamicJsonDocument responseData(128);
        responseData["brightness"] = LED_BRIGHTNESS;
        responseData["auto_brightness"] = LED_AUTO_BRIGHTNESS;
        String dataStr;
        serializeJson(responseData, dataStr);
        
        server.send(200, "application/json", createJsonResponse(true, "Brightness updated", dataStr));
    }

    void handleSetColor() {
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WEB "API call: /api/color");
        #endif
        
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
            return;
        }
        
        String body = server.arg("plain");
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, body);
        
        if (error) {
            server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
            return;
        }
        
        if (doc.containsKey("hex")) {
            String hexColor = doc["hex"];
            if (hexColor.length() == 7 && hexColor.charAt(0) == '#') {
                LED_R = strtol(hexColor.substring(1, 3).c_str(), NULL, 16);
                LED_G = strtol(hexColor.substring(3, 5).c_str(), NULL, 16);
                LED_B = strtol(hexColor.substring(5, 7).c_str(), NULL, 16);
            } else {
                server.send(400, "application/json", createJsonResponse(false, "Invalid hex"));
                return;
            }
        } else if (doc.containsKey("r") && doc.containsKey("g") && doc.containsKey("b")) {
            LED_R = doc["r"];
            LED_G = doc["g"];
            LED_B = doc["b"];
        } else {
            server.send(400, "application/json", createJsonResponse(false, "Missing color params"));
            return;
        }
        
        if (LED_ENABLED) {
            extern int lastHour, lastMinute;
            lastHour = -1;
            lastMinute = -1;
        }
        
        DynamicJsonDocument responseData(256);
        responseData["color"]["r"] = LED_R;
        responseData["color"]["g"] = LED_G;
        responseData["color"]["b"] = LED_B;
        char hexColor[8];
        sprintf(hexColor, "#%02X%02X%02X", LED_R, LED_G, LED_B);
        responseData["color"]["hex"] = hexColor;
        String dataStr;
        serializeJson(responseData, dataStr);
        
        server.send(200, "application/json", createJsonResponse(true, "Color updated", dataStr));
    }

    void handleCors() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(200, "text/plain", "OK");
    }

public:
    WebApi(NetworkManager& n, TimeManager& t, LedDriver& l, LightSensor& s)
        : server(WEB_SERVER_PORT), network(n), timeMgr(t), leds(l), sensor(s) {}

    void begin() {
        server.onNotFound([this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(404, "application/json", createJsonResponse(false, "Endpoint not found"));
        });
        
        server.on("/api/status", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleStatus();
        });
        
        server.on("/api/power", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handlePower();
        });
        
        server.on("/api/brightness", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleSetBrightness();
        });
        
        server.on("/api/color", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleSetColor();
        });
        
        server.on("/api/power", HTTP_OPTIONS, [this]() { this->handleCors(); });
        server.on("/api/brightness", HTTP_OPTIONS, [this]() { this->handleCors(); });
        server.on("/api/color", HTTP_OPTIONS, [this]() { this->handleCors(); });
        
        server.begin();
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WEB "API server started on port " + String(WEB_SERVER_PORT));
        #endif
    }

    void handleClient() {
        server.handleClient();
    }
};
