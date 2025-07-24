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

void handleControl() {
  server.send(200, "text/html",
    "<!DOCTYPE html>"
    "<html lang='de'>"
    "<head>"
    "<meta charset='UTF-8' />"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0' />"
    "<title>LED Steuerung</title>"
    "<style>"
    "body { font-family: sans-serif; background: #f2f2f2; padding: 1rem; max-width: 400px; margin: auto; }"
    "h1 { font-size: 1.2rem; }"
    "label { display: block; margin-top: 1rem; }"
    "input[type='range'] { width: 100%; }"
    "input, button, select { font-size: 1rem; padding: 0.4rem; width: 100%; box-sizing: border-box; margin-top: 0.3rem; }"
    "button { background: #007bff; color: white; border: none; cursor: pointer; }"
    "button:hover { background: #0056b3; }"
    "pre { background: #eee; padding: 0.5rem; overflow: auto; font-size: 0.8rem; }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>LED Steuerung</h1>"

    "<label>"
    "LED Status:"
    "<select id='power'>"
    "<option value='true'>An</option>"
    "<option value='false'>Aus</option>"
    "</select>"
    "</label>"

    "<label>"
    "Helligkeit:"
    "<input type='range' id='brightness' min='0' max='255' />"
    "<button onclick='setAutoBrightness()'>Auto</button>"
    "</label>"

    "<label>"
    "Farbe:"
    "<input type='color' id='color' value='#ffffff' />"
    "</label>"

    "<button onclick='applySettings()'>Übernehmen</button>"

    "<h2>Status</h2>"
    "<button onclick='loadStatus()'>Aktualisieren</button>"
    "<pre id='status'>...</pre>"

    "<script>"
    "function applySettings() {"
    "  const power = document.getElementById('power').value === 'true';"
    "  const brightness = parseInt(document.getElementById('brightness').value);"
    "  const hex = document.getElementById('color').value;"

    "  fetch('/api/power', {"
    "    method: 'POST',"
    "    headers: { 'Content-Type': 'application/json' },"
    "    body: JSON.stringify({ enabled: power })"
    "  });"

    "  fetch('/api/brightness', {"
    "    method: 'POST',"
    "    headers: { 'Content-Type': 'application/json' },"
    "    body: JSON.stringify({ brightness: brightness })"
    "  });"

    "  fetch('/api/color', {"
    "    method: 'POST',"
    "    headers: { 'Content-Type': 'application/json' },"
    "    body: JSON.stringify({ hex: hex })"
    "  });"

    "  setTimeout(loadStatus, 500);"
    "}"

    "function setAutoBrightness() {"
    "  fetch('/api/brightness', {"
    "    method: 'POST',"
    "    headers: { 'Content-Type': 'application/json' },"
    "    body: JSON.stringify({ brightness: 'auto' })"
    "  });"
    "  setTimeout(loadStatus, 500);"
    "}"

    "function loadStatus() {"
    "  fetch('/api/status')"
    "    .then(res => res.json())"
    "    .then(data => {"
    "      document.getElementById('status').textContent = JSON.stringify(data, null, 2);"
    "    });"
    "}"

    "loadStatus();"
    "</script>"
    "</body>"
    "</html>"
  );
}


void handlePower() {
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WEB "API call: /api/power");
  #endif
  
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
  
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_LED "Power state changed: " + String(LED_ENABLED ? "ON" : "OFF"));
  #endif
  
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
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WEB "API call: /api/brightness");
  #endif
  
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
  
  int brightness = doc["brightness"];
  
  if (brightness < 0 || brightness > 255) {
    server.send(400, "application/json", createJsonResponse(false, "Brightness must be between 0 and 255"));
    return;
  }
  
  LED_AUTO_BRIGHTNESS = false;  
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
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WEB "API call: /api/color");
  #endif
  
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

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    handleControl();
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
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_WEB "API server started on port " + String(WEB_SERVER_PORT));
  #endif
}

void handleWebRequests() {
  server.handleClient();
}
