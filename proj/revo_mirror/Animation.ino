// Some utility shit
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

// Reset everything aka set everything to black
void reset_all() {
  fill_solid(leds, numLED, CRGB::Black);
}

void fill_all() {
  fill_solid(leds, numLED, CHSV(gHue, 255, gBrightness));
  FastLED.show();
  FastLED.delay(20);
}

void fill_all_grad() {
  // Grab the colors
  CRGB col_start = CHSV(gHue, 255, gBrightness);
  CRGB col_end = CHSV((gHue + 128) % 255, 255, gBrightness);
  fill_gradient_RGB(leds, numLED, col_start, col_end);

  FastLED.show();
  FastLED.delay(20);
}

void fill_all_grad_2() {
  CRGB col0 = CHSV(gHue, 255, gBrightness);
  CRGB col1 = CHSV((gHue + 64) % 255, 255, gBrightness);
  CRGB col2 = CHSV((gHue + 128) % 255, 255, gBrightness);
  CRGB col3 = CHSV((gHue + 192) % 255, 255, gBrightness);

  fill_gradient_RGB(leds, 72, col0, col1);
  fill_gradient_RGB(leds + 72, 71, col1, col0);

  FastLED.show();
  FastLED.delay(20);
}

///////////////////////////////////////////////////////////////////////////////////////////
// All of the following animations are some kind of theater marquee style. This means    //
// at the core, its  packets of light chasing eachother across the strip, although we'll //
// change up how they chase in different animations.                                     //
///////////////////////////////////////////////////////////////////////////////////////////

// Simple theater chase where packets move continuously
void theater_chase() {
  fill_palette(leds, numLED, gIndex, 6, gPalette, gBrightness, gBlending);
  FastLED.show();
}

void theater_chase_mir() {
  fill_palette(leds(0, 71), 72, gIndex, 6, gPalette, gBrightness, gBlending);
  leds(72, 142) = leds(71, 0);
  FastLED.show();
}

void theater_chase_mir2() {
  fill_palette(leds(0, 35), 36, gIndex, 6, gPalette, gBrightness, gBlending);
  leds(36, 71) = leds(35, 0);
  leds(72, 107) = leds(0, 35);
  leds(108, 142) = leds(34, 0);
  FastLED.show();
}

// Bouncing with wider bounces
void theater_tri8() {
  fill_palette(leds, numLED, triwave8(gIndex), 6, gPalette, gBrightness, gBlending);
  FastLED.show();
}

void theater_tri8_mir() {
  fill_palette(leds(0, 71), 72, triwave8(gIndex), 6, gPalette, gBrightness, gBlending);
  leds(72, 142) = leds(71, 0);
  FastLED.show();
}

void theater_tri8_mir2() {
  fill_palette(leds(0, 35), 36, triwave8(gIndex), 6, gPalette, gBrightness, gBlending);
  leds(36, 71) = leds(35, 0);
  leds(72, 107) = leds(0, 35);
  leds(108, 142) = leds(34, 0);
  FastLED.show();
}

// A theater chase where packets switch direction every once in a while. Lets see how
// this looks with a saw wave
void theater_tri() {
  fill_palette(leds, numLED, triwave(gIndex), 6, gPalette, gBrightness, gBlending);
  FastLED.show();
}

void theater_tri_mir() {
  fill_palette(leds(0, 71), 72, triwave(gIndex), 6, gPalette, gBrightness, gBlending);
  leds(72, 142) = leds(71, 0);
  FastLED.show();
}

void theater_tri_mir2() {
  fill_palette(leds(0, 35), 36, triwave(gIndex), 6, gPalette, gBrightness, gBlending);
  leds(36, 71) = leds(35, 0);
  leds(72, 107) = leds(0, 35);
  leds(108, 142) = leds(34, 0);
  FastLED.show();
}

// A glitchy style "chase". It not even really a chase, I just keep the palette static and
// change the width of the packets. Looks kind of funky, but kind of cool too.
void theater_mod() {
  fill_palette(leds, numLED, 0, gIndex, gPalette, gBrightness, gBlending);
  FastLED.show();
}

void theater_mod_mir() {
  fill_palette(leds(0, 71), 72, 0, gIndex, gPalette, gBrightness, gBlending);
  leds(72, 142) = leds(71, 0);
  FastLED.show();
}

void theater_mod_mir2() {
  fill_palette(leds(0, 35), 36, 0, gIndex, gPalette, gBrightness, gBlending);
  leds(36, 71) = leds(35, 0);
  leds(72, 107) = leds(0, 35);
  leds(108, 142) = leds(34, 0);
  FastLED.show();
}

// Ramp the brightness up, then cut off quickly like a saw wave
void ramp_up() {
  static uint8_t brightness = 0;
  static uint8_t cIndex = 0;

  // Jump to next color in the gradient if we are at the bottom of the ramp
  if (brightness == 0) {
    gIndex += 12;
  }

  fill_solid(leds, numLED, ColorFromPalette(gPalette, cIndex, gIndex * 4, gBlending));
  FastLED.show();

}

// Jump to a fully illuminated strand, then ramp down. Also like a saw wave, but
// the opposite direction.
void ramp_down() {
  static uint8_t brightness = 0;
  static uint8_t cIndex = 0;

  // Jump to next color in the gradient if we are at the bottom of the ramp
  if (brightness == 0) {
    gIndex += 12;
  }

  fill_solid(leds, numLED, ColorFromPalette(gPalette, cIndex, gIndex * 4, gBlending));
  FastLED.show();
}
