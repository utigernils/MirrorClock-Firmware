#pragma once
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void initLightSensor() {
  Wire.begin(D4, D3); 
  if (lightMeter.begin()) {
    Serial.println("BH1750 initialized");
  } else {
    Serial.println("Error initializing BH1750");
  }
}

uint8_t getBrightness() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Ambient light: ");
  Serial.print(lux);
  Serial.println(" lx");

  lux = constrain(lux, 0, 200); 
  uint8_t brightness = map(lux, 0, 10, 10, 255); 
  return brightness;
}
