#include "wifi_connection.h"
#include "web_api.h"
#include "time_sync.h"
#include "led_driver.h"
#include "lines.h"
#include "light_sensor.h"

int lastHour = -1;
int lastMinute = -1;

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 100;

const bool autoBrightness = false;

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
  if (currentTime - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentTime;

    if (autoBrightness) {
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
