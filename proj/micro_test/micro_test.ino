#include <FastLED.h>

#define LED 6
#define NUMLED 500

CRGB leds[NUMLED];

void setup() {
  FastLED.addLeds<WS2812B, LED, GRB>(leds, NUMLED);
  FastLED.show();
}

void loop() {
  static uint16_t led_on = 0;

  EVERY_N_MILLISECONDS(20) {
    led_on = (led_on+1)%NUMLED;
  }

  fill_solid(leds, led_on, CRGB::Red);
  FastLED.show();

}
