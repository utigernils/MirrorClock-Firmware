#include "wifi_connection.h"
#include "web_api.h"
#include "time_sync.h"
#include "led_driver.h"
#include "lines.h"
#include "light_sensor.h"

int lastHour = -1;
int lastMinute = -1;

void setup() {
  Serial.begin(115200);

  connectToWiFi();
  setupWebAPI();
  startNTP();
  setupLEDs();
  initLightSensor();

  lightLine(IT);
  lightLine(IS);
  strip.show();
}

void loop() {
  handleWebRequests();

  strip.setBrightness(getBrightness());
  strip.show();

  int hours = getHour();
  int minutes = getMinute();

  if (hours >= 12) hours -= 12;

  if (hours != lastHour || minutes != lastMinute) {
    updateWatchface(hours, minutes);
    lastHour = hours;
    lastMinute = minutes;
  }
}
