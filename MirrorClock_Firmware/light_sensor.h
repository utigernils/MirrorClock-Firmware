#pragma once
#include <Wire.h>
#include <BH1750.h>
#include "config.h"

BH1750 lightMeter;

void initLightSensor() {
  Wire.begin(LIGHT_SENSOR_SDA, LIGHT_SENSOR_SCL); 
  if (lightMeter.begin()) {
    Serial.println("BH1750 initialized");
  } else {
    Serial.println("Error initializing BH1750");
  }
}

uint8_t getBrightness() {
  float lux = lightMeter.readLightLevel();
  lux = constrain(lux, LIGHT_SENSOR_MIN_LUX, LIGHT_SENSOR_MAX_LUX); 
  uint8_t brightness = map(lux, LIGHT_SENSOR_MIN_LUX, LIGHT_SENSOR_CALIBRATION_MAX, LIGHT_SENSOR_MIN_BRIGHTNESS, LIGHT_SENSOR_MAX_BRIGHTNESS); 
  return brightness;
}

uint8_t getLightLevel() {
  float lux = lightMeter.readLightLevel();
  return lux;
}


