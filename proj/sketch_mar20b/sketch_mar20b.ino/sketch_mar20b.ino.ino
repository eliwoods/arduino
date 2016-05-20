#include <FastLED.h>

#define NUMLED 30 

#define LEDIN 7

CRGB leds[NUMLED];

void setup() {
  delay(2000);

  FastLED.addLeds<WS2812B, LEDIN, GRB>(leds, NUMLED);
  FastLED.setBrightness(100);
  FastLED.show();

}

void loop() {

  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(10) {
    hue++;
  }

  fill_rainbow(leds, NUMLED, hue, 10);
  FastLED.show();


}
