#include "config.h"
#include <Preferences.h>

Preferences preferences;

// ====== LED CONFIGURATION ======
int LED_PIN = 3;
int LED_COUNT = 114;
int LED_R = 255;
int LED_G = 255;
int LED_B = 255;
int LED_BRIGHTNESS = 150;
bool LED_ENABLED = true;
bool LED_AUTO_BRIGHTNESS = false;

// ====== TRANSITION CONFIGURATION ======
int TRANSITION_EFFECT = 0;
int TRANSITION_DURATION = 1000;

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
unsigned long UPDATE_INTERVAL = 100;
bool AUTO_BRIGHTNESS_ENABLED = false;

// ====== GLOBAL STATE VARIABLES ======
int lastHour = -1;
int lastMinute = -1;

// ====== WEB SERVER CONFIGURATION ======
int WEB_SERVER_PORT = 80;

// ====== MQTT CONFIGURATION ======
bool MQTT_ENABLED = false;
String MQTT_BROKER = "192.168.1.100";
int MQTT_PORT = 1883;
String MQTT_USER = "";
String MQTT_PASSWORD = "";
String MQTT_TOPIC_PREFIX = "mirrorclock";

// ====== CONFIGURATION MANAGEMENT ======
void loadConfig() {
    preferences.begin("mirrorclock", false);
    
    LED_PIN = preferences.getInt("led_pin", 3);
    LED_COUNT = preferences.getInt("led_cnt", 114);
    LED_R = preferences.getInt("led_r", 255);
    LED_G = preferences.getInt("led_g", 255);
    LED_B = preferences.getInt("led_b", 255);
    LED_BRIGHTNESS = preferences.getInt("led_brt", 150);
    LED_ENABLED = preferences.getBool("led_en", true);
    LED_AUTO_BRIGHTNESS = preferences.getBool("led_auto", false);
    
    TRANSITION_EFFECT = preferences.getInt("trans_ef", 0);
    TRANSITION_DURATION = preferences.getInt("trans_dur", 1000);
    
    TIMEZONE = preferences.getString("tz", "CET-1CEST,M3.5.0/2,M10.5.0/3");
    NTP_SERVER = preferences.getString("ntp", "pool.ntp.org");
    
    LIGHT_SENSOR_SDA = preferences.getInt("ls_sda", D4);
    LIGHT_SENSOR_SCL = preferences.getInt("ls_scl", D3);
    LIGHT_SENSOR_MAX_LUX = preferences.getInt("ls_max_l", 200);
    LIGHT_SENSOR_MIN_LUX = preferences.getInt("ls_min_l", 0);
    LIGHT_SENSOR_CALIBRATION_MAX = preferences.getInt("ls_cal_m", 10);
    LIGHT_SENSOR_MIN_BRIGHTNESS = preferences.getInt("ls_min_b", 5);
    LIGHT_SENSOR_MAX_BRIGHTNESS = preferences.getInt("ls_max_b", 255);

    UPDATE_INTERVAL = preferences.getULong("upd_int", 100);
    AUTO_BRIGHTNESS_ENABLED = preferences.getBool("auto_brt", false);

    WEB_SERVER_PORT = preferences.getInt("web_port", 80);

    MQTT_ENABLED = preferences.getBool("mqtt_en", false);
    MQTT_BROKER = preferences.getString("mqtt_ip", "192.168.1.100");
    MQTT_PORT = preferences.getInt("mqtt_p", 1883);
    MQTT_USER = preferences.getString("mqtt_u", "");
    MQTT_PASSWORD = preferences.getString("mqtt_pw", "");
    MQTT_TOPIC_PREFIX = preferences.getString("mqtt_top", "mirrorclock");

    preferences.end();
}

void saveConfig() {
    preferences.begin("mirrorclock", false);
    
    preferences.putInt("led_pin", LED_PIN);
    preferences.putInt("led_cnt", LED_COUNT);
    preferences.putInt("led_r", LED_R);
    preferences.putInt("led_g", LED_G);
    preferences.putInt("led_b", LED_B);
    preferences.putInt("led_brt", LED_BRIGHTNESS);
    preferences.putInt("trans_ef", TRANSITION_EFFECT);
    preferences.putInt("trans_dur", TRANSITION_DURATION);

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

    preferences.putULong("upd_int", UPDATE_INTERVAL);
    preferences.putBool("auto_brt", AUTO_BRIGHTNESS_ENABLED);

    preferences.putInt("web_port", WEB_SERVER_PORT);

    preferences.putBool("mqtt_en", MQTT_ENABLED);
    preferences.putString("mqtt_ip", MQTT_BROKER);
    preferences.putInt("mqtt_p", MQTT_PORT);
    preferences.putString("mqtt_u", MQTT_USER);
    preferences.putString("mqtt_pw", MQTT_PASSWORD);
    preferences.putString("mqtt_top", MQTT_TOPIC_PREFIX);

    preferences.end();
}
