#include "config.h"
#include <Preferences.h>

Preferences preferences;

// ====== LED CONFIGURATION ======
int LED_R = 255;
int LED_G = 255;
int LED_B = 255;
int LED_BRIGHTNESS = 150;
bool LED_ENABLED = true;
bool LED_AUTO_BRIGHTNESS = false;

// ====== TIME CONFIGURATION ======
String TIMEZONE = "CET-1CEST,M3.5.0/2,M10.5.0/3";
String NTP_SERVER = "pool.ntp.org";

// ====== LIGHT SENSOR CONFIGURATION ======
int LIGHT_SENSOR_SDA = D4;
int LIGHT_SENSOR_SCL = D3;
int LIGHT_SENSOR_MAX_LUX = 200;
int LIGHT_SENSOR_MIN_LUX = 0;
int LIGHT_SENSOR_CALIBRATION_MAX = 10;
int LIGHT_SENSOR_MIN_BRIGHTNESS = 5;
int LIGHT_SENSOR_MAX_BRIGHTNESS = 255;

// ====== MAIN LOOP CONFIGURATION ======
const unsigned long UPDATE_INTERVAL = 100;
const bool AUTO_BRIGHTNESS_ENABLED = false;

// ====== GLOBAL STATE VARIABLES ======
int lastHour = -1;
int lastMinute = -1;

// ====== WEB SERVER CONFIGURATION ======
int WEB_SERVER_PORT = 80;

// ====== CONFIGURATION MANAGEMENT ======
void loadConfig() {
    preferences.begin("mirrorclock", false);
    
    LED_R = preferences.getInt("led_r", 255);
    LED_G = preferences.getInt("led_g", 255);
    LED_B = preferences.getInt("led_b", 255);
    LED_BRIGHTNESS = preferences.getInt("led_brt", 150);
    LED_ENABLED = preferences.getBool("led_en", true);
    LED_AUTO_BRIGHTNESS = preferences.getBool("led_auto", false);
    
    TIMEZONE = preferences.getString("tz", "CET-1CEST,M3.5.0/2,M10.5.0/3");
    NTP_SERVER = preferences.getString("ntp", "pool.ntp.org");
    
    LIGHT_SENSOR_SDA = preferences.getInt("ls_sda", D4);
    LIGHT_SENSOR_SCL = preferences.getInt("ls_scl", D3);
    LIGHT_SENSOR_MAX_LUX = preferences.getInt("ls_max_l", 200);
    LIGHT_SENSOR_MIN_LUX = preferences.getInt("ls_min_l", 0);
    LIGHT_SENSOR_CALIBRATION_MAX = preferences.getInt("ls_cal_m", 10);
    LIGHT_SENSOR_MIN_BRIGHTNESS = preferences.getInt("ls_min_b", 5);
    LIGHT_SENSOR_MAX_BRIGHTNESS = preferences.getInt("ls_max_b", 255);

    WEB_SERVER_PORT = preferences.getInt("web_port", 80);

    preferences.end();
}

void saveConfig() {
    preferences.begin("mirrorclock", false);
    
    preferences.putInt("led_r", LED_R);
    preferences.putInt("led_g", LED_G);
    preferences.putInt("led_b", LED_B);
    preferences.putInt("led_brt", LED_BRIGHTNESS);
    preferences.putBool("led_en", LED_ENABLED);
    preferences.putBool("led_auto", LED_AUTO_BRIGHTNESS);
    
    preferences.putString("tz", TIMEZONE);
    preferences.putString("ntp", NTP_SERVER);
    
    preferences.putInt("ls_sda", LIGHT_SENSOR_SDA);
    preferences.putInt("ls_scl", LIGHT_SENSOR_SCL);
    preferences.putInt("ls_max_l", LIGHT_SENSOR_MAX_LUX);
    preferences.putInt("ls_min_l", LIGHT_SENSOR_MIN_LUX);
    preferences.putInt("ls_cal_m", LIGHT_SENSOR_CALIBRATION_MAX);
    preferences.putInt("ls_min_b", LIGHT_SENSOR_MIN_BRIGHTNESS);
    preferences.putInt("ls_max_b", LIGHT_SENSOR_MAX_BRIGHTNESS);

    preferences.putInt("web_port", WEB_SERVER_PORT);

    preferences.end();
}
