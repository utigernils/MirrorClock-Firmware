#pragma once
#include <Wire.h>
#include <BH1750.h>
#include "config.h"

class LightSensor {
private:
    BH1750 lightMeter;
    uint8_t lastBrightness = 0;

public:
    void begin() {
        Wire.begin(LIGHT_SENSOR_SDA, LIGHT_SENSOR_SCL); 
        if (lightMeter.begin()) {
            #if DEBUG_ENABLED
                Serial.println(DEBUG_PREFIX_SENSOR "BH1750 light sensor initialized");
            #endif
        } else {
            #if DEBUG_ENABLED
                Serial.println(DEBUG_PREFIX_SENSOR "ERROR: Failed to initialize BH1750 sensor");
            #endif
        }
    }

    uint8_t getBrightness() {
        float lux = lightMeter.readLightLevel();
        lux = constrain(lux, LIGHT_SENSOR_MIN_LUX, LIGHT_SENSOR_MAX_LUX); 
        uint8_t brightness = map(lux, LIGHT_SENSOR_MIN_LUX, LIGHT_SENSOR_CALIBRATION_MAX, LIGHT_SENSOR_MIN_BRIGHTNESS, LIGHT_SENSOR_MAX_BRIGHTNESS); 
        
        if (brightness != lastBrightness) {
            #if DEBUG_ENABLED
                Serial.println(DEBUG_PREFIX_SENSOR "Auto brightness: " + String(lux) + " lux → " + String(brightness) + "/255");
            #endif
            lastBrightness = brightness;
        }
        
        return brightness;
    }

    float getLightLevel() {
        return lightMeter.readLightLevel();
    }
};
