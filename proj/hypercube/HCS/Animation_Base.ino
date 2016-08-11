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
void chaser(CRGB *leds, uint16_t numLED, uint8_t pal_index, CRGBPalette16 palette) {
  fill_palette(leds, numLED, pal_index, 4, palette, gBrightness, gBlending);
}

// Fills a stip and empties it. THIS IS GONNA NEED SOME THINKING IN ORDER TO MAKE
// IT GLOBAL
void fill_to_empty(CRGB *leds, uint16_t numLED, uint8_t pal_index, CRGBPalette16 palette) {
  // Some variables for the filling animation
  static boolean _fill = true; // Flag to know which way we are filling

  EVERY_N_MILLISECONDS(100) {
    pal_index = (pal_index + 1) % numLED;
  }

  if (_fill) {
    fill_solid(leds, pal_index, ColorFromPalette(palette, pal_index, gBrightness, gBlending));
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

// A glitchy style "chase". It not even really a chase, I just keep the palette static and
// change the width of the packets. Looks kind of funky, but kind of cool too.
void theater_chase_mod(CRGB *leds, uint16_t numLED, uint8_t col_inc, CRGBPalette16 palette) {
  fill_palette(leds, numLED, 0, col_inc, palette, gBrightness, gBlending);
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

// Sends chasers all going one direction along the chosen shell
// Reverse is just relative to the the non reversed direction, not
// sure which way it physically 
void chase_straight(uint8_t shell, boolean reverse) {
  // Now fill both the inner template
  if (shell == 0) {
    chaser(led_tmplt, strip_len, gIndex, iPalette);
    for (uint8_t s = 0; s < in_strips; s++) {
      if (s % 2 == 0) {
        if (reverse) {
          in_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt(strip_len - 1, 0);
        }
        else {
          in_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          in_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt(strip_len - 1, 0);
        }
        else {
          in_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt;
        }
      }
    }
  }
  // Fill outer template
  else if (shell == 1) {
    chaser(led_tmplt, strip_len, gIndex, oPalette);
    for (uint8_t s = 0; s < out_strips; s++) {
      if (s % 2 == 0) {
        if (reverse) {
          out_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt(strip_len - 1, 0);
        }
        else {
          out_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          out_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt(strip_len - 1, 0);
        }
        else {
          out_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt;
        }
      }
    }
  }
}

// The same as the chase, except that we offset each strand by some integer amoutn.
void chase_spiral(uint8_t shell, uint8_t offset, boolean reverse) {
  // First do the inner cylider since it's smaller
  if (shell == 0) {
    for (uint8_t s = 0; s < in_strips; s++) {
      chaser(led_tmplt, strip_len, gIndex + offset * s, iPalette);
      if (s % 2 == 0) {
        if (reverse) {
          in_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt(strip_len-1, 0);
        }
        else {
          in_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          in_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt(strip_len -1, 0);
        }
        else  {
          in_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt;
        }
      }
    }
  }

  // Next do the outer cylinder
  if (shell == 1) {
    for (uint8_t s = 0; s < out_strips; s++) {
      chaser(led_tmplt, strip_len, gIndex + offset * s, oPalette);
      if (s % 2 == 0) {
        if (reverse) {
          out_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt(strip_len -1 , 0);
        }
        else {
          out_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
      else {
        if(reverse) {
          out_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt(strip_len -1, 0);
        }
        else {
          out_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt;
        }
      }
    }
  }
}

// Sends whole strands around the two shells. Can either be run as a solid
// band or have it fade as it goes around like a tracer
void shell_wrap(uint8_t shell, uint8_t fade_opt, boolean reverse) {
  // We'll have to use a separate counter for these than the global index
  static uint8_t in_pos = 0;
  static uint8_t out_pos = 0;
  static uint8_t in_pal_index = 0;
  static uint8_t out_pal_index = 0;
  static uint8_t in_fade_length = in_strips;
  static uint8_t out_fade_length = out_strips;
  static boolean in_fade_rev = false;
  static boolean out_fade_rev = false;

  // Update the positions of both the strips and reset the array
  if(shell == 0) {
    in_pos = (in_pos + 1) % in_strips;
  }
  else if (shell == 1) {
    out_pos = (out_pos + 1) % out_strips;
  }

  // Update the palette index whenever one of the shells makes a full revolution
  if (out_pos == 0) {
    out_pal_index += 12;
  }
  if (in_pos == 0) {
    in_pal_index +=  12;
  }

  // If fade_out is turned on, the strips will leave tracers so that it goes from full brightness to full
  // dim around the whole shell
  if (fade_opt == 0) {
    // Inner shell
    if (shell == 0) {
      for (uint8_t ss = 0; ss < in_strips; ss++) {
        fill_solid(led_tmplt, strip_len, ColorFromPalette(iPalette, in_pal_index, maxBrightness * (in_strips - ss + 1) / (double)(in_strips + 1)));
        if(reverse) {
          in_leds(strip_len * ((in_pos + (in_strips-ss-1)) % in_strips), strip_len * (((in_pos + (in_strips-ss-1)) % in_strips) + 1) - 1) = led_tmplt;
        }
        else {
          in_leds(strip_len * ((in_pos + ss) % in_strips), strip_len * (((in_pos + ss) % in_strips) + 1) - 1) = led_tmplt;
        }
      }
    }
    // Outer shell
    else if (shell == 1) {
      for (uint8_t ss = 0; ss < out_strips; ss++ ) {
        fill_solid(led_tmplt, strip_len, ColorFromPalette(oPalette, out_pal_index, maxBrightness * (out_strips - ss + 1) / (double)(out_strips + 1)));
        if(reverse) {
          out_leds(strip_len * ((out_pos + (out_strips-ss-1)) % out_strips), strip_len * (((out_pos + (out_strips-ss-1)) % out_strips) + 1) - 1) = led_tmplt;
        }
        else {
          out_leds(strip_len * ((out_pos + ss) % out_strips), strip_len * (((out_pos + ss) % out_strips) + 1) - 1) = led_tmplt;
        }
      }
    }
  }
  // Like the one above, except that the fade length varies with time
  else if(fade_opt == 1) {
    // Inner shell
    if (shell == 0) {
      // First draw the  animation, same method as fade_opt == 0
      for (uint8_t ss = 0; ss < in_fade_length; ss++) {
        fill_solid(led_tmplt, strip_len, ColorFromPalette(iPalette, in_pal_index, maxBrightness * (in_fade_length - ss + 1) / (double)(in_fade_length + 1)));
        if(reverse) {
          in_leds(strip_len * ((in_pos + (in_strips-ss-1)) % in_strips), strip_len * (((in_pos + (in_strips-ss-1)) % in_strips) + 1) - 1) = led_tmplt;
        }
        else {
          in_leds(strip_len * ((in_pos + ss) % in_strips), strip_len * (((in_pos + ss) % in_strips) + 1) - 1) = led_tmplt;
        }
      }
      // Change the fade length according to direction of travel
      if(!in_fade_rev) {
        in_fade_length--;
      }
      else {
        in_fade_length++;
      }
      // Check if we are at one of the bounds of length
      if (in_fade_length == 0 && !in_fade_rev) {
        in_fade_rev = true;
        in_fade_length += 2;
      }
      if (in_fade_length == in_strips && in_fade_rev) {
        in_fade_rev = false;
        in_fade_length -= 2;
      }
    }
    // Outer Shell
    if (shell == 1) {
      // First draw the  animation, same method as fade_opt == 0
      for (uint8_t ss = 0; ss < in_fade_length; ss++) {
        fill_solid(led_tmplt, strip_len, ColorFromPalette(oPalette, out_pal_index, maxBrightness * (out_fade_length - ss + 1) / (double)(out_fade_length + 1)));
        if(reverse) {
          out_leds(strip_len * ((out_pos + (out_strips-ss-1)) % out_strips), strip_len * (((out_pos + (out_strips-ss-1)) % out_strips) + 1) - 1) = led_tmplt;
        }
        else {
          out_leds(strip_len * ((out_pos + ss) % out_strips), strip_len * (((out_pos + ss) % out_strips) + 1) - 1) = led_tmplt;
        }
      }
      // Change the fade length according to direction of travel
      if(!out_fade_rev) {
        out_fade_length--;
      }
      else {
        out_fade_length++;
      }
      // Check if we are at one of the bounds of length
      if (out_fade_length == 0 && !out_fade_rev) {
        out_fade_rev = true;
        out_fade_length += 2;
      }
      if (out_fade_length == out_strips && out_fade_rev) {
        out_fade_rev = false;
        out_fade_length -= 2;
      }
    }
  }
  // This one does not fade out, but just overwrites the previous rotations color
  else if(fade_opt == 2) {
    // Inner shell
    if(shell == 0) {
      fill_solid(led_tmplt, strip_len, ColorFromPalette(iPalette, in_pal_index, maxBrightness, gBlending));
      if(reverse) {
        in_leds(strip_len * (in_strips - in_pos -1), strip_len * (in_strips - in_pos) - 1) = led_tmplt;
      }
      else {
        in_leds(strip_len * in_pos, strip_len * (in_pos + 1) - 1) = led_tmplt;
      }
    }
    // Outer shell
    else if(shell == 1) {
      fill_solid(led_tmplt, strip_len, ColorFromPalette(oPalette, out_pal_index, maxBrightness, gBlending));
      if(reverse) {
        out_leds((out_strips - out_pos - 1) * strip_len, strip_len * (out_strips - out_pos) - 1) = led_tmplt;
      }
      else {
        out_leds(out_pos * strip_len, strip_len * (out_pos + 1) - 1) = led_tmplt;
      }
    }
  }

  // This just draws one strip at a time, similar to the one above it except that we clear it before each redraw.
  else if (fade_opt == 3) {
    // Inner shell
    if (shell == 0) {
      clear_in();
      fill_solid(led_tmplt, strip_len, ColorFromPalette(iPalette, in_pal_index, maxBrightness, gBlending));
      in_leds(strip_len*in_pos, strip_len*(in_pos+1) - 1) = led_tmplt;
    }
    // Outer shell, do this a few strips at a time since there's more of them
    else if(shell == 1) {
      clear_out();
      fill_solid(led_tmplt, strip_len, ColorFromPalette(oPalette, out_pal_index, maxBrightness, gBlending));
      for (uint8_t ss = 0; ss < 3; ss++) {
        out_leds(((out_pos + ss) % out_strips) * strip_len, strip_len * (((out_pos + ss) % out_strips) + 1) - 1) = led_tmplt;
      }
    }
  }

}
