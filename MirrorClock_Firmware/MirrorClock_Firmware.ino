#include "config.h"
#include "wifi_connection.h"
#include "web_api.h"
#include "time_sync.h"
#include "led_driver.h"
#include "lines.h"
#include "light_sensor.h"

unsigned long lastUpdateTime = 0;

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

  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;

    if (AUTO_BRIGHTNESS_ENABLED) {
      strip.setBrightness(getBrightness());
      strip.show();
    }

    int hours = getHour();
    int minutes = getMinute();

    if (hours >= 12) hours -= 12;

    if (hours != lastHour || minutes != lastMinute) {
      updateWatchface(hours, minutes);
      lastHour = hours;
      lastMinute = minutes;
    }
  }
}
