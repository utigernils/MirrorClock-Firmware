#pragma once
#include <time.h>

const char* TZ_EUROPE_ZURICH = "CET-1CEST,M3.5.0/2,M10.5.0/3";
const char* NTP_SERVER = "pool.ntp.org";

void startNTP() {
  configTime(TZ_EUROPE_ZURICH, NTP_SERVER);
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
