#pragma once
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void startNTP() {
  timeClient.begin();
}

void updateTime() {
  timeClient.update();
}

unsigned long getEpochTime() {
  return timeClient.getEpochTime();
}

int getHour() {
  int hour = ((timeClient.getEpochTime() % 86400L) / 3600) + 1;
  return (hour >= 24) ? hour - 24 : hour;
}

int getMinute() {
  return (timeClient.getEpochTime() % 3600) / 60;
}
