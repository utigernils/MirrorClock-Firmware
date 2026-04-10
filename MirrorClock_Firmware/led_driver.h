#pragma once
#include <Adafruit_NeoPixel.h>
#include "lines.h"
#include "config.h"

class LedDriver {
private:
    Adafruit_NeoPixel strip;
    bool current_state[150] = {false};
    bool target_state[150] = {false};
    bool is_transitioning = false;
    unsigned long transition_start = 0;

    int roundTo5(int value) {
        return (value / 5) * 5;
    }

    void lightLed(int x, int y) {
        int ledNum = 0;
        if (x <= 11 && y <= 10) {
            if (x % 2 != 0)
                ledNum = (y + ((x - 1) * 10));
            else
                ledNum = ((11 - y) + ((x - 1) * 10));

            if (ledNum > 0 && ledNum <= LED_COUNT) {
                 target_state[ledNum - 1] = true;
            }
        }
    }

    void lightLine(const Line &line) {
        for (int i = line.begin; i <= line.end; i++) {
            lightLed(i, line.row);
        }
    }

public:
    LedDriver() : strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        strip.begin();
        strip.clear();
        strip.show();
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_LED "NeoPixel strip initialized (" + String(LED_COUNT) + " LEDs)");
        #endif
    }

    void clear() { 
        for(int i=0; i<LED_COUNT; i++) target_state[i] = false; 
        startTransition(); 
    }
    void show() { /* No-op, managed by loop() */ }
    void setBrightness(int b) { strip.setBrightness(b); }
    
    void lightLinePublic(const Line &line) {
        lightLine(line);
        startTransition();
    }

    void updateWatchface(int hours, int minutes) {
        #if DEBUG_ENABLED
            Serial.println(DEBUG_PREFIX_LED "Updating display: " + String(hours) + ":" + String(minutes < 10 ? "0" : "") + String(minutes));
        #endif

        for (int i = 0; i < LED_COUNT; i++) {
            target_state[i] = false;
        }

        if (!LED_ENABLED) {
            startTransition();
            return;
        }

        int remainder = minutes % 5;
        if (remainder != 0) {
            for (int i = 1; i <= remainder; i++) {
                if (110 + i - 1 < LED_COUNT) {
                    target_state[110 + i - 1] = true;
                }
                strip.setPixelColor(110 + i - 1, strip.Color(LED_R, LED_G, LED_B));
            }
        }

        minutes = roundTo5(minutes);

        lightLine(IT);
        lightLine(IS);

        switch (minutes) {
            case 5: lightLine(FIVE); lightLine(AFTER); break;
            case 10: lightLine(TEN); lightLine(AFTER); break;
            case 15: lightLine(QUARTER); lightLine(AFTER); break;
            case 20: lightLine(TWENTY); lightLine(AFTER); break;
            case 25: lightLine(FIVE); lightLine(BEFORE); lightLine(HALF); break;
            case 30: lightLine(HALF); hours++; break;
            case 35: lightLine(FIVE); lightLine(AFTER); lightLine(HALF); hours++; break;
            case 40: lightLine(TWENTY); lightLine(BEFORE); hours++; break;
            case 45: lightLine(QUARTER); lightLine(BEFORE); hours++; break;
            case 50: lightLine(TEN); lightLine(BEFORE); hours++; break;
            case 55: lightLine(FIVE); lightLine(BEFORE); hours++; break;
        }

        if (hours == 12) hours = 0;

        switch (hours) {
            case 0: lightLine(TWELVE); break;
            case 1: lightLine(ONE); break;
            case 2: lightLine(TWO); break;
            case 3: lightLine(THREE); break;
            case 4: lightLine(FOUR); break;
            case 5: lightLine(FIVE_HOUR); break;
            case 6: lightLine(SIX); break;
            case 7: lightLine(SEVEN); break;
            case 8: lightLine(EIGHT); break;
            case 9: lightLine(NINE); break;
            case 10: lightLine(TEN_HOUR); break;
            case 11: lightLine(ELEVEN); break;
        }

        startTransition();
    }

    void startTransition() {
        if (TRANSITION_EFFECT == 0 || TRANSITION_DURATION == 0) {
            // Immediate transition
            for (int i = 0; i < LED_COUNT; i++) {
                current_state[i] = target_state[i];
            }
            is_transitioning = false;
            applyCurrentState();
        } else {
            transition_start = millis();
            is_transitioning = true;
        }
    }

    void loop() {
        if (!is_transitioning) {
            // If color/brightness changed while not transitioning, just reapply
            static int lastR = -1;
            static int lastG = -1;
            static int lastB = -1;
            static int lastBrt = -1;
            if (lastR != LED_R || lastG != LED_G || lastB != LED_B || lastBrt != LED_BRIGHTNESS) {
               applyCurrentState();
               lastR = LED_R; lastG = LED_G; lastB = LED_B; lastBrt = LED_BRIGHTNESS;
            }
            return;
        }

        unsigned long elapsed = millis() - transition_start;
        float progress = (float)elapsed / TRANSITION_DURATION;

        if (progress >= 1.0f) {
            for (int i = 0; i < LED_COUNT; i++) {
                current_state[i] = target_state[i];
            }
            is_transitioning = false;
            applyCurrentState();
            return;
        }

        strip.setBrightness(LED_BRIGHTNESS);
        
        for (int i = 0; i < LED_COUNT; i++) {
            bool was_on = current_state[i];
            bool will_be_on = target_state[i];
            
            uint8_t r = 0, g = 0, b = 0;
            
            // TRANSITION_EFFECT: 1 = Crossfade, 2 = Typewriter
            if (TRANSITION_EFFECT == 1) {
                // Crossfade
                float start_factor = was_on ? 1.0f : 0.0f;
                float end_factor = will_be_on ? 1.0f : 0.0f;
                float current_factor = start_factor + (end_factor - start_factor) * progress;
                
                r = LED_R * current_factor;
                g = LED_G * current_factor;
                b = LED_B * current_factor;
            } else if (TRANSITION_EFFECT == 2) {
                // Typewriter / Sequential
                // The progress sweeps across the LEDs
                float led_threshold = (float)i / LED_COUNT;
                if (progress > led_threshold) {
                    if (will_be_on) { r = LED_R; g = LED_G; b = LED_B; }
                } else {
                    // hasn't reached it yet, keep old state
                    if (was_on) { r = LED_R; g = LED_G; b = LED_B; }
                }
            } else {
                // Fallback to crossfade
                float current_factor = was_on ? (1.0f - progress) : 0.0f;
                if (!was_on && will_be_on) current_factor = progress;
                else if (was_on && will_be_on) current_factor = 1.0f;
                r = LED_R * current_factor;
                g = LED_G * current_factor;
                b = LED_B * current_factor;
            }
            
            strip.setPixelColor(i, strip.Color(r, g, b));
        }
        strip.show();
    }

    void applyCurrentState() {
        strip.setBrightness(LED_BRIGHTNESS);
        strip.clear();
        if (LED_ENABLED) {
            for (int i = 0; i < LED_COUNT; i++) {
                if (current_state[i]) {
                    strip.setPixelColor(i, strip.Color(LED_R, LED_G, LED_B));
                }
            }
        }
        strip.show();
    }

};
