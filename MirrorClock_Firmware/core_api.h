#pragma once
#include <ArduinoJson.h>
#include "config.h"
#include "wifi_connection.h"
#include "time_sync.h"
#include "led_driver.h"
#include "light_sensor.h"

class CoreApi {
private:
    NetworkManager& network;
    TimeManager& timeMgr;
    LedDriver& leds;
    LightSensor& sensor;

public:
    CoreApi(NetworkManager& n, TimeManager& t, LedDriver& l, LightSensor& s)
        : network(n), timeMgr(t), leds(l), sensor(s) {}

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

    String getStatus() {
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
        return response;
    }

    String getState() {
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
        return response;
    }

    bool updateState(const String& payload) {
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, payload);
        
        if (error) return false;
        
        bool changed = false;

        if (doc.containsKey("enabled")) {
            LED_ENABLED = doc["enabled"];
            if (!LED_ENABLED) {
                leds.clear();
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
        
        return true;
    }

    String getConfig() {
        DynamicJsonDocument doc(2048);
        
        doc["led_pin"] = LED_PIN;
        doc["led_count"] = LED_COUNT;
        
        doc["transition_effect"] = TRANSITION_EFFECT;
        doc["transition_duration"] = TRANSITION_DURATION;
        
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
        
        doc["mqtt_enabled"] = MQTT_ENABLED;
        doc["mqtt_broker"] = MQTT_BROKER;
        doc["mqtt_port"] = MQTT_PORT;
        doc["mqtt_user"] = MQTT_USER;
        doc["mqtt_password"] = ""; // Don't send out password
        doc["mqtt_topic_prefix"] = MQTT_TOPIC_PREFIX;
        
        String response;
        serializeJson(doc, response);
        return response;
    }

    bool updateConfig(const String& payload) {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);
        
        if (error) return false;
        
        if (doc.containsKey("led_pin")) LED_PIN = doc["led_pin"];
        if (doc.containsKey("led_count")) LED_COUNT = doc["led_count"];
        
        if (doc.containsKey("transition_effect")) TRANSITION_EFFECT = doc["transition_effect"];
        if (doc.containsKey("transition_duration")) TRANSITION_DURATION = doc["transition_duration"];
        
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
        
        if (doc.containsKey("mqtt_enabled")) MQTT_ENABLED = doc["mqtt_enabled"];
        if (doc.containsKey("mqtt_broker")) MQTT_BROKER = String((const char*)doc["mqtt_broker"]);
        if (doc.containsKey("mqtt_port")) MQTT_PORT = doc["mqtt_port"];
        if (doc.containsKey("mqtt_user")) MQTT_USER = String((const char*)doc["mqtt_user"]);
        if (doc.containsKey("mqtt_password")) {
            String pw = String((const char*)doc["mqtt_password"]);
            if(pw.length() > 0) MQTT_PASSWORD = pw;
        }
        if (doc.containsKey("mqtt_topic_prefix")) MQTT_TOPIC_PREFIX = String((const char*)doc["mqtt_topic_prefix"]);
        
        saveConfig();
        return true;
    }

    void reboot() {
        delay(500);
        ESP.restart();
    }
    
    // Pass-through helpers for managers
    bool isNetworkConnected() { return network.isConnected(); }
};