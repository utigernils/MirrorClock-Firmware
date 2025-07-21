/*
 * MirrorClock Web API
 * 
 * Available Endpoints:
 * 
 * GET  /api/status     - Get comprehensive system status (JSON)
 * POST /api/power      - Control LED on/off state (JSON body: {"enabled": true/false})
 * POST /api/brightness - Set LED brightness (JSON body: {"brightness": 0-255} OR {"brightness": "auto"})
 * POST /api/color      - Set LED color (JSON body: {"r":255,"g":255,"b":255} OR {"hex":"#FFFFFF"})
 * 
 * All POST endpoints require JSON body and return JSON responses
 * All responses include success status, timestamp, message, and data fields
 */

#pragma once
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "time_sync.h"
#include "led_driver.h"
#include "light_sensor.h"
#include "config.h"
#include <ESP8266WiFi.h>

ESP8266WebServer server(WEB_SERVER_PORT);

String createJsonResponse(bool success, String message = "", String data = "") {
  DynamicJsonDocument doc(1024);
  doc["success"] = success;
  doc["timestamp"] = getEpochTime();
  
  if (message != "") {
    doc["message"] = message;
  }
  
  if (data != "") {
    DynamicJsonDocument dataDoc(512);
    deserializeJson(dataDoc, data);
    doc["data"] = dataDoc;
  }
  
  String response;
  serializeJson(doc, response);
  return response;
}

void handleStatus() {
  DynamicJsonDocument doc(2048);
  
  doc["system"]["uptime"] = millis();
  doc["system"]["free_heap"] = ESP.getFreeHeap();
  doc["system"]["chip_id"] = ESP.getChipId();
  doc["system"]["flash_size"] = ESP.getFlashChipSize();
  doc["system"]["cpu_freq"] = ESP.getCpuFreqMHz();
  
  doc["wifi"]["connected"] = WiFi.status() == WL_CONNECTED;
  doc["wifi"]["ssid"] = WiFi.SSID();
  doc["wifi"]["ip"] = WiFi.localIP().toString();
  doc["wifi"]["rssi"] = WiFi.RSSI();
  doc["wifi"]["mac"] = WiFi.macAddress();
  
  doc["time"]["hour"] = getHour();
  doc["time"]["minute"] = getMinute();
  doc["time"]["epoch"] = getEpochTime();
  doc["time"]["ntp_server"] = NTP_SERVER;
  doc["time"]["timezone"] = TIMEZONE;
  
  doc["led"]["enabled"] = LED_ENABLED;
  doc["led"]["brightness"] = LED_BRIGHTNESS;
  doc["led"]["auto_brightness"] = LED_AUTO_BRIGHTNESS;
  doc["led"]["color"]["r"] = LED_R;
  doc["led"]["color"]["g"] = LED_G;
  doc["led"]["color"]["b"] = LED_B;
  doc["led"]["count"] = LED_COUNT;
  doc["led"]["pin"] = LED_PIN;
  
  doc["light_sensor"]["lux"] = getLightLevel();
  doc["light_sensor"]["auto_brightness"] = AUTO_BRIGHTNESS_ENABLED;
  doc["light_sensor"]["calculated_brightness"] = getBrightness();
  
  doc["config"]["update_interval"] = UPDATE_INTERVAL;
  doc["config"]["web_port"] = WEB_SERVER_PORT;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handlePower() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
    return;
  }
  
  String body = server.arg("plain");
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
    return;
  }
  
  if (!doc.containsKey("enabled")) {
    server.send(400, "application/json", createJsonResponse(false, "Missing 'enabled' parameter"));
    return;
  }
  
  LED_ENABLED = doc["enabled"];
  
  if (!LED_ENABLED) {
    strip.clear();
    strip.show();
  } else {
    extern int lastHour, lastMinute;
    lastHour = -1;
    lastMinute = -1;
  }
  
  DynamicJsonDocument responseData(128);
  responseData["enabled"] = LED_ENABLED;
  String dataStr;
  serializeJson(responseData, dataStr);
  
  server.send(200, "application/json", createJsonResponse(true, "Power state updated", dataStr));
}

