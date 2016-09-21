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

boolean is_black(CRGB col) {
  if(col.r == 0 && col.g == 0 && col.b == 0) {
    return true;
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////////////
// These are really simple animations, that will be layered to make more intereseting //
// configurations.                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

// Simple theater chase where packets move continuously
void chaser(CRGB *leds, uint16_t numLED, uint16_t pal_index, CRGBPalette16 palette) {
  fill_palette(leds, numLED, pal_index, 4, palette, gBrightness, gBlending);
}

// A glitchy style "chase". It not even really a chase, I just keep the palette static and
// change the width of the packets. Looks kind of funky, but kind of cool too.
void chaser_mod(CRGB *leds, uint16_t numLED, uint16_t col_inc, CRGBPalette16 palette) {
  fill_palette(leds, numLED, 0, col_inc, palette, gBrightness, gBlending);
}

// Sends chasers all going one direction along the chosen shell
// Reverse is just relative to the the non reversed direction, not
// sure which way it physically 
void chase_straight(uint16_t shell, boolean reverse) {
  // Control the index locally
  static uint8_t index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 10) {
    thisTimer.setPeriod(gRate);
    index++;
  }

  // Now fill both the inner template
  if (shell == 0) {
    chaser(led_tmplt, strip_len, index, iPalette);
    for (uint16_t s = 0; s < in_strips; s++) {
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
    chaser(led_tmplt, strip_len, index, oPalette);
    for (uint16_t s = 0; s < out_strips; s++) {
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

void chase_mod(uint16_t shell, boolean reverse) {
  static uint8_t index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 10) {
    thisTimer.setPeriod(gRate);
    index++;
  }

  if(shell == INNER) {
    chaser_mod(led_tmplt, strip_len, index, iPalette);
    for (uint16_t s = 0; s < out_strips; s++) {
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
  if(shell == OUTER) {
    chaser_mod(led_tmplt, strip_len, index, oPalette);
    for (uint16_t s = 0; s < out_strips; s++) {
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
void chase_spiral_static(uint16_t shell, uint16_t offset, boolean reverse) {
  // Local index control
  static uint8_t index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 10) {
    thisTimer.setPeriod(gRate);
    index++;
  }

  // First do the inner cylider since it's smaller
  if (shell == 0) {
    for (uint16_t s = 0; s < in_strips; s++) {
      chaser(led_tmplt, strip_len, index + offset * s, iPalette);
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
    for (uint16_t s = 0; s < out_strips; s++) {
      chaser(led_tmplt, strip_len, index + offset * s, oPalette);
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

// The same as the chase, except that we offset each strand by some integer amoutn.
void chase_spiral_mod(uint16_t shell, boolean reverse) {
  // Local index control
  static uint8_t index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 10) {
    thisTimer.setPeriod(gRate);
    index++;
  }

  static uint8_t iOffset = 0;
  static uint8_t oOffset = 0;
  EVERY_N_MILLISECONDS(100) {
    if(shell == INNER) {
      iOffset++;
    }
    if(shell == OUTER) {
      oOffset++;
    }
  }

  // First do the inner cylider since it's smaller
  if (shell == INNER) {
    for (uint16_t s = 0; s < in_strips; s++) {
      chaser(led_tmplt, strip_len, index + iOffset * s, iPalette);
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
  if (shell == OUTER) {
    for (uint16_t s = 0; s < out_strips; s++) {
      chaser(led_tmplt, strip_len, index + oOffset * s, oPalette);
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

// Like the theater chase, except that we mirror it at the center of the strip. This when combined
// with an offset will give us a sick helix motion
void chase_helix(uint16_t shell, uint16_t offset, boolean reverse) {
  // Local index control
  static uint8_t index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 10) {
    thisTimer.setPeriod(gRate);
    index++;
  }

  // Inner shell
  if (shell == 0) {
    for (uint16_t ss = 0; ss < in_strips; ss++) {
      chaser(led_tmplt, strip_len, index + offset*ss, iPalette);
      if (ss % 2 == 0) {
        if(reverse) {
          in_leds(strip_len*ss, strip_len*ss/2 - 1) = led_tmplt(strip_len/2 - 1, 0);
          in_leds(ss*strip_len/2, strip_len*(ss+1) - 1) = led_tmplt(0, strip_len/2 -1);
        }
        else {
          in_leds(ss*strip_len, ss*strip_len/2 - 1) = led_tmplt(0, strip_len/2 - 1);
          in_leds(ss*strip_len/2, strip_len*(ss+1) -1 ) = led_tmplt(strip_len/2 -1, 0);
        }
      }
      else {
        if(reverse) {
          in_leds(strip_len*ss/2 - 1, strip_len*ss) = led_tmplt(strip_len/2-1, 0);
          in_leds(strip_len*(ss+1)-1, ss*strip_len/2) = led_tmplt(0, strip_len/2 -1);
        }
        else {
          in_leds(strip_len*ss/2 - 1, strip_len*ss) = led_tmplt(0, strip_len/2 -1);
          in_leds(strip_len*(ss+1)-1, ss*strip_len/2) = led_tmplt(strip_len/2-1, 0);
        }
      }

    }
  }
  // Outer Shell
  if (shell == 1) {
    for (uint16_t ss = 0; ss < out_strips; ss++) {
      chaser(led_tmplt, strip_len, index + offset*ss, oPalette);
      if (ss % 2 == 0) {
        if(reverse) {
          out_leds(strip_len*ss, strip_len*ss/2 - 1) = led_tmplt(strip_len/2 - 1, 0);
          out_leds(ss*strip_len/2, strip_len*(ss+1) - 1) = led_tmplt(0, strip_len/2 -1);
        }
        else {
          out_leds(ss*strip_len, ss*strip_len/2 - 1) = led_tmplt(0, strip_len/2 - 1);
          out_leds(ss*strip_len/2, strip_len*(ss+1) -1 ) = led_tmplt(strip_len/2 -1, 0);
        }
      }
      else {
        if(reverse) {
          out_leds(strip_len*ss/2 - 1, strip_len*ss) = led_tmplt(strip_len/2-1, 0);
          out_leds(strip_len*(ss+1)-1, ss*strip_len/2) = led_tmplt(0, strip_len/2 -1);
        }
        else {
          out_leds(strip_len*ss/2 - 1, strip_len*ss) = led_tmplt(0, strip_len/2 -1);
          out_leds(strip_len*(ss+1)-1, ss*strip_len/2) = led_tmplt(strip_len/2-1, 0);
        }
      }

    }
  }
}

// Sends whole strands around the two shells. Can either be run as a solid
// band or have it fade as it goes around like a tracer
void shell_wrap(uint16_t shell, boolean reverse, uint8_t fade_opt) {
  // We'll have to use a separate counter for these than the global index
  static uint16_t in_pos = 0;
  static uint16_t out_pos = 0;
  static uint16_t in_pal_index = 0;
  static uint16_t out_pal_index = 0;
  static uint16_t in_fade_length = in_strips;
  static uint16_t out_fade_length = out_strips;
  static boolean in_fade_rev = false;
  static boolean out_fade_rev = false;
  static const double in_rate_increase = 1.2;
  static const double out_rate_increase = 3;

  // Update the positions of both the strips and reset the array
  if(shell == INNER) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(in_rate_increase*gRate);
      in_pos = (in_pos+1) % in_strips;
    }
  }
  if (shell == OUTER) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(out_rate_increase*gRate);
      out_pos = (out_pos+1) % out_strips;
    }
  }


  // Update the palette index whenever one of the shells makes a full revolution
  if (out_pos == 0) {
    out_pal_index += 12;
    if(!oAnimSwitch) {
      while(is_black(ColorFromPalette(oPalette, out_pal_index, gBrightness, gBlending))) {
        out_pal_index += 12;
      }
    }
  }
  if (in_pos == 0) {
    in_pal_index +=  12;
    if(!iAnimSwitch) {
      while(is_black(ColorFromPalette(iPalette, in_pal_index, gBrightness, gBlending))) {
        out_pal_index += 12;
      }
    }
  }

  // This wraps around the shell, and makes the wrap fade to black so that it looks like a tracer
  if (fade_opt == 0) {
    // Inner shell
    if (shell == INNER) {
      for (uint16_t ss = 0; ss < in_strips; ss++) {
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
    else if (shell == OUTER) {
      for (uint16_t ss = 0; ss < out_strips; ss++ ) {
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
    if (shell == INNER) {
      // First draw the  animation, same method as fade_opt == 0
      for (uint16_t ss = 0; ss < in_fade_length; ss++) {
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
    if (shell == OUTER) {
      // First draw the  animation, same method as fade_opt == 0
      for (uint16_t ss = 0; ss < in_fade_length; ss++) {
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
    if(shell == INNER) {
      fill_solid(led_tmplt, strip_len, ColorFromPalette(iPalette, in_pal_index, maxBrightness, gBlending));
      if(reverse) {
        in_leds(strip_len * (in_strips - in_pos -1), strip_len * (in_strips - in_pos) - 1) = led_tmplt;
      }
      else {
        in_leds(strip_len * in_pos, strip_len * (in_pos + 1) - 1) = led_tmplt;
      }
    }
    // Outer shell
    else if(shell == OUTER) {
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
    if (shell == INNER) {
      clear_in();
      fill_solid(led_tmplt, strip_len, ColorFromPalette(iPalette, in_pal_index, maxBrightness, gBlending));
      in_leds(strip_len*in_pos, strip_len*(in_pos+1) - 1) = led_tmplt;
    }
    // Outer shell, do this a few strips at a time since there's more of them
    else if(shell == OUTER) {
      clear_out();
      fill_solid(led_tmplt, strip_len, ColorFromPalette(oPalette, out_pal_index, maxBrightness, gBlending));
      for (uint16_t ss = 0; ss < 3; ss++) {
        out_leds(((out_pos + ss) % out_strips) * strip_len, strip_len * (((out_pos + ss) % out_strips) + 1) - 1) = led_tmplt;
      }
    }
  }
}

void ring_bounce_opp(uint8_t shell, uint8_t width) {
  // Quick error check
  if (width < 1) {
    width = 1;
  }

  // Let's use some of our own indexing for this function instead of the
  // global since we care a little more about it's position.
  static uint8_t strip_index = 0;

  // Some boolean flags to let us know which direction we're headinging
  static boolean _rev = false;

  // First draw the circles. Lets ignore the palette style here so
  // we can get a nice smooth color transition
  if(shell == INNER) {
    clear_in();
    //draw_circle(strip_index, width, ColorFromPalette(gPalette, gHue, maxBrightness, gBlending), INNER);
    draw_circle(strip_index, width, CRGB::Red, INNER);
  }
  if (shell == OUTER) {
    clear_out();
    //draw_circle(strip_len - strip_index, width, ColorFromPalette(gPalette, (gHue+128)%256, maxBrightness, gBlending), OUTER);
    draw_circle(strip_len - strip_index, width, CRGB::Red, OUTER);
  }

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

void snow_anim(uint8_t shell, double density) {
  // Error check
  if(density >= 1) {
    density = 0.5;
  }

  static double rnd;
  if(shell == INNER) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(gRate*5);
      for(uint16_t pxl = 0; pxl < in_LED_tot; pxl++) {
        rnd = random16(1001)/1000.;
        if(rnd < density) {
          in_leds[pxl] = CHSV(ColorFromPalette(gPalette, random8(), gBrightness, gBlending), 255, maxBrightness);
        }
        else {
          in_leds[pxl] = CRGB::Black;
        }
      }
    }
  }
  if(shell == OUTER) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(gRate*5);
      for(uint16_t pxl = 0; pxl < out_LED_tot; pxl++) {
        rnd = random16(1001)/1000.;
        if(rnd < density) {
          out_leds[pxl] = CHSV(ColorFromPalette(gPalette, random8(), gBrightness, gBlending), 255, maxBrightness);
        }
        else {
          out_leds[pxl] = CRGB::Black;
        }
      }
    }
  }

}

void fill_shell(uint8_t shell) {
  if(shell == INNER) {
    fill_solid(in_leds, in_LED_tot, ColorFromPalette(gPalette, gHue, maxBrightness*0.8, gBlending));
  }
  if (shell == OUTER) {
    fill_solid(out_leds, out_LED_tot, ColorFromPalette(gPalette, (gHue+128)%256, maxBrightness*0.8, gBlending));
  }
}
