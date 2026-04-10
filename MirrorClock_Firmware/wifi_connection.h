#pragma once
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "config.h"

void connectToWiFi() {
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WIFI "Connecting to WiFi or starting setup portal...");
  #endif
  
  WiFiManager wifiManager;
  
  if (!wifiManager.autoConnect("MirrorClock-Setup")) {
    #if DEBUG_ENABLED
      Serial.println(DEBUG_PREFIX_WIFI "Failed to connect and hit timeout");
    #endif
    // Reset and try again
    ESP.restart();
    delay(1000);
  }
  
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WIFI "Connected! IP: " + WiFi.localIP().toString() + 
                   " | RSSI: " + String(WiFi.RSSI()) + "dBm");
  #endif
}
