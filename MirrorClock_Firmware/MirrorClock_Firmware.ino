#include "wifi_connection.h"
#include "time_sync.h"
#include "led_driver.h"
#include "lines.h"

int lastHour = -1;
int lastMinute = -1;

void setup() {
  Serial.begin(115200);

  connectToWiFi();
  startNTP();
  setupLEDs();

  lightLine(IT);
  lightLine(IS);
  strip.show();
}

void loop() {
  updateTime();
  unsigned long epoch = getEpochTime();

  int hours = ((epoch % 86400L) / 3600) + 1;
  int minutes = (epoch % 3600) / 60;

  if (hours >= 12) hours -= 12;

  if (hours != lastHour || minutes != lastMinute) {
    updateWatchface(hours, minutes);
    lastHour = hours;
    lastMinute = minutes;
  }
}
