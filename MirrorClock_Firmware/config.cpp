#include "config.h"
#include <Adafruit_NeoPixel.h>

// ====== WIFI CONFIGURATION ======
const char* WIFI_SSID = "...";
const char* WIFI_PASSWORD = "...";

// ====== LED CONFIGURATION ======
int LED_R = 255;
int LED_G = 255;
int LED_B = 255;
int LED_BRIGHTNESS = 150;
bool LED_ENABLED = true;
bool LED_AUTO_BRIGHTNESS = false;

// ====== TIME CONFIGURATION ======
const char* TIMEZONE = "CET-1CEST,M3.5.0/2,M10.5.0/3";
const char* NTP_SERVER = "pool.ntp.org";

// ====== MAIN LOOP CONFIGURATION ======
const unsigned long UPDATE_INTERVAL = 100;
const bool AUTO_BRIGHTNESS_ENABLED = false;

// ====== DEBUG CONFIGURATION ======
// Note: DEBUG_ENABLED and DEBUG_PREFIX_* constants are defined in config.h

// ====== GLOBAL STATE VARIABLES ======
int lastHour = -1;
int lastMinute = -1;

// ====== GLOBAL OBJECTS ======
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
