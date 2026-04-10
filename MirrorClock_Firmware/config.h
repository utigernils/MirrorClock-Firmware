#pragma once

// ====== CONFIGURATION MANAGEMENT ======
void loadConfig();
void saveConfig();

// ====== LED CONFIGURATION ======
extern int LED_PIN;
extern int LED_COUNT;

extern int LED_R;
extern int LED_G;
extern int LED_B;
extern int LED_BRIGHTNESS;
extern bool LED_ENABLED;
extern bool LED_AUTO_BRIGHTNESS;

// ====== TRANSITION CONFIGURATION ======
extern int TRANSITION_EFFECT;
extern int TRANSITION_DURATION;

// ====== TIME CONFIGURATION ======
#include <Arduino.h>
extern String TIMEZONE;
extern String NTP_SERVER;

// ====== LIGHT SENSOR CONFIGURATION ======
extern int LIGHT_SENSOR_SDA;
extern int LIGHT_SENSOR_SCL;
extern int LIGHT_SENSOR_MAX_LUX;
extern int LIGHT_SENSOR_MIN_LUX;
extern int LIGHT_SENSOR_CALIBRATION_MAX;
extern int LIGHT_SENSOR_MIN_BRIGHTNESS;
extern int LIGHT_SENSOR_MAX_BRIGHTNESS;

// ====== MAIN LOOP CONFIGURATION ======
extern unsigned long UPDATE_INTERVAL;
extern bool AUTO_BRIGHTNESS_ENABLED;

// ====== GLOBAL STATE VARIABLES ======
extern int lastHour;
extern int lastMinute;

// ====== WEB SERVER CONFIGURATION ======
extern int WEB_SERVER_PORT;

// ====== MQTT CONFIGURATION ======
extern bool MQTT_ENABLED;
extern String MQTT_BROKER;
extern int MQTT_PORT;
extern String MQTT_USER;
extern String MQTT_PASSWORD;
extern String MQTT_TOPIC_PREFIX;

// ====== DEBUG CONFIGURATION ======
#define DEBUG_ENABLED true
#define DEBUG_PREFIX_WIFI    "[WIFI] "
#define DEBUG_PREFIX_LED     "[LED]  "
#define DEBUG_PREFIX_TIME    "[TIME] "
#define DEBUG_PREFIX_SENSOR  "[SENSOR] "
#define DEBUG_PREFIX_WEB     "[WEB]  "
#define DEBUG_PREFIX_SYSTEM  "[SYSTEM] "