void handleSetBrightness() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
    return;
  }
  
  String body = server.arg("plain");
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
    return;
  }
  
  if (!doc.containsKey("brightness")) {
    server.send(400, "application/json", createJsonResponse(false, "Missing 'brightness' parameter"));
    return;
  }
  
  // Check if brightness is "auto" string or numeric value
  if (doc["brightness"].is<String>()) {
    String brightnessStr = doc["brightness"];
    if (brightnessStr.equalsIgnoreCase("auto")) {
      LED_AUTO_BRIGHTNESS = true;
      
      DynamicJsonDocument responseData(128);
      responseData["brightness"] = "auto";
      responseData["auto_brightness"] = LED_AUTO_BRIGHTNESS;
      String dataStr;
      serializeJson(responseData, dataStr);
      
      server.send(200, "application/json", createJsonResponse(true, "Auto brightness enabled", dataStr));
      return;
    } else {
      server.send(400, "application/json", createJsonResponse(false, "Invalid brightness value. Use numeric value (0-255) or 'auto'"));
      return;
    }
  }
  
  // Handle numeric brightness value
  int brightness = doc["brightness"];
  
  if (brightness < 0 || brightness > 255) {
    server.send(400, "application/json", createJsonResponse(false, "Brightness must be between 0 and 255"));
    return;
  }
  
  LED_AUTO_BRIGHTNESS = false;  // Disable auto brightness when setting manual value
  LED_BRIGHTNESS = brightness;
  if (LED_ENABLED) {
    strip.setBrightness(LED_BRIGHTNESS);
    strip.show();
  }
  
  DynamicJsonDocument responseData(128);
  responseData["brightness"] = LED_BRIGHTNESS;
  responseData["auto_brightness"] = LED_AUTO_BRIGHTNESS;
  String dataStr;
  serializeJson(responseData, dataStr);
  
  server.send(200, "application/json", createJsonResponse(true, "Brightness updated", dataStr));
}

void handleSetColor() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", createJsonResponse(false, "No JSON body provided"));
    return;
  }
  
  String body = server.arg("plain");
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", createJsonResponse(false, "Invalid JSON"));
    return;
  }
  
  if (doc.containsKey("hex")) {
    String hexColor = doc["hex"];
    if (hexColor.length() == 7 && hexColor.charAt(0) == '#') {
      LED_R = strtol(hexColor.substring(1, 3).c_str(), NULL, 16);
      LED_G = strtol(hexColor.substring(3, 5).c_str(), NULL, 16);
      LED_B = strtol(hexColor.substring(5, 7).c_str(), NULL, 16);
    } else {
      server.send(400, "application/json", createJsonResponse(false, "Invalid hex color format. Use #RRGGBB"));
      return;
    }
  } else if (doc.containsKey("r") && doc.containsKey("g") && doc.containsKey("b")) {
    LED_R = doc["r"];
    LED_G = doc["g"];
    LED_B = doc["b"];
    
    if (LED_R < 0 || LED_R > 255 || LED_G < 0 || LED_G > 255 || LED_B < 0 || LED_B > 255) {
      server.send(400, "application/json", createJsonResponse(false, "RGB values must be between 0 and 255"));
      return;
    }
  } else {
    server.send(400, "application/json", createJsonResponse(false, "Missing color parameters. Provide either 'hex' or 'r','g','b'"));
    return;
  }
  
  if (LED_ENABLED) {
    extern int lastHour, lastMinute;
    lastHour = -1;
    lastMinute = -1;
  }
  
  DynamicJsonDocument responseData(256);
  responseData["color"]["r"] = LED_R;
  responseData["color"]["g"] = LED_G;
  responseData["color"]["b"] = LED_B;
  
  char hexColor[8];
  sprintf(hexColor, "#%02X%02X%02X", LED_R, LED_G, LED_B);
  responseData["color"]["hex"] = hexColor;
  
  String dataStr;
  serializeJson(responseData, dataStr);
  
  server.send(200, "application/json", createJsonResponse(true, "Color updated", dataStr));
}

void handleCors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "text/plain", "OK");
}

void setupWebAPI() {
  server.onNotFound([]() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(404, "application/json", createJsonResponse(false, "Endpoint not found"));
  });
  
  server.on("/api/status", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    handleStatus();
  });
  
  server.on("/api/power", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    handlePower();
  });
  
  server.on("/api/brightness", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    handleSetBrightness();
  });
  
  server.on("/api/color", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    handleSetColor();
  });
  
  server.on("/api/power", HTTP_OPTIONS, handleCors);
  server.on("/api/brightness", HTTP_OPTIONS, handleCors);
  server.on("/api/color", HTTP_OPTIONS, handleCors);
  
  server.begin();
  Serial.println("Web API started on port " + String(WEB_SERVER_PORT));
}

void handleWebRequests() {
  server.handleClient();
}
