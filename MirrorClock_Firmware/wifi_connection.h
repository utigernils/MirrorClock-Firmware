#pragma once
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "config.h"

class NetworkManager {
public:
    void connect() {
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WIFI "Connecting to WiFi or starting setup portal...");
        #endif
        
        WiFiManager wifiManager;
        
        if (!wifiManager.autoConnect("MirrorClock-Setup")) {
            #if DEBUG_ENABLED
                Serial.println(DEBUG_PREFIX_WIFI "Failed to connect and hit timeout");
            #endif
            ESP.restart();
            delay(1000);
        }
        
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_WIFI "Connected! IP: " + WiFi.localIP().toString() + 
                           " | RSSI: " + String(WiFi.RSSI()) + "dBm");
        #endif
    }
    
    bool isConnected() const { return WiFi.status() == WL_CONNECTED; }
    String getIP() const { return WiFi.localIP().toString(); }
    int getRSSI() const { return WiFi.RSSI(); }
    String getSSID() const { return WiFi.SSID(); }
    String getMacAddress() const { return WiFi.macAddress(); }
};
