#pragma once
#include <ESP8266WiFi.h>
#include "config.h"

void connectToWiFi() {
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WIFI "Connecting to '" + String(WIFI_SSID) + "'...");
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    attempts++;
    #if DEBUG_ENABLED
      if (attempts % 10 == 0) {
        Serial.println(DEBUG_PREFIX_WIFI "Still connecting... (" + String(attempts * 0.5) + "s)");
      }
    #endif
  }
  
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WIFI "Connected! IP: " + WiFi.localIP().toString() + 
                   " | RSSI: " + String(WiFi.RSSI()) + "dBm");
  #endif
}
