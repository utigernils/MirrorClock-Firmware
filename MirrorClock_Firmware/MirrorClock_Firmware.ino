#include "app.h"

App* mirrorClockApp;

void setup() {
  mirrorClockApp = new App();
  mirrorClockApp->setup();
}

void loop() {
  mirrorClockApp->loop();
}
