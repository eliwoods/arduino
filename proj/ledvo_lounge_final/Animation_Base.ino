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
  fill_solid(ih_leds, ih_LED_total, CRGB::Black);
  fill_solid(oh_leds, oh_LED_total, CRGB::Black);
  for (uint8_t dd = 0; dd < 3; dd++) {
    fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
  }
}

void fill_all() {
  CRGB col = CHSV(map(analogRead(HUE_POT), 0, 688, 0, 255), 255, gBrightness);
  fill_solid(ih_leds, ih_LED_total, col);
  fill_solid(oh_leds, oh_LED_total, col);
  for (uint8_t dd = 0; dd < 3; dd++) {
    fill_solid(d_leds[dd], d_LED_num, col);
  }
  FastLED.show();
  FastLED.delay(20);
}

// Draw some basic shapes, this will use fill solid to draw them.
void draw_trapezoid(uint8_t side, CRGB col) {
  // The diagonals
  if (side == 0) {
    // Diagonal
    fill_solid(d_leds[side], d_LED_num, col);
    // Inner Hex
    fill_solid(ih_leds(0, led_strand - 1), led_strand, col);
    // Outer Hex
    fill_solid(oh_leds(0, 2 * led_strand - 1), 2 * led_strand, col);
  }
  else if (side == 1 || side == 2) {
    // Diagonal
    fill_solid(d_leds[side], d_LED_num, col);
    fill_solid(d_leds[side - 1], d_LED_num, col);
    // Inner Hex
    fill_solid(ih_leds(side * (2 * led_strand) + led_strand, (side + 1) * (2 * led_strand) + led_strand - 1), 2 * led_strand, col);
    // Outer Hex
    fill_solid(oh_leds(side * (4 * led_strand) + 2 * led_strand, (side + 1) * (4 * led_strand) + 2 * led_strand - 1), 4 * led_strand, col);
  }
  if (side == 4) {
    // Diagonal
    fill_solid(d_leds[side - 1], d_LED_num, col);
    // Inner Hex
    fill_solid(ih_leds(ih_LED_total - led_strand, ih_LED_total - 1), led_strand, col);
    // Outer Hex
    fill_solid(oh_leds(oh_LED_total - 2 * led_strand, oh_LED_total - 1), 2 * led_strand, col);
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
    thisTimer.setPeriod(map(analogRead(DJ_POT), 0, 1023, 50, 800));
    fill_solid(ih_leds, ih_LED_total, CHSV(255, 0, gBrightness));
    fill_solid(oh_leds, oh_LED_total, CHSV(255, 0, gBrightness));
    for (uint8_t dd = 0; dd < 3; dd++) {
      fill_solid(d_leds[dd], d_LED_num, CHSV(255, 0, gBrightness));
    }
  }
  //FastLED.setBrightness(gBrightness);
  FastLED.show();
  reset_all();
}

// Strobes black over animations. Basically we play this animation in
// conjunction with whatever else is running. It's not really an interrupt
// animation but rather another layer.
void strobe_black() {
  EVERY_N_MILLISECONDS_I(thisTimer, 175) {
    thisTimer.setPeriod(map(analogRead(DJ_POT), 0, 1023, 50, 800));
    reset_all();
    FastLED.show();
    FastLED.delay(50);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
// All of the following animations are some kind of theater marquee style. This means    //
// at the core, its  packets of light chasing eachother across the strip, although we'll //
// change up how they chase in different animations.                                     //
///////////////////////////////////////////////////////////////////////////////////////////

// Simple theater chase where packets move continuously
void theater_chase(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  fill_palette(leds, numLED, pal_index, 6, gPalette, gBrightness, gBlending);
}

// A theater chase where packets switch direction every once in a while. Lets see how
// this looks with a saw wave
void theater_chase_tri(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  fill_palette(leds, numLED, triwave(pal_index), 4, gPalette, maxBrightness, gBlending);
}

// A glitchy style "chase". It not even really a chase, I just keep the palette static and
// change the width of the packets. Looks kind of funky, but kind of cool too.
void theater_chase_mod(CRGB *leds, uint16_t numLED, uint8_t col_inc) {
  fill_palette(leds, numLED, 0, col_inc, gPalette, maxBrightness, gBlending);
}

// Fill the whole strip from left to right, then empty from left to right.
/*void fill_to_empty(CRGB *leds, uint16_t numLED) {
  // Index for grabbing colors from the global palette
  static uint8_t pal_index = 0;

  // Some variables for the filling animation
  static boolean _fill = true; // Flag to know which way we are filling
  static uint16_t lead = 0;

  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 10, 200));
    lead = (lead + 1) % numLED;
  }

  if (_fill) {
    //fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    fill_solid(leds, lead, CRGB::Red);
  }
  else {
    fill_solid(leds, lead, CRGB::Black);
  }
  FastLED.show();

  // Switch fill flag if we are done filling. If we are done emptying, switch
  // the flag and also grab the next color in the palette. Try this in here
  // to see if it fixes the issue of missing the switch at 0
  if (lead == numLED - 1) {
    if (_fill) {
      _fill = false;
    }
    else {
      _fill = true;
    }
  }
  }*/


// Pretty self explanatory. Grab a random LED and turn it on if it's off, or turn it off if it's on.
// Rinse and repeat. Have to do this with a delay based method though :(
/*void starry_night_flicker(CRGB *leds, uint16_t numLED) {
  // Grab a random LED
  uint16_t led = random16(numLED);

  // Now ramp up the brightness if the pixel is off, or ramp it down if it's on
  if (leds[led].r == 0 && leds[led].g == 0 && leds[led].b == 0) {
    for (uint8_t bb = 0; bb < 256; bb += 16) {
      leds[led] = CRGB(bb, bb, bb);
      FastLED.show();
      FastLED.delay(5);
    }
  }
  else {
    for (uint8_t bb = 255; bb >= 0; bb -= 16) {
      leds[led] = CRGB(bb, bb, bb);
      FastLED.show();
      FastLED.delay(5);
    }
  }
  }*/
