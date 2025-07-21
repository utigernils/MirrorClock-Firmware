#pragma once
#include <time.h>
#include "config.h"

void startNTP() {
  configTime(TIMEZONE, NTP_SERVER);
}

bool getLocalTimeSafe(struct tm* timeinfo) {
  if (!getLocalTime(timeinfo)) {
    Serial.println("Failed to obtain time");
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
