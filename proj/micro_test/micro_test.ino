#include <FastLED.h>

#define LED 3
#define NUMLED 3

CRGB leds[NUMLED];

CRGB col;
uint8_t cc = 0;
uint8_t gHue = 0;
uint8_t gVal = 0;
boolean swtch = true;

void setup() {
  FastLED.addLeds<WS2812B, LED, GRB>(leds, NUMLED);
  FastLED.show();
}

void loop() {

//  gVal = beatsin8(10);
//  fill_solid(leds, NUMLED, CHSV(gHue, 255, gVal));
//  EVERY_N_SECONDS(5) {
//    gHue += 60;
//    fill_solid(leds, NUMLED, CHSV(gHue, 255, 255));
//    FastLED.show();
//    delay(1000);
//  }

  for (int i = 0; i < 9; i++) {
    gHue = 255 * (i / 9.);
    fill_solid(leds, NUMLED, CHSV(gHue, 255, 255));
    FastLED.show();
    delay(5000);
    for (int j = 0; j < 5; j++) {
      for (int gg = 0; gg < 255; gg++) {
        fill_solid(leds, NUMLED, CHSV(gHue, 255, gg));
        FastLED.show();
        delay(10);
      }
      for(int gg = 255; gg >= 0; gg--) {
        fill_solid(leds, NUMLED, CHSV(gHue, 255, gg));
        FastLED.show();
        delay(10);
      }
    }
  }

}
