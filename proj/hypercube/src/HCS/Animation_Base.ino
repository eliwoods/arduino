/////////////////////////////////////////////////
// These are the more utility based functions. //
/////////////////////////////////////////////////

// Copies the inner and outer led templates to the main array and
// pushes them to the LEDs
void merge_animations() {
  leds(0, in_LED_tot - 1) = in_leds;
  leds(in_LED_tot, led_tot - 1) = out_leds;
  LEDS.show();
}

// Just resets the color of all the leds, doesn't push it though since
// we'll usually do something with them right after calling this.
void clear_all() {
  fill_solid(leds, led_tot, CRGB::Black);
}

void clear_in() {
  fill_solid(leds, in_LED_tot, CRGB::Black);
}

void clear_out() {
  fill_solid(leds + in_LED_tot, out_LED_tot, CRGB::Black);
}

////////////////////////////////////////////////////////////////////////////////////////
// These are really simple animations, that will be layered to make more intereseting //
// configurations.                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

// Simple theater chase where packets move continuously
void chaser(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  fill_palette(leds, numLED, pal_index, 4, gPalette, gBrightness, gBlending);
}

// Fills a stip and empties it. THIS IS GONNA NEED SOME THINKING IN ORDER TO MAKE
// IT GLOBAL
void fill_to_empty(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  // Some variables for the filling animation
  static boolean _fill = true; // Flag to know which way we are filling

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
  if (pal_index == numLED - 1) {
    _fill = !_fill;
  }
}

// Draw a circle, the index goes from 0->strand_len which represents the bottom led
// on a strand to the top LED. The third argument is a flag that chooses which cylinder
// to draw to, false -> inner and true -> outer. For this, we'll assume that the index
// represents the top pixel of a given circle's width.
void draw_circle(uint8_t index, uint8_t cir_width, CRGB col, boolean outer) {
  // A quick error check
  if (cir_width < 1) {
    cir_width = 1;
  }


  // Draw the circle by just setting the exact pixels to the color. This will let us
  // draw over other patterns without completely overwriting the template
  if (outer) {
    for (uint8_t ss = 0; ss < out_strips; ss++) {
      if (ss % 2 == 0) {
        // For variable circle width
        for (uint8_t cc = 0; cc < cir_width; cc++) {
          // Check that we don't go out of bounds for the strip
          if (index + cc > (ss + 1)*strip_len - 1) {
            leds[in_LED_tot + ss * strip_len + index + cc] = col;
          }
        }
      } // End even strip loop
      else {
        // For variable circle width
        for (uint8_t cc = 0; cc < cir_width; cc++) {
          // Check for bounds
          if (index - cc < ss * strip_len) {
            leds[in_LED_tot + (ss + 1)*strip_len - index - 1] = col;
          }
        }
      } // End odd strip loop
    } // End loop over strips
  } // End of outer shell
  else {
    for (uint8_t ss = 0; ss < in_strips; ss++) {
      if (ss % 2 == 0) {
        // For variable circle width
        for (uint8_t cc = 0; cc < cir_width; cc++) {
          // Check that we don't go out of bounds for the strip
          if (index + cc > (ss + 1)*strip_len - 1) {
            leds[ss * strip_len + index + cc] = col;
          }
        }
      } // End even strip loop
      else {
        // For variable circle width
        for (uint8_t cc = 0; cc < cir_width; cc++) {
          // Check for bounds
          if (index - cc < ss * strip_len) {
            leds[(ss + 1)*strip_len - index - 1] = col;
          }
        }
      } // End odd strip loop
    } // End loop over strips
  } // End of inner shell 

}

