////////////////////////////////////////////////////////////
// DJ animations. These are meant to interrupt the normal //
// animations and take over.                              //
////////////////////////////////////////////////////////////

// Can't be named strobe for some reason... IDK man
void strobes() {
  // Fill everything with white at a user controlled rate, then fade to black to
  // get the strobe effect that we want
  EVERY_N_MILLISECONDS_I(thisTimer, 150) {
    thisTimer.setPeriod(map(analogRead(DJ_POT), 0, 1253, 50, 300));
    fill_solid(ih_leds, ih_LED_total*6, CRGB::White);
    fill_solid(oh_leds, oh_LED_per_strand*6, CRGB::White);
    for (uint8_t ss = 0; ss < 6; ss++) {
      fill_solid(d_leds[ss], d_LED_per_strand, CRGB::White);
    }
  }

  FastLED.show();
  fadeToBlackBy(ih_leds, ih_LED_total*6, 10);
  fadeToBlackBy(oh_leds, oh_LED_per_strand*6, 10);
  for (uint8_t ss = 0; ss < 6; ss++) {
    fadeToBlackBy(d_leds[ss], d_LED_per_strand, 10);
  }
}

// Strobes black over animations. Basically we play this animation in
// conjunction with whatever else is running. It's not really an interrupt
// animation but rather another layer.
void strobe_black() {
  EVERY_N_MILLISECONDS_I(thisTimer, 175) {
    fill_solid(ih_leds, ih_LED_total*6, CRGB::Black);
    fill_solid(oh_leds, oh_LED_per_strand*6, CRGB::Black);
    for (uint8_t ss = 0; ss < 6; ss++ ) {
      fill_solid(d_leds[ss], d_LED_per_strand, CRGB::Black);
    }
    FastLED.show();
    FastLED.delay(50);
  }
}

// Does what it says, sets every LED to black
void blackout() {
  fill_solid(ih_leds, ih_LED_total, CRGB::Black);
  fill_solid(oh_leds, oh_LED_per_strand*6, CRGB::Black);
  for (uint8_t ss = 0; ss < 6; ss++ ) {
    fill_solid(d_leds[ss], d_LED_per_strand, CRGB::Black);
  }
  FastLED.show();
}

// Same as blackout, but with white.
void whiteout() {
  fill_solid(ih_leds, ih_LED_total, CRGB::White);
  fill_solid(oh_leds, oh_LED_per_strand*6, CRGB::White);
  for (uint8_t ss = 0; ss < 6; ss++ ) {
    fill_solid(d_leds[ss], d_LED_per_strand, CRGB::White);
  }
  FastLED.show();
}

///////////////////////////////////////
// Some simple modulating animations //
///////////////////////////////////////

// Fill the whole bar and fade to black in a smooth oscillating fashion. Also, slowly
// work our way through the global template
void fill_smooth(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  static uint8_t brightness = 0;

  // Some variables to help us smoothly transition between rates and
  // keep track of where we are on the sinwave
  static uint8_t bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
  static uint8_t prev_brightness = 0;
  static uint8_t last_second = 99;
  uint8_t second = (millis() / 1000) % 60;

  // Modulate brigthness at input dependent rate
  prev_brightness = brightness;
  brightness = beatsin8(bpm, 0, maxBrightness);

  // If we are about to fade to black, grab the next color in the palette
  if (brightness == 1 && (brightness - prev_brightness) < 0) {
    pal_index += 16;
  }

  // Check for when we should switch bpm here, so we don't interrupt color assignment
  if (brightness == 1 && (brightness - prev_brightness) < 0 && last_second != second) {
    last_second = second;
    bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
  }

  // Fill the whole strip
  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();
}

