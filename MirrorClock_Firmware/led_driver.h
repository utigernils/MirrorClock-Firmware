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

    int current_r = -1, current_g = -1, current_b = -1, current_brt = -1;
    int start_r = 0, start_g = 0, start_b = 0, start_brt = 0;

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
        if (current_r == -1) {
            current_r = LED_R; current_g = LED_G; current_b = LED_B; current_brt = LED_BRIGHTNESS;
        }
        
        start_r = current_r;
        start_g = current_g;
        start_b = current_b;
        start_brt = current_brt;

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
        // Trigger transition if color/brightness was changed manually (e.g. from API/HomeAssistant)
        if (!is_transitioning) {
            if (current_r != LED_R || current_g != LED_G || current_b != LED_B || current_brt != LED_BRIGHTNESS) {
               for (int i = 0; i < LED_COUNT; i++) {
                   target_state[i] = current_state[i];
               }
               startTransition();
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

        float interpolated_brt = start_brt + (LED_BRIGHTNESS - start_brt) * progress;
        strip.setBrightness((int)interpolated_brt);

        float interpolated_r = start_r + (LED_R - start_r) * progress;
        float interpolated_g = start_g + (LED_G - start_g) * progress;
        float interpolated_b = start_b + (LED_B - start_b) * progress;

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

                r = interpolated_r * current_factor;
                g = interpolated_g * current_factor;
                b = interpolated_b * current_factor;
            } else if (TRANSITION_EFFECT == 2) {
                // Typewriter / Sequential sweeps across the LEDs
                float led_threshold = (float)i / LED_COUNT;
                if (progress > led_threshold) {
                    if (will_be_on) { r = LED_R; g = LED_G; b = LED_B; }        
                } else {
                    if (was_on) { r = start_r; g = start_g; b = start_b; }
                }
            } else {
                // Fallback crossfade
                float current_factor = was_on ? (1.0f - progress) : 0.0f;       
                if (!was_on && will_be_on) current_factor = progress;
                else if (was_on && will_be_on) current_factor = 1.0f;
                r = interpolated_r * current_factor;
                g = interpolated_g * current_factor;
                b = interpolated_b * current_factor;
            }
            
            strip.setPixelColor(i, strip.Color(r, g, b));
        }
        strip.show();
    }

    void applyCurrentState() {
        current_r = LED_R;
        current_g = LED_G;
        current_b = LED_B;
        current_brt = LED_BRIGHTNESS;

        strip.setBrightness(current_brt);
        strip.clear();
        if (LED_ENABLED) {
            for (int i = 0; i < LED_COUNT; i++) {
                if (current_state[i]) {
                    strip.setPixelColor(i, strip.Color(current_r, current_g, current_b));   
                }
            }
        }
        strip.show();
    }
};
