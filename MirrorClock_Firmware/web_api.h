#pragma once
#include <ESP8266WebServer.h>
#include "time_sync.h"
#include "led_driver.h"
#include "light_sensor.h"
#include "config.h"

ESP8266WebServer server(WEB_SERVER_PORT);

void handleStatus() {
  String json = "{";
  json += "\"hour\":" + String(getHour()) + ",";
  json += "\"minute\":" + String(getMinute()) + ",";
  json += "\"brightness\":" + String(LED_BRIGHTNESS) + ",";
  json += "\"BH1750\":{";
  json += "\"lux\":" + String(getLightLevel());
  json += "}";
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetBrightness() {
  if (server.hasArg("value")) {
    LED_BRIGHTNESS = server.arg("value").toInt();
    strip.setBrightness(LED_BRIGHTNESS);
    strip.show();
    server.send(200, "text/plain", "Brightness set to " + String(LED_BRIGHTNESS));
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