// A single equalizer type bar
void palette_eq(CRGB *leds, uint16_t numLED) {
  static uint8_t pal_index = 0;
  static uint16_t lead_max = numLED / 2;
  static uint16_t lead = 0;

  // Some variables to help us smoothly transition between rates and
  // keep track of where we are on the sinwave
  static uint8_t bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
  static uint8_t prev = 0;
  static uint8_t last_second = 99;
  uint8_t second = (millis() / 1000) % 60;

  // Fill bar at input dependent rate. First record the previous lead value
  // so that we can check which diretion we are heading on the sin wave later on
  prev = lead;
  lead = beatsin8(bpm, 0, lead_max);

  // If the eq bar is about to be empty, generate a new max length to fill.
  // Also increment the index of the palette, for a little more modulation cuz
  // why not.
  if (lead == 0 && (lead - prev) < 0) {
    pal_index += 16;
    lead_max = random8(numLED / 3, numLED);
  }

  // Check for rate change here so we don't interrupt the color and length
  // assignment
  if (lead == 0 && (lead - prev) < 0 && last_second != second) {
    last_second = second;
    bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
    lead = 0;
  }

  //fill_palette(leds, lead, pal_index, 16, gPalette, maxBrightness, gBlending);
  fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  FastLED.show();
  fadeToBlackBy(leds, numLED, 20);
}

///////////////////////////////////////////////////////////////////////////////////////////
// All of the following animations are some kind of theater marquee style. This means    //
// at the core, its  packets of light chasing eachother across the strip, although we'll //
// change up how they chase in different animations.                                     //
///////////////////////////////////////////////////////////////////////////////////////////

