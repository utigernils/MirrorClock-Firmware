#pragma once
#include <Adafruit_NeoPixel.h>
#include "lines.h"

#define LED_PIN 3
#define LED_COUNT 114

int R = 255;
int G = 255;
int B = 255;
int Brightness = 150;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setupLEDs() {
  strip.begin();
  strip.clear();
  strip.show();
}

int roundTo5(int value) {
  return (value / 5) * 5;
}

void lightLed(int x, int y) {
  strip.setBrightness(Brightness);
  int ledNum = 0;
  if (x <= 11 && y <= 10) {
    if (x % 2 != 0)
      ledNum = (y + ((x - 1) * 10));
    else
      ledNum = ((11 - y) + ((x - 1) * 10));

    strip.setPixelColor(ledNum - 1, strip.Color(R, G, B));
  }
}

void lightLine(const Line &line) {
  for (int i = line.begin; i <= line.end; i++) {
    lightLed(i, line.row);
  }
}

void updateWatchface(int hours, int minutes) {
  Serial.println("Updating watchface...");
  strip.clear();

  int remainder = minutes % 5;
  if (remainder != 0) {
    for (int i = 1; i <= remainder; i++) {
      strip.setPixelColor(110 + i - 1, strip.Color(R, G, B));
    }
  }

  minutes = roundTo5(minutes);

  lightLine(IT);
  lightLine(IS);

  switch (minutes) {
    case 5: lightLine(FIVE); lightLine(AFTER); break;
    case 10: lightLine(TEN); lightLine(AFTER); break;
    case 15: lightLine(QUARTER); lightLine(AFTER); break;
    case 20: lightLine(TWENTY); lightLine(AFTER); break;
    case 25: lightLine(FIVE); lightLine(BEFORE); lightLine(HALF); break;
    case 30: lightLine(HALF); hours++; break;
    case 35: lightLine(FIVE); lightLine(AFTER); lightLine(HALF); hours++; break;
    case 40: lightLine(TWENTY); lightLine(BEFORE); hours++; break;
    case 45: lightLine(QUARTER); lightLine(BEFORE); hours++; break;
    case 50: lightLine(TEN); lightLine(BEFORE); hours++; break;
    case 55: lightLine(FIVE); lightLine(BEFORE); hours++; break;
  }

  if (hours == 12) hours = 0;

  switch (hours) {
    case 0: lightLine(TWELVE); break;
    case 1: lightLine(ONE); break;
    case 2: lightLine(TWO); break;
    case 3: lightLine(THREE); break;
    case 4: lightLine(FOUR); break;
    case 5: lightLine(FIVE_HOUR); break;
    case 6: lightLine(SIX); break;
    case 7: lightLine(SEVEN); break;
    case 8: lightLine(EIGHT); break;
    case 9: lightLine(NINE); break;
    case 10: lightLine(TEN_HOUR); break;
    case 11: lightLine(ELEVEN); break;
  }

  strip.show();
}
