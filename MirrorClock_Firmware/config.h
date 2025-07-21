#pragma once

// ====== WIFI CONFIGURATION ======
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// ====== LED CONFIGURATION ======
#define LED_PIN 3
#define LED_COUNT 114

extern int LED_R;
extern int LED_G;
extern int LED_B;
extern int LED_BRIGHTNESS;
extern bool LED_ENABLED;
extern bool LED_AUTO_BRIGHTNESS;

// ====== TIME CONFIGURATION ======
extern const char* TIMEZONE;
extern const char* NTP_SERVER;

// ====== LIGHT SENSOR CONFIGURATION ======
#define LIGHT_SENSOR_SDA D4
#define LIGHT_SENSOR_SCL D3
#define LIGHT_SENSOR_MAX_LUX 200
#define LIGHT_SENSOR_MIN_LUX 0
#define LIGHT_SENSOR_CALIBRATION_MAX 10
#define LIGHT_SENSOR_MIN_BRIGHTNESS 5
#define LIGHT_SENSOR_MAX_BRIGHTNESS 255

// ====== MAIN LOOP CONFIGURATION ======
extern const unsigned long UPDATE_INTERVAL;
extern const bool AUTO_BRIGHTNESS_ENABLED;

// ====== GLOBAL STATE VARIABLES ======
extern int lastHour;
extern int lastMinute;

// ====== WEB SERVER CONFIGURATION ======
#define WEB_SERVER_PORT 80

// ====== DEBUG CONFIGURATION ======
#define DEBUG_ENABLED true
#define DEBUG_PREFIX_WIFI    "[WIFI] "
#define DEBUG_PREFIX_LED     "[LED]  "
#define DEBUG_PREFIX_TIME    "[TIME] "
#define DEBUG_PREFIX_SENSOR  "[SENSOR] "
#define DEBUG_PREFIX_WEB     "[WEB]  "
#define DEBUG_PREFIX_SYSTEM  "[SYSTEM] "

// ====== GLOBAL OBJECTS ======
extern class Adafruit_NeoPixel strip;
