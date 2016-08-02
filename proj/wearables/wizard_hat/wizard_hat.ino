#include <FastLED.h>

#define NUMLED 2
#define LEDIN 0

CRGB leds[NUMLED];

void setup() {
  FastLED.addLeds<WS2812B, LEDIN, GRB>(leds, NUMLED);
  FastLED.setBrightness(100);
  FastLED.show();

}

void loop() {
  fill_solid(leds, NUMLED, CRGB::Red);
  FastLED.show();
  FastLED.delay(100);

}
