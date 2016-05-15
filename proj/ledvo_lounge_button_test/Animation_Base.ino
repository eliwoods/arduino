// Some utility shit

// Reset everything aka set everything to black
void reset_all() {
  fill_solid(leds, num_strip * numLED, CRGB::Black);
}

////////////////////////////////////////////////////////////
// DJ animations. These are meant to interrupt the normal //
// animations and take over.                              //
////////////////////////////////////////////////////////////

// Can't be named strobe for some reason... IDK man
void strobes() {
  // Fill everything with white at a user controlled rate, then fade to black to
  // get the strobe effect that we want
  EVERY_N_MILLISECONDS(200) {
    //thisTimer.setPeriod(map(analogRead(DJ_POT), 0, 1253, 50, 300));
    fill_solid(leds, num_strip * numLED, CRGB::White);
  }
  LEDS.show();
  fill_solid(leds, num_strip * numLED, CRGB::Black);
}

// Strobes black over animations. Basically we play this animation in
// conjunction with whatever else is running. It's not really an interrupt
// animation but rather another layer.
void strobe_black() {
  EVERY_N_MILLISECONDS_I(thisTimer, 175) {
    fill_solid(leds, num_strip * numLED, CRGB::Black);
    LEDS.show();
    FastLED.delay(50);
  }
}

// Does what it says, sets every LED to black
void blackout() {
  fill_solid(leds, num_strip * numLED, CRGB::Black);
}

// Same as blackout, but with white.
void whiteout() {
  fill_solid(leds, num_strip * numLED, CRGB::White);
  LEDS.show();
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
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 500));
    if(anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }
  for (uint8_t ss = 0; ss < num_strip; ss++) {
    fill_palette(leds + ss * numLED, numLED, pal_index, 6, gPalette, gBrightness, gBlending);
  }
  LEDS.show();
}

// A theater chase where packets switch direction every once in a while. Lets see how
// this looks with a saw wave
void theater_chase_tri() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 500));
    if(anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  for (uint8_t ss = 0; ss < num_strip; ss++) {
    fill_palette(leds + ss * numLED, numLED, triwave8(pal_index), 6, gPalette, gBrightness, gBlending);
  }
  LEDS.show();
}

void theater_chase_mod() {
  static uint8_t col_inc = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 500));
    if(anim_reverse) {
      col_inc--;
    }
    else {
      col_inc++;
    }
  }
  for (uint8_t ss = 0; ss < num_strip; ss++) {
    if (col_inc < 256 / 2) {
      fill_palette(leds + ss * numLED, numLED, 0, col_inc, gPalette, maxBrightness, gBlending);
    }
    else {
      fill_palette(leds + ss * numLED, numLED, 0, 256 - col_inc, gPalette, maxBrightness, gBlending);
    }
  }
  LEDS.show();
}
