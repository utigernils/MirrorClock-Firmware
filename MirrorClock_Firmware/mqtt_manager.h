#pragma once
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "core_api.h"

class MqttManager {
private:
    WiFiClient espClient;
    PubSubClient client;
    CoreApi& api;
    unsigned long lastReconnectAttempt = 0;

    void publishStatus() {
        String response = api.getStatus();
        client.publish((MQTT_TOPIC_PREFIX + "/status").c_str(), api.createJsonResponse(true, "Status retrieved", response).c_str());
    }

    void publishState() {
        String response = api.getState();
        client.publish((MQTT_TOPIC_PREFIX + "/state").c_str(), api.createJsonResponse(true, "State retrieved", response).c_str(), true);
    }

    void handlePostState(String payload) {
        if (api.updateState(payload)) {
            publishState();
        }
    }

    void publishConfig() {
        String response = api.getConfig();
        client.publish((MQTT_TOPIC_PREFIX + "/config").c_str(), api.createJsonResponse(true, "Config retrieved", response).c_str());
    }

    void handlePostConfig(String payload) {
        if (api.updateConfig(payload)) {
            publishConfig();
        }
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
        else if (topicStr == MQTT_TOPIC_PREFIX + "/system/reboot") api.reboot();
    }

    void reconnect() {
        if (!MQTT_ENABLED || MQTT_BROKER == "" || !api.isNetworkConnected()) return;
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
    MqttManager(CoreApi& a) : api(a) {
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

        if (!client.connected()) {
            // Re-apply server settings just in case they were updated via API
            client.setServer(MQTT_BROKER.c_str(), MQTT_PORT);
            reconnect();
        } else {
            client.loop();
        }
    }
};