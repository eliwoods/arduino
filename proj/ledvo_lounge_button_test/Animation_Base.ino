// Some utility shit

// Reset everything aka set everything to black
void reset_all() {
  fill_solid(ih_leds, ih_LED_total, CRGB::Black);
  fill_solid(oh_leds, oh_LED_total, CRGB::Black);
  for (uint8_t dd = 0; dd < 6; dd++) {
    fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
  }
}

////////////////////////////////////////////////////////////
// DJ animations. These are meant to interrupt the normal //
// animations and take over.                              //
////////////////////////////////////////////////////////////

// Can't be named strobe for some reason... IDK man
void strobes() {
  // Fill everything with white at a user controlled rate, then fade to black to
  // get the strobe effect that we want
  EVERY_N_MILLISECONDS_I(thisTimer, 200) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 50, 800));
    fill_solid(ih_leds, ih_LED_total, CRGB::White);
    fill_solid(oh_leds, oh_LED_total, CRGB::White);
    for (uint8_t dd = 0; dd < 6; dd++) {
      fill_solid(d_leds[dd], d_LED_num, CRGB::White);
    }
  }
  FastLED.show();
  fill_solid(ih_leds, ih_LED_total, CRGB::Black);
  fill_solid(oh_leds, oh_LED_total, CRGB::Black);
  for (uint8_t dd = 0; dd < 6; dd++) {
    fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
  }
}

// Strobes black over animations. Basically we play this animation in
// conjunction with whatever else is running. It's not really an interrupt
// animation but rather another layer.
void strobe_black() {
  EVERY_N_MILLISECONDS_I(thisTimer, 175) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 50, 800));
    fill_solid(ih_leds, ih_LED_total, CRGB::Black);
    fill_solid(oh_leds, oh_LED_total, CRGB::Black);
    for (uint8_t dd = 0; dd < 6; dd++) {
      fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
    }
    FastLED.show();
    FastLED.delay(50);
  }
}

// Does what it says, sets every LED to black
void blackout() {
  fill_solid(ih_leds, ih_LED_total, CRGB::Black);
  fill_solid(oh_leds, oh_LED_total, CRGB::Black);
  for (uint8_t dd = 0; dd < 6; dd++) {
    fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
  }
  FastLED.show();
}

// Same as blackout, but with white.
void whiteout() {
  fill_solid(ih_leds, ih_LED_total, CRGB::White);
  fill_solid(oh_leds, oh_LED_total, CRGB::White);
  for (uint8_t dd = 0; dd < 6; dd++) {
    fill_solid(d_leds[dd], d_LED_num, CRGB::White);
  }
  FastLED.show();
}

///////////////////////////////////////////////////////////////////////////////////////////
// All of the following animations are some kind of theater marquee style. This means    //
// at the core, its  packets of light chasing eachother across the strip, although we'll //
// change up how they chase in different animations.                                     //
///////////////////////////////////////////////////////////////////////////////////////////

// Simple theater chase where packets move continuously
void theater_chase() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  fill_palette(ih_leds, ih_LED_total, pal_index, 6, gPalette, gBrightness, gBlending);
  fill_palette(oh_leds, oh_LED_total, pal_index, 6, gPalette, gBrightness, gBlending);
  for (uint8_t dd = 0; dd < 6; dd++) {
    fill_palette(d_leds[dd], d_LED_num, pal_index, 6, gPalette, gBrightness, gBlending);
  }
  FastLED.show();
}

// A theater chase where packets switch direction every once in a while. Lets see how
// this looks with a saw wave
void theater_chase_tri() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  fill_palette(ih_leds, ih_LED_total, triwave8(pal_index), 6, gPalette, gBrightness, gBlending);
  fill_palette(oh_leds, oh_LED_total, triwave8(pal_index), 6, gPalette, gBrightness, gBlending);
  for (uint8_t dd = 0; dd < 6; dd++) {
    fill_palette(d_leds[dd], d_LED_num, triwave8(pal_index), 6, gPalette, gBrightness, gBlending);
  }
  FastLED.show();
}

void fill_all_pal() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 300));
    pal_index++;
  }

  fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  for(uint8_t dd = 0; dd < 6; dd++) {
    fill_solid(d_leds[dd], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  FastLED.show();
}

void theater_chase_mod() {
  static uint8_t col_inc = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 300));
    if (anim_reverse) {
      col_inc--;
    }
    else {
      col_inc++;
    }
  }
  if (col_inc < 256 / 2) {
    fill_palette(ih_leds, ih_LED_total, 0, col_inc, gPalette, gBrightness, gBlending);
  }
  else {
    fill_palette(ih_leds, ih_LED_total, 0, 256 - col_inc, gPalette, gBrightness, gBlending);
  }
  if (col_inc < 256 / 2) {
    fill_palette(oh_leds, oh_LED_total, 0, col_inc, gPalette, gBrightness, gBlending);
  }
  else {
    fill_palette(oh_leds, oh_LED_total, 0, 256 - col_inc, gPalette, gBrightness, gBlending);
  }
  for (uint8_t dd = 0; dd < 6; dd++) {
    if (col_inc < 256 / 2) {
      fill_palette(d_leds[dd], d_LED_num, 0, col_inc, gPalette, gBrightness, gBlending);
    }
    else {
      fill_palette(d_leds[dd], d_LED_num, 0, 256 - col_inc, gPalette, gBrightness, gBlending);
    }
  }
  LEDS.show();
}
