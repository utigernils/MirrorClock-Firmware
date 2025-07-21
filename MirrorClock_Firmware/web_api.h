#pragma once
#include <ESP8266WebServer.h>
#include "time_sync.h"
#include "led_driver.h"

extern int Brightness;
extern Adafruit_NeoPixel strip;

ESP8266WebServer server(80);

void handleStatus() {
  String json = "{";
  json += "\"hour\":" + String(getHour()) + ",";
  json += "\"minute\":" + String(getMinute()) + ",";
  json += "\"brightness\":" + String(Brightness);
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetBrightness() {
  if (server.hasArg("value")) {
    Brightness = server.arg("value").toInt();
    strip.setBrightness(Brightness);
    strip.show();
    server.send(200, "text/plain", "Brightness set to " + String(Brightness));
  } else {
    server.send(400, "text/plain", "Missing parameter: value");
  }
}

void setupWebAPI() {
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/brightness", HTTP_POST, handleSetBrightness);
  server.begin();
}

void handleWebRequests() {
  server.handleClient();
}
