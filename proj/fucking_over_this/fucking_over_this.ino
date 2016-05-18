#include<FastLED.h>

#define NUM_LED 6

CRGB leds[30];

// WS2811_PORTD: 25,26,27,28,14,15,29,11
//

const CRGBPalette16 test(CRGB::Red, CRGB::Blue, CRGB::Orange, CRGB::Purple, CRGB::Yellow,
                         CRGB::Green, CRGB::White, CRGB::Pink, CRGB::Black, CRGB::Black,
                         CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black
                        );

void setup() {
  LEDS.addLeds<WS2812B, 6, GRB>(leds, 30).setCorrection(TypicalLEDStrip);
  LEDS.setMaxPowerInMilliWatts(9000);
}

void loop() {

  /*for (uint8_t ss = 0; ss < 5; ss++) {
    fill_solid(&(leds[ss * NUM_LED]), NUM_LED, ColorFromPalette(test, ss * 16, 32, LINEARBLEND));
  }*/

  fill_solid(leds, 30, ColorFromPalette(test, 0, map(analogRead(0),0, 1253, 0, 255), LINEARBLEND));

  LEDS.show();
  LEDS.delay(10);
}
