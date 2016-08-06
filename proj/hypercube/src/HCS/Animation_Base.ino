// Simple theater chase where packets move continuously
void chaser(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  fill_palette(leds, numLED, pal_index, 4, gPalette, gBrightness, gBlending);
}

// Fills a stip and empties it
void fill_to_empty(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  // Some variables for the filling animation
  static boolean _fill = true; // Flag to know which way we are filling
  static uint16_t lead = 0;

  EVERY_N_MILLISECONDS(100) {
    pal_index = (pal_index + 1) % numLED;
  }

  if (_fill) {
    //fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    fill_solid(leds, pal_index, CRGB::Red);
  }
  else {
    fill_solid(leds, pal_index, CRGB::Black);
  }

  // Switch fill flag if we are done filling. If we are done emptying, switch
  // the flag and also grab the next color in the palette. Try this in here
  // to see if it fixes the issue of missing the switch at 0
  if (lead == numLED - 1) {
    _fill = !_fill;
  }
}
