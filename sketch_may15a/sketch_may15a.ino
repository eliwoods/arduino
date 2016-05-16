#include <FastLED.h>

const uint8_t numLED = 90;
const uint8_t numStrips = 6;

CRGB leds[numLED*numStrips];

void setup() {
  FastLED.addLeds<WS2812B, 0, GRB>(leds, numLED*numStrips);
  FastLED.show();
}

void loop() {
  // put your main code here, to run repeatedly:

}
