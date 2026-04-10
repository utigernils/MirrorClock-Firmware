#pragma once
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "wifi_connection.h"
#include "time_sync.h"
#include "led_driver.h"
#include "light_sensor.h"

class MqttManager {
private:
    WiFiClient espClient;
    PubSubClient client;
    NetworkManager& network;
    TimeManager& timeMgr;
    LedDriver& leds;
    LightSensor& sensor;
    unsigned long lastReconnectAttempt = 0;

    String createJsonResponse(bool success, String message = "", String data = "") {
        DynamicJsonDocument doc(2048);
        doc["success"] = success;
        doc["timestamp"] = timeMgr.getEpochTime();
        if (message != "") doc["message"] = message;
        if (data != "") {
            DynamicJsonDocument dataDoc(1024);
            deserializeJson(dataDoc, data);
            doc["data"] = dataDoc;
        }
        String response;
        serializeJson(doc, response);
        return response;
    }

    void publishStatus() {
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
        client.publish((MQTT_TOPIC_PREFIX + "/status").c_str(), createJsonResponse(true, "Status retrieved", response).c_str());
    }

    void publishState() {
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
        client.publish((MQTT_TOPIC_PREFIX + "/state").c_str(), createJsonResponse(true, "State retrieved", response).c_str(), true);
    }

    void handlePostState(String payload) {
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, payload);
        if (error) return;
        
        bool changed = false;

        if (doc.containsKey("enabled")) {
            LED_ENABLED = doc["enabled"];
            if (!LED_ENABLED) { leds.clear(); leds.show(); }
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
            lastHour = -1; lastMinute = -1;
            if (LED_ENABLED && !LED_AUTO_BRIGHTNESS) leds.setBrightness(LED_BRIGHTNESS);
            saveConfig();
            publishState();
        }
    }

    void publishConfig() {
        DynamicJsonDocument doc(2048);
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
        doc["mqtt_enabled"] = MQTT_ENABLED;
        doc["mqtt_broker"] = MQTT_BROKER;
        doc["mqtt_port"] = MQTT_PORT;
        doc["mqtt_user"] = MQTT_USER;
        doc["mqtt_topic_prefix"] = MQTT_TOPIC_PREFIX;
        
        String response;
        serializeJson(doc, response);
        client.publish((MQTT_TOPIC_PREFIX + "/config").c_str(), createJsonResponse(true, "Config retrieved", response).c_str());
    }

    void handlePostConfig(String payload) {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);
        if (error) return;
        
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
        publishConfig();
    }

    void handleCallback(char* topic, byte* payload, unsigned int length) {
        String topicStr = String(topic);
        String payloadStr = "";
        for (unsigned int i = 0; i < length; i++) {
            payloadStr += (char)payload[i];
        }

        if (topicStr == MQTT_TOPIC_PREFIX + "/status/request") publishStatus();
        else if (topicStr == MQTT_TOPIC_PREFIX + "/state/request") publishState();
        else if (topicStr == MQTT_TOPIC_PREFIX + "/state/set") handlePostState(payloadStr);
        else if (topicStr == MQTT_TOPIC_PREFIX + "/config/request") publishConfig();
        else if (topicStr == MQTT_TOPIC_PREFIX + "/config/set") handlePostConfig(payloadStr);
        else if (topicStr == MQTT_TOPIC_PREFIX + "/system/reboot") ESP.restart();
    }

    void reconnect() {
        if (!MQTT_ENABLED || MQTT_BROKER == "" || !network.isConnected()) return;
        if (client.connected()) return;

        if (millis() - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = millis();
            String clientId = "MirrorClock-" + String(ESP.getChipId(), HEX);
            
            bool connected = false;
            if (MQTT_USER.length() > 0) {
                connected = client.connect(clientId.c_str(), MQTT_USER.c_str(), MQTT_PASSWORD.c_str());
            } else {
                connected = client.connect(clientId.c_str());
            }

            if (connected) {
                client.subscribe((MQTT_TOPIC_PREFIX + "/status/request").c_str());
                client.subscribe((MQTT_TOPIC_PREFIX + "/state/request").c_str());
                client.subscribe((MQTT_TOPIC_PREFIX + "/state/set").c_str());
                client.subscribe((MQTT_TOPIC_PREFIX + "/config/request").c_str());
                client.subscribe((MQTT_TOPIC_PREFIX + "/config/set").c_str());
                client.subscribe((MQTT_TOPIC_PREFIX + "/system/reboot").c_str());
                lastReconnectAttempt = 0;
                publishState(); // Broadcast state on connect
            }
        }
    }

public:
    MqttManager(NetworkManager& n, TimeManager& t, LedDriver& l, LightSensor& s)
        : network(n), timeMgr(t), leds(l), sensor(s) {
        client.setClient(espClient);
    }

    void begin() {
        if (!MQTT_ENABLED) return;
        client.setServer(MQTT_BROKER.c_str(), MQTT_PORT);
        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            this->handleCallback(topic, payload, length);
        });
    }

    void loop() {
        if (!MQTT_ENABLED) return;
        
        // If broker changed during runtime
        if(client.connected() == false && (strcmp(client.getServerDomain(), MQTT_BROKER.c_str()) != 0 || client.getServerPort() != MQTT_PORT)){
             client.setServer(MQTT_BROKER.c_str(), MQTT_PORT);
        }

        if (!client.connected()) {
            reconnect();
        } else {
            client.loop();
        }
    }
};