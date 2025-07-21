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
