#pragma once
#include <time.h>
#include "config.h"

void startNTP() {
  configTime(TIMEZONE, NTP_SERVER);
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_TIME "NTP sync started with " + String(NTP_SERVER));
  #endif
}

bool getLocalTimeSafe(struct tm* timeinfo) {
  if (!getLocalTime(timeinfo)) {
    static unsigned long lastError = 0;
    unsigned long now = millis();
    if (now - lastError > 30000) { // Only print error every 30 seconds
      #if DEBUG_ENABLED
        Serial.println(DEBUG_PREFIX_TIME "ERROR: Failed to obtain time from NTP");
      #endif
      lastError = now;
    }
    return false;
  }
  return true;
}

int getHour() {
  struct tm timeinfo;
  if (!getLocalTimeSafe(&timeinfo)) return -1;
  return timeinfo.tm_hour;
}

int getMinute() {
  struct tm timeinfo;
  if (!getLocalTimeSafe(&timeinfo)) return -1;
  return timeinfo.tm_min;
}

unsigned long getEpochTime() {
  time_t now;
  time(&now);
  return now;
}
