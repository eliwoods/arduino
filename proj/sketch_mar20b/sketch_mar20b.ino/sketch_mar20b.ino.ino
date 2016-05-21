#include <FastLED.h>

#define NUMLED 30 

#define LEDIN 7

CRGBArray<NUMLED> leds;
CRGBArray<15> t_leds;

void setup() {
  delay(2000);

  FastLED.addLeds<WS2812B, LEDIN, GRB>(leds, NUMLED);
  FastLED.setBrightness(100);
  FastLED.show();

}

void loop() {

  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(100) {
    hue++;
  }

  theater_chase_mod(leds(15,29), 15, hue);
  
  //leds(29, 15) = leds(0, 14);
  //theater_chase_mod(leds(29, 15), 15, hue);
  //fill_rainbow(leds, NUMLED, triwave(hue), 10);
  FastLED.show();
  //theater_chase_mod(leds, NUMLED, hue);


}

void theater_chase_mod(CRGB *leds, uint16_t numLED, uint8_t col_inc) {
//  if (col_inc < 256 / 2) {
//    fill_palette(leds, numLED, 0, col_inc, RainbowColors_p, 100, LINEARBLEND);
//  }
//  else {
//    fill_palette(leds, numLED, 0, 256 - col_inc, RainbowColors_p, 100, LINEARBLEND);
//  }
fill_palette(leds, numLED, col_inc, 6, RainbowColors_p, 100, LINEARBLEND);
  FastLED.show();
}

uint8_t triwave(uint8_t in) {
  if ( in < 64) {
    return in;
  }
  else if (in >= 64 && in < 128) {
    return (127 - in);
  }
  else if (in >= 128 && in < 192) {
    return (in - 128);
  }
  else if (in >= 192) {
    return (255 - in);
  }
}

void rnbw(CPixelView<CRGB> _leds, uint8_t hue) {
  fill_rainbow(t_leds, 15, hue, 10);
  _leds(0, 14) = t_leds;
  _leds(29, 15) = t_leds;
}

