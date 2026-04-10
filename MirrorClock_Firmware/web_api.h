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
        DynamicJsonDocument doc(2048);
        doc["success"] = success;
        doc["timestamp"] = timeMgr.getEpochTime();
        
        if (message != "") {
            doc["message"] = message;
        }
        
        if (data != "") {
            DynamicJsonDocument dataDoc(1024);
            deserializeJson(dataDoc, data);
            doc["data"] = dataDoc;
        }
        
        String response;
        serializeJson(doc, response);
        return response;
    }

    void handleGetStatus() {
        DynamicJsonDocument doc(1024);
        
        doc["system"]["uptime"] = millis();
        doc["system"]["free_heap"] = ESP.getFreeHeap();
        doc["system"]["chip_id"] = ESP.getChipId();
        doc["system"]["cpu_freq"] = ESP.getCpuFreqMHz();
        
        doc["wifi"]["connected"] = network.isConnected();
        doc["wifi"]["ssid"] = network.getSSID();
        doc["wifi"]["ip"] = network.getIP();
        doc["wifi"]["rssi"] = network.getRSSI();
        
        doc["time"]["epoch"] = timeMgr.getEpochTime();
        doc["time"]["formatted"] = String(timeMgr.getHour()) + ":" + String(timeMgr.getMinute());
        
        doc["sensor"]["lux"] = sensor.getLightLevel();
        doc["sensor"]["calculated_brightness"] = sensor.getBrightness();
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", createJsonResponse(true, "Status retrieved", response));
    }

    void handleGetState() {
        DynamicJsonDocument doc(512);
        
        doc["enabled"] = LED_ENABLED;
        doc["brightness"] = LED_BRIGHTNESS;
        doc["auto_brightness"] = LED_AUTO_BRIGHTNESS;
        doc["color"]["r"] = LED_R;
        doc["color"]["g"] = LED_G;
        doc["color"]["b"] = LED_B;
        char hexColor[8];
        sprintf(hexColor, "#%02X%02X%02X", LED_R, LED_G, LED_B);
        doc["color"]["hex"] = String(hexColor);
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", createJsonResponse(true, "State retrieved", response));
    }

    void handlePostState() {
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
            return;
        }
        
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, server.arg("plain"));
        
        if (error) {
            server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
            return;
        }
        
        bool changed = false;

        if (doc.containsKey("enabled")) {
            LED_ENABLED = doc["enabled"];
            if (!LED_ENABLED) {
                leds.clear();
                leds.show();
            }
            changed = true;
        }
        
        if (doc.containsKey("brightness")) {
            if (doc["brightness"].is<String>() && String((const char*)doc["brightness"]).equalsIgnoreCase("auto")) {
                LED_AUTO_BRIGHTNESS = true;
            } else {
                LED_AUTO_BRIGHTNESS = false;
                LED_BRIGHTNESS = doc["brightness"];
            }
            changed = true;
        }
        
        if (doc.containsKey("color")) {
            JsonObject color = doc["color"];
            if (color.containsKey("hex")) {
                String hexColor = color["hex"];
                if (hexColor.length() == 7 && hexColor.charAt(0) == '#') {
                    LED_R = strtol(hexColor.substring(1, 3).c_str(), NULL, 16);
                    LED_G = strtol(hexColor.substring(3, 5).c_str(), NULL, 16);
                    LED_B = strtol(hexColor.substring(5, 7).c_str(), NULL, 16);
                    changed = true;
                }
            } else if (color.containsKey("r") && color.containsKey("g") && color.containsKey("b")) {
                LED_R = color["r"];
                LED_G = color["g"];
                LED_B = color["b"];
                changed = true;
            }
        }
        
        if (changed) {
            extern int lastHour, lastMinute;
            lastHour = -1;
            lastMinute = -1;
            if (LED_ENABLED && !LED_AUTO_BRIGHTNESS) {
                leds.setBrightness(LED_BRIGHTNESS);
            }
            saveConfig();
        }
        
        handleGetState();
    }

    void handleGetConfig() {
        DynamicJsonDocument doc(1024);
        
        doc["led_pin"] = LED_PIN;
        doc["led_count"] = LED_COUNT;
        
        doc["timezone"] = TIMEZONE;
        doc["ntp_server"] = NTP_SERVER;
        
        doc["sensor_sda"] = LIGHT_SENSOR_SDA;
        doc["sensor_scl"] = LIGHT_SENSOR_SCL;
        doc["sensor_max_lux"] = LIGHT_SENSOR_MAX_LUX;
        doc["sensor_min_lux"] = LIGHT_SENSOR_MIN_LUX;
        doc["sensor_cal_max"] = LIGHT_SENSOR_CALIBRATION_MAX;
        doc["sensor_min_brightness"] = LIGHT_SENSOR_MIN_BRIGHTNESS;
        doc["sensor_max_brightness"] = LIGHT_SENSOR_MAX_BRIGHTNESS;
        
        doc["update_interval"] = UPDATE_INTERVAL;
        doc["auto_brightness_enabled"] = AUTO_BRIGHTNESS_ENABLED;
        doc["web_server_port"] = WEB_SERVER_PORT;
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", createJsonResponse(true, "Config retrieved", response));
    }

    void handlePostConfig() {
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
            return;
        }
        
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, server.arg("plain"));
        
        if (error) {
            server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
            return;
        }
        
        if (doc.containsKey("led_pin")) LED_PIN = doc["led_pin"];
        if (doc.containsKey("led_count")) LED_COUNT = doc["led_count"];
        
        if (doc.containsKey("timezone")) TIMEZONE = String((const char*)doc["timezone"]);
        if (doc.containsKey("ntp_server")) NTP_SERVER = String((const char*)doc["ntp_server"]);
        
        if (doc.containsKey("sensor_sda")) LIGHT_SENSOR_SDA = doc["sensor_sda"];
        if (doc.containsKey("sensor_scl")) LIGHT_SENSOR_SCL = doc["sensor_scl"];
        if (doc.containsKey("sensor_max_lux")) LIGHT_SENSOR_MAX_LUX = doc["sensor_max_lux"];
        if (doc.containsKey("sensor_min_lux")) LIGHT_SENSOR_MIN_LUX = doc["sensor_min_lux"];
        if (doc.containsKey("sensor_cal_max")) LIGHT_SENSOR_CALIBRATION_MAX = doc["sensor_cal_max"];
        if (doc.containsKey("sensor_min_brightness")) LIGHT_SENSOR_MIN_BRIGHTNESS = doc["sensor_min_brightness"];
        if (doc.containsKey("sensor_max_brightness")) LIGHT_SENSOR_MAX_BRIGHTNESS = doc["sensor_max_brightness"];
        
        if (doc.containsKey("update_interval")) UPDATE_INTERVAL = doc["update_interval"];
        if (doc.containsKey("auto_brightness_enabled")) AUTO_BRIGHTNESS_ENABLED = doc["auto_brightness_enabled"];
        if (doc.containsKey("web_server_port")) WEB_SERVER_PORT = doc["web_server_port"];
        
        saveConfig();
        
        server.send(200, "application/json", createJsonResponse(true, "Config updated. Please restart device for some changes to take effect."));
    }

    void handleSystemReboot() {
        server.send(200, "application/json", createJsonResponse(true, "Rebooting..."));
        delay(500);
        ESP.restart();
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
            if (server.method() == HTTP_OPTIONS) {
                this->handleCors();
            } else {
                server.send(404, "application/json", createJsonResponse(false, "Endpoint not found"));
            }
        });
        
        server.on("/api/status", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleGetStatus();
        });
        
        server.on("/api/state", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleGetState();
        });
        
        server.on("/api/state", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handlePostState();
        });

        server.on("/api/config", HTTP_GET, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleGetConfig();
        });
        
        server.on("/api/config", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handlePostConfig();
        });

        server.on("/api/system/reboot", HTTP_POST, [this]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            this->handleSystemReboot();
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
