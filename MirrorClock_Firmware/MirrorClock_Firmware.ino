#include "app.h"

App* mirrorClockApp;

void setup() {
  Serial.begin(115200);
  loadConfig();
  mirrorClockApp = new App();
  mirrorClockApp->setup();
}

void loop() {
  mirrorClockApp->loop();
}
