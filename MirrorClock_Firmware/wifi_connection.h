#pragma once
#include <ESP8266WiFi.h>

const char *ssid = "...";
const char *password = "...";

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
}
