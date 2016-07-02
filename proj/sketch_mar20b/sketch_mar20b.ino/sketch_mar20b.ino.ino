#include <FastLED.h>

#define NUMLED 30

#define LEDIN 6

CRGBArray<NUMLED> leds;
CRGBArray<15> t_leds;

void setup() {
  delay(2000);

  FastLED.addLeds<WS2812B, LEDIN, GRB>(leds, NUMLED);
  FastLED.setBrightness(2 00);
  FastLED.show();

}

void loop() {

  fill_solid(leds, NUMLED, CRGB::White);
  FastLED.show();


}

void trap_solid() {
//  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;
//  static uint8_t anim_set = 0;
//  static uint8_t current_trap = 0;
//  static uint8_t next_trap = 0;
//  static boolean just_switched = true;
//  static boolean flagged = false;
//
//  EVERY_N_MILLISECONDS(20) {
//    //pal_index++;
//    if (!flagged) {
//      brightness += 16;
//    }
//    else {
//      brightness -= 16;
//    }
//    if (just_switched) {
//      just_switched = false;
//    }
//  }
//
//  if (!just_switched && !flagged && brightness == 0) {
//    if ( current_trap == 2) {
//      flagged = true;
//      brightness = 255;
//    }
//    current_trap = (current_trap + 1) % 3;
//    just_switched = true;
//  }
//  if (!just_switched && flagged && brightness == 15) {
//    if (current_trap == 2) {
//      flagged = false;
//      brightness = 0;
//    }
//    else {
//      brightness = 255;
//    }
//    fill_solid(leds(current_trap*10, (current_trap+1)*10-1), 10, CRGB::Black);
//    FastLED.show();
//    current_trap = (current_trap + 1) % 3;
//    if(current_trap == 0) {
//      pal_index += 12;
//    }
//    just_switched = true;
//  }
//  fill_solid(leds(current_trap * 10, (current_trap + 1) * 10 - 1), 10, ColorFromPalette(RainbowColors_p, pal_index, brightness, LINEARBLEND));
//  FastLED.show();

  EVERY_N_MILLISECONDS(10) {
    pal_index++;
  }

  static uint8_t brightness = 0;

  brightness = beatsin8(60, 0, 128);
  leds[10] = ColorFromPalette(RainbowColors_p, pal_index, brightness, LINEARBLEND);
  FastLED.show();


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

