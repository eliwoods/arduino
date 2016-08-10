// Sends chasers all going one direction along the chosen shell
// Reverse is just relative to the the non reversed direction, not
// sure which way it physically 
void chase_straight(uint8_t strip, boolean reverse) {
  // Fill the template array first
  chaser(led_tmplt, strip_len, gIndex);

  // Now fill both the inner template
  if (strip == 0) {
    for (uint8_t s = 0; s < in_strips; s++) {
      if (s % 2 == 0) {
        if (reverse) {
          in_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt(strand_len - 1, 0);
        }
        else {
          in_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          in_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt(strand_len - 1, 0);
        }
        else {
          in_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt;
        }
      }
    }
  }
  // Fill outer template
  else if (strip == 1) {
    for (uint8_t s = 0; s < out_strips; s++) {
      if (s % 2 == 0) {
        if (reverse) {
          out_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt(strip_len - 1, 0);
        }
        else {
          out_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          out_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt(strip_len - 1, 0);
        }
        else {
          out_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
    }
  }
}

void chase_spiral(uint8_t shell, uint8_t offset, boolean reverse) {
  // First do the inner cylider since it's smaller
  if (shell == 0) {
    for (uint8_t s = 0; s < in_strips; s++) {
      chaser(led_tmplt, strip_len, gIndex + offset * s);
      if (s % 2 == 0) {
        if (reverse) {
          in_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt(strand_len-1, 0);
        }
        else {
          in_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          in_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt(strand_len -1, 0);
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
      chaser(led_tmplt, strip_len, gIndex + offset * s);
      if (s % 2 == 0) {
        if(reverse) {
          out_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt(strand_len -1, 0);
        }
        else {
          out_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt;
        }
      }
      else {
        if (reverse) {
          out_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt(strand_len -1 , 0);
        }
        else {
          out_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }
    }
  }
}

// Send rings of x width bouncing in opposite directions
void ring_bounce_opp(uint16_t del, uint8_t width) {
  // Quick error check
  if (width < 1) {
    width = 1;
  }

  // Let's use some of our own indexing for this function instead of the
  // global since we care a little more about it's position.
  static uint8_t strip_index = 0;
  static uint8_t pal_index = 0;

  // Some boolean flags to let us know which direction we're headinging
  static boolean _rev = false;

  // First draw the two shells
  draw_circle(strip_index, width, ColorFromPalette(gPalette, pal_index, maxBrightness, gBlending), false);
  draw_circle(strip_len - strip_index, width, ColorFromPalette(gPalette, pal_index, maxBrightness, gBlending), true);
  LEDS.show();

  // Now update the index depending on direction
  EVERY_N_MILLISECONDS(del) {
    if (!_rev) {
      strip_index++;
    }
    else {
      strip_index--;
    }
    clear_all(); // Gotta clear it so the circles remain separate
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
    pal_index += 12; // Try this here since it's technically the starting position
  }

}

// Sends whole strands around the two strands. Can either be run as a solid
// band or have it fade as it goes around like a tracer
void shell_wrap(uint16_t del, boolean fade_out) {
  // We'll have to use a separate counter for these than the global index
  static uint8_t in_pos = 0;
  static uint8_t out_pos = 0;
  static uint8_t in_pal_index = 0;
  static uint8_t out_pal_index = 0;

  // Update the positions of both the strips and reset the array
  EVERY_N_MILLISECONDS(del) {
    in_pos = (in_pos + 1) % in_strips;
    out_pos = (out_pos + 1) % out_strips;
    clear_all();
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
  if (fade_out) {
    // Fill the inner perimeter
    for (uint8_t ss = 0; ss < in_strips; ss++) {
      fill_solid(led_tmplt, strip_len, ColorFromPalette(gPalette, in_pal_index, maxBrightness * (in_strips - ss + 1) / (double)(in_strips + 1)));
      in_leds(strip_len * ((in_pos + ss) % in_strips), strip_len * (((in_pos + ss) % in_strips) + 1) - 1) = led_tmplt;
    }
    // Fill the outer perimeter
    for (uint8_t ss = 0; ss < out_strips; ss++ ) {
      fill_solid(led_tmplt, strip_len, ColorFromPalette(gPalette, out_pal_index, maxBrightness * (out_strips - ss + 1) / (double)(out_strips + 1)));
      out_leds(strip_len * ((out_pos + ss) % out_strips), strip_len * (((out_pos + ss) % out_strips) + 1) - 1) = led_tmplt;
    }
  }
  else {
    // Outer, try filling a few of the strips
    fill_solid(led_tmplt, strip_len, ColorFromPalette(gPalette, out_pal_index, maxBrightness, gBlending));
    for (uint8_t ss = 0; ss < 3; ss++) {
      out_leds(((out_pos + ss) % out_strips) * strip_len, strip_len * (((out_pos + ss) % out_strips) + 1) - 1) = led_tmplt;
    }

    // Inner, lets only fill one
    fill_solid(led_tmplt, strip_len, ColorFromPalette(gPalette, in_pal_index, maxBrightness, gBlending));
    in_leds(strip_len * in_pos, strip_len * (in_pos + 1) - 1) = led_tmplt;

  }
  copy_pasta_dump();
}

