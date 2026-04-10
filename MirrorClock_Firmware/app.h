#pragma once
#include "config.h"
#include "wifi_connection.h"
#include "time_sync.h"
#include "led_driver.h"
#include "light_sensor.h"
#include "core_api.h"
#include "web_api.h"
#include "mqtt_manager.h"

class App {
private:
    NetworkManager networkManager;
    TimeManager timeManager;
    LedDriver leds;
    LightSensor sensor;
    CoreApi coreApi;
    WebApi webApi;
    MqttManager mqttManager;

    unsigned long lastUpdateTime = 0;

public:
    App() : coreApi(networkManager, timeManager, leds, sensor),
            webApi(coreApi),
            mqttManager(coreApi) {}

    void setup() {
        #if DEBUG_ENABLED
            delay(3000);
            Serial.println("\n==================================================");
            Serial.println(DEBUG_PREFIX_SYSTEM "MirrorClock starting up...");
            Serial.println(DEBUG_PREFIX_SYSTEM "Firmware build: " + String(__DATE__) + " " + String(__TIME__));
            Serial.println(DEBUG_PREFIX_SYSTEM "Free heap: " + String(ESP.getFreeHeap()) + " bytes");
            Serial.println("==================================================");
        #endif

        networkManager.connect();
        timeManager.begin();
        leds.begin();
        sensor.begin();
        webApi.begin();
        mqttManager.begin();

        leds.lightLinePublic(IT);
        leds.lightLinePublic(IS);
        leds.show();
        
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_SYSTEM "Startup complete - entering main loop");
            Serial.println("==================================================\n");
        #endif
    }

    void loop() {
        webApi.handleClient();
        mqttManager.loop();

        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
            lastUpdateTime = currentTime;

            #if DEBUG_ENABLED
                static unsigned long lastStatusTime = 0;
                if (currentTime - lastStatusTime >= 300000) { 
                    lastStatusTime = currentTime;
                    Serial.println(DEBUG_PREFIX_SYSTEM "Status - Uptime: " + String(currentTime/1000) + "s | Free heap: " + String(ESP.getFreeHeap()) + " bytes");
                }
            #endif

            if (LED_AUTO_BRIGHTNESS) {
                LED_BRIGHTNESS = sensor.getBrightness();
                leds.setBrightness(LED_BRIGHTNESS);
                leds.show();
            }

            int hours = timeManager.getHour();
            int minutes = timeManager.getMinute();

            if (hours >= 12) hours -= 12;

            if (hours != lastHour || minutes != lastMinute) {
                leds.updateWatchface(hours, minutes);
                lastHour = hours;
                lastMinute = minutes;
            }
        }
    }
};
