// Draw a circle, the index goes from 0->strand_len which represents the bottom led
// on a strand to the top LED. The third argument is a flag that chooses which cylinder
// to draw to, false -> inner and true -> outer. For this, we'll assume that the index
// represents the top pixel of a given circle's width.
void draw_circle(uint16_t index, uint16_t width, CRGB col, uint16_t shell) {
  // A quick error check
  if (width < 1) {
    width = 1;
  }


  // Draw the circle by just setting the exact pixels to the color. This will let us
  // draw over other patterns without completely overwriting the template
  if (shell == 0) {
    for (uint16_t ss = 0; ss < in_strips; ss++) {
      if (ss % 2 == 0) {
        for (uint16_t cc = 0; cc < width; cc++) {
          // Check that we don't go out of bounds for the strip
          if (ss * strip_len + index + cc <= (ss + 1)*strip_len - 1) {
            leds[ss * strip_len + index + cc] = col;
          }
        }
      } // End even strip loop
      else {
        for (uint16_t cc = 0; cc < width; cc++) {
          // Check for bounds
          if ((ss + 1)*strip_len - 1 - (index+cc) >= ss * strip_len) {
            leds[(ss + 1)*strip_len - 1 - (index+cc)] = col;
          }
        }
      } // End odd strip loop
    } // End loop over strips
  } // End of inner shell 

  else if (shell == 1) {
    for (uint16_t ss = 0; ss < out_strips; ss++) {
      if (ss % 2 == 0) {
        // For variable circle width
        for (uint16_t cc = 0; cc < width; cc++) {
          // Check that we don't go out of bounds for the strip
          if (ss * strip_len + index + cc <= (ss + 1)*strip_len - 1) {
            leds[in_LED_tot + ss * strip_len + index + cc] = col;
          }
        }
      } // End even strip loop
      else {
        // For variable circle width
        for (uint16_t cc = 0; cc < width; cc++) {
          // Check for bounds
          if ((ss + 1)*strip_len - 1 - (index + cc) >= ss * strip_len) {
            leds[in_LED_tot + (ss + 1)*strip_len - 1 - (index+cc)] = col;
          }
        }
      } // End odd strip loop
    } // End loop over strips
  } // End of outer shell
}

// Send rings of x width bouncing in opposite directions. Needs to have clear_all() run
// before it does, or at least another animation that totally overwrites what's going on.
// Breaking these into two different functions so it's not such a monstrosity.
void ring_bounce_overlay(uint8_t width) {
  // Quick error check
  if (width < 1) {
    width = 1;
  }

  // Let's use some of our own indexing for this function instead of the
  // global since we care a little more about it's position.
  static uint8_t strip_index = 0;

  // Some boolean flags to let us know which direction we're headinging
  static boolean _rev = false;

  // First draw the two shells
  draw_circle(strip_index, width, CRGB::Black, INNER);
  draw_circle(strip_len - strip_index, width, CRGB::Black, OUTER);

  // Now update the index depending on direction
  EVERY_N_MILLISECONDS_I(thisTimer, 20) {
    thisTimer.setPeriod(gRate);

    if (!_rev) {
      strip_index++;
    }
    else {
      strip_index--;
    }
  }

  // Check if we're at one of the extremes of a strip. Move the index and flip the flag
  // according to which extreme we're at
  if (strip_index == strip_len && !_rev) {
    strip_index -= 2;
    _rev = true;
  }
  if (strip_index == 0 && _rev) {
    strip_index++;
    _rev = false;
  }
}


// Lets try
void helix_spiral_overlay(uint16_t width, uint16_t offset) {
  // Quick error check
  if(width < 1 ) {
    width = 1 ;
  }
  static uint16_t index = 0;

  EVERY_N_MILLISECONDS_I(thisTimer, 10) {
    thisTimer.setPeriod(gRate);
    index = (index+1)%strip_len;
  }

  //// Inner shell
  //if (shell == 0) {
  //  for (uint16_t ss = 0; ss < in_strips; ss++) {
  //    if (ss % 2 == 0) {
  //      // For variable circle width
  //      for (uint16_t cc = 0; cc < width; cc++) {
  //        // Check that we don't go out of bounds for the strip
  //        if (ss*strip_len + index + cc + ss*offset <= (ss + 1)*strip_len - 1) {
  //          leds[ss * strip_len + index + cc + ss*offset] = CRGB::Black;
  //          leds[(ss + 1)*strip_len - 1 - (index+cc + ss*offset)] = CRGB::Black;
  //        }
  //      }
  //    } // End even strip loop
  //    else {
  //      for (uint16_t cc = 0; cc < width; cc++) {
  //        if ((ss+1)*strip_len - 1 - (index + cc + ss*offset) >= ss * strip_len) {
  //          leds[(ss + 1)*strip_len - 1 - (index+cc + ss*offset)] = CRGB::Black;
  //          leds[ss * strip_len + index + cc + ss*offset] = CRGB::Black;
  //        }
  //      }
  //    } 
  //  } // End loop over strips
  //}
  //if (shell == 1) {
  for (uint16_t ss = 0; ss < out_strips; ss++) {
    if (ss % 2 == 0) {
      // For variable circle width
      for (uint16_t cc = 0; cc < width; cc++) {
        // Check that we don't go out of bounds for the strip
        if (ss*strip_len + index + cc + ss*offset <= (ss + 1)*strip_len - 1) {
          leds[in_LED_tot + ss * strip_len + index + cc + ss*offset] = CRGB::Black;
          leds[in_LED_tot + (ss + 1)*strip_len - 1 - (index+cc + ss*offset)] = CRGB::Black;
        }
      }
    } // End even strip loop
    else {
      for (uint16_t cc = 0; cc < width; cc++) {
        if ((ss+1)*strip_len - 1 - (index + cc + ss*offset) >= ss * strip_len) {
          leds[in_LED_tot + ss * strip_len + index + cc + ss*offset] = CRGB::Black;
          leds[in_LED_tot + (ss + 1)*strip_len - 1 - (index+cc + ss*offset)] = CRGB::Black;
        }
      }
    } 
  } // End loop over strips
  //}
}