// Simple theater chase where packets move continuously
void theater_chase(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  fill_palette(leds, numLED, pal_index, 6, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

// A theater chase where packets switch directions every once in a while
void theater_chase_bounce(CRGB *leds, uint16_t numLED) {
  static uint8_t pal_index = 0;
  static uint8_t bpm = map(analogRead(RATE_POT), 0 , 1253, 60, 20);

  // Some variables to help us smoothly transition between rates
  static uint8_t last_index = 0;
  static uint8_t last_second = 99;
  uint8_t second = (millis() / 1000) % 60;

  // Update lead LED position at an input dependent rate
  last_index = pal_index;
  pal_index = beatsin8(bpm, 0, 155);

  if ((pal_index == 127 && (last_index - pal_index) < 0) && second != last_second) {
    last_second = second;
    bpm = map(analogRead(RATE_POT), 0 , 1253, 60, 20);
  }

  fill_palette(leds, numLED, pal_index, 4, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

// A theater chase where packets switch direction every once in a while. Lets see how
// this looks with a saw wave
void theater_chase_saw(CRGB *leds, uint16_t numLED) {
  static uint8_t pal_index = 0;

  // Increment palette index at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 500));
    pal_index++;
  }

  fill_palette(leds, numLED, triwave8(pal_index), 4, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

void theater_chase_mod(CRGB *leds, uint16_t numLED) {
  // Index shit
  static uint8_t pal_index = 0;
  static uint8_t col_inc = 0;

  // Move the palette index every 10 ms
  EVERY_N_MILLISECONDS(10) {
    pal_index++;
  }
  // Update the fill_palette spacing at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 5) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 4000));
    col_inc++;
  }
  if (col_inc < 256 / 2) {
    fill_palette(leds, numLED, pal_index, col_inc, gPalette, maxBrightness, gBlending);
  }
  else {
    fill_palette(leds, numLED, pal_index, 256 - col_inc, gPalette, maxBrightness, gBlending);
  }
  FastLED.show();
}

// THESE FUNCTIONS ARE UNDER SERIOUS CONSTRUCTION!!!!!!!!! //

// Ramp the brightness up, then cut off quickly like a saw wave
void fill_ramp_up(CRGB *leds, uint16_t numLED) {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;

  // Ramp the brightness up at a input controlled rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    brightness += 16;
  }

  // Jump to next color in the gradient if we are at the bottom of the ramp
  if (brightness == 0) {
    pal_index += 16;
  }

  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();

}

// Jump to a fully illuminated strand, then ramp down. Also like a saw wave, but
// the opposite direction.
void fill_ramp_down(CRGB *leds, uint16_t numLED) {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;

  // Ramp the brightness up at a input controlled rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    brightness -= 16;
  }

  // Jump to next color in the gradient if we are at the bottom of the ramp
  if (brightness == 0) {
    pal_index -= 16;
  }

  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();
}


// Like palette_eq, but uses a triangular wave. This lets us have more control over the speed
// and transition smoother between animation rates.
void palette_eq_tri(CRGB *leds, uint16_t numLED, uint16_t lead, uint16_t lead_max, uint8_t pal_index) {

  // Fill the the bar until we get to lead_max, then work our way back down. This doesn't use
  // triwave8 explicitly since it has a set range and we need to change the amplitude of the triangluar
  // each time we make a "round trip"
  if (lead < lead_max) {
    fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else {
    fill_solid(leds, 2 * lead_max - lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  FastLED.show();
  // Need this so that the decreasing direction animates properly
  fadeToBlackBy(leds, numLED, 10);
}

// Fill the whole strip from left to right, then empty from left to right.
void fill_to_empty(CRGB *leds, uint16_t numLED) {
  // Index for grabbing colors from the global palette
  static uint8_t pal_index = 0;

  // Some variables for the filling animation
  static boolean _fill = true; // Flag to know which way we are filling
  static uint16_t lead = 0;

  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
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
}


// Pretty self explanatory. Grab a random LED and turn it on if it's off, or turn it off if it's on.
// Rinse and repeat. Have to do this with a delay based method though :(
void starry_night_flicker(CRGB *leds, uint16_t numLED) {
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
}

/*void starry_night_vector(CRGB *leds, uint16_t numLED) {
  // Vector to store the randomly chosen leds
  static std::vector<uint16_t> stars;

  // If the vector is empty, we are either running for the first time
  // or we are on the next cycle. Either way we want to set it up
  if (stars.size() == 0) {
    uint8_t rnd;
    for (uint16_t ll = 0; ll < numLED; ll++) {
      rnd = random8();
      // Only set ~1/3 of the leds as a star
      if (rnd < (255 / 3)) {
        stars.push_back(ll);
      }
    }
  }

  // Increase brightness at an input controlled rate
  static uint8_t brightness_counter = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 150));
    brightness_counter += 4;
  }

  // Now loop over stars and ramp their brightness up and down
  uint8_t brightness = triwave8(brightness_counter);
  for (uint16_t ll = 0; ll < stars.size(); ll++ ) {
    leds[stars[ll]] = CRGB(brightness, brightness, brightness);
  }

  FastLED.show();

  // Check if we are at the valley of our triwave, and clear the
  // vector for the next set of stars if we are.
  if (brightness_counter == 0) {
    stars.clear();
  }

  }*/


// Random blocks of colors with a random width.
/*void theater_chase_random(CRGB *leds, uint16_t numLED) {
  // Vector to hold the widths, since they're randomly
  // determined, we need a vector to deal with varying sizes
  static std::vector<uint8_t> packet_widths;
  static std::vector<CRGB> packet_colors;

  // Fill the vector the first time it's run. Use a static variable
  // so we only have to run it once
  static boolean first_run = false;
  if (first_run) {
    uint8_t rnd_width;
    uint8_t num_filled = 0; // Keep track of how many LEDs we've filled
    uint8_t pal_index = 0;
    while (num_filled < numLED) {
      // Generate new packet length
      rnd_width = random8(2, 5);
      packet_widths.push_back(rnd_width);
      num_filled += rnd_width;

      // Assign packet a color from the current palette
      packet_colors.push_back(ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
      pal_index += 16;
    }

    // Now loop through the vectors and fill the strand
    for (uint8_t vec_index = 0; vec_index < packet_widths.size(); vec_index++) {

    }

  }

  }*/
