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
  
  #if DEBUG_ENABLED
    delay(3000); // Give serial time to connect
    Serial.println("\n==================================================");
    Serial.println(DEBUG_PREFIX_SYSTEM "MirrorClock starting up...");
    Serial.println(DEBUG_PREFIX_SYSTEM "Firmware build: " + String(__DATE__) + " " + String(__TIME__));
    Serial.println(DEBUG_PREFIX_SYSTEM "Free heap: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("==================================================");
  #endif

  connectToWiFi();
  setupWebAPI();
  startNTP();
  setupLEDs();
  initLightSensor();

  // Show initial "IT IS" display
  lightLine(IT);
  lightLine(IS);
  strip.show();
  
  #if DEBUG_ENABLED
    Serial.println(DEBUG_PREFIX_SYSTEM "Startup complete - entering main loop");
    Serial.println("==================================================\n");
  #endif
}

void loop() {
  handleWebRequests();

  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;

    // Periodic status output (every 5 minutes)
    #if DEBUG_ENABLED
      static unsigned long lastStatusTime = 0;
      if (currentTime - lastStatusTime >= 300000) { // 5 minutes
        lastStatusTime = currentTime;
        Serial.println(DEBUG_PREFIX_SYSTEM "Status - Uptime: " + String(currentTime/1000) + "s | Free heap: " + String(ESP.getFreeHeap()) + " bytes");
      }
    #endif

    if (LED_AUTO_BRIGHTNESS) {
      LED_BRIGHTNESS = getBrightness();
      strip.setBrightness(LED_BRIGHTNESS);
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