void bar_wrap_overlay(uint16_t width) {
  // Error check
  if(width < 1) {
    width = 1;
  }
  
  static uint16_t index = 0;
  static boolean reverse = false;

  EVERY_N_MILLISECONDS_I(thisTimer, 30) {
    thisTimer.setPeriod(gRate);

    if(reverse) {
      if(index == 0) {
        index = out_strips-1;
      }
      else {
        index = (index-1);
      }
    }
    else {
      index = (index+1)%out_strips;
    }
  }

  fill_solid(led_tmplt, strip_len, CRGB::Black);
  
  // If anything is fucked up about this animation its right here vvvvv
  for(uint8_t ww = 0; ww < width; ww++) {
    leds(in_LED_tot + ((index + ww) % out_strips) * strip_len, in_LED_tot + strip_len * (((index + ww) % out_strips) + 1) - 1) = led_tmplt;
    //if(index+ww == out_strips-1) {
    //  leds(in_LED_tot + (index+ww)*strip_len, led_tot-1) = led_tmplt;
    //}
    //else if (index+ww >= out_strips) {
    //  leds(in_LED_tot + ((index+ww)%out_strips)*strip_len, in_LED_tot + ((index+ww+1)%out_strips)*strip_len -1 ) = led_tmplt;
    //}
    //else {
    //  leds(in_LED_tot + (index+ww)*strip_len, in_LED_tot + (index+ww+1)*strip_len - 1) = led_tmplt;
    //}
  }

}

void overlay_snow(uint16_t del, double density) {
  // Error check
  if(density >= 1) {
    density = 0.5;
  }

  static double rnd;
  EVERY_N_MILLISECONDS(del) {
    for(uint16_t pxl = 0; pxl < led_tot; pxl++) {
      rnd = random16(1001)/1000.;
      if(rnd < density) {
        leds[pxl] = CHSV(255, 0, random8(maxBrightness));
      }
    }
  }

}

void randomize_combinations() {
  // Randomize Palettes
  iPaletteCounter = random8(numPalStyles-1);
  oPaletteCounter = random8(numPalStyles);
  gPaletteCounter = random8(numPalettes);

  // Randomize Animations
  iAnimCounter = random8(numAnimation);
  oAnimCounter = random8(numAnimation);

  // Also reset all of the overlay flags (including the BRB)
  do_not_pressed = false;
  piezo0_flicked = false;
  piezo1_flicked = false;
  laser0_on = false;
  laser1_on = false;
  laser2_on = false;
  laser3_on = false;

  // Set all the flags for switching animations and palettes to
  // false incase we were in the middle of one. Don't want to get caught in some
  // fucked up transition
  iAnimSwitchCount = 0;
  oAnimSwitchCount = 0;
  iAnimSwitch = false;
  oAnimSwitch = false;
}

void do_not_press_response() {
  do_not_pressed = false;
  
  // First flash the whole thing red, alternating between inner and outer shells
  for(uint8_t itr = 0; itr < 4; itr++) {
    if (itr %2 == 0) {
      fill_solid(in_leds, in_LED_tot, CRGB::Red);
    }
    else {
      fill_solid(out_leds, out_LED_tot, CRGB::Red);
    }
    merge_animations();
    clear_all();
    LEDS.delay(200);
  }

  // Next send the chaser mod going opposite directions. Stop it once it gets to the
  // maximum length between packets
  uint8_t temp_index = 0;
  while(temp_index < 150) {
    update_RedBlack_p(INNER);
    update_RedBlack_p(OUTER);
    chaser_mod(leds, led_tot, temp_index, iPalette);
    temp_index++;
    LEDS.show();
    LEDS.delay(20);
  }

  // Now randomize and resume the show  
  randomize_combinations();
  merge_animations();
}
