// Send rings of x width bouncing in opposite directions. Needs to have clear_all() run
// before it does, or at least another animation that totally overwrites what's going on
void ring_bounce_opp(uint16_t del, uint8_t width) {
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
  EVERY_N_MILLISECONDS(del) {
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
void helix_spiral_overlay(uint16_t del, uint16_t width) {
  // Quick error check
  if(width < 1 ) {
    width = 1 ;
  }
  static uint16_t index = 0;

  EVERY_N_MILLISECONDS(del) {
    index = (index+1)%strip_len;
  }

  helix_overlay(index, width, 16, CRGB::Black, OUTER, true);
}

void bar_wrap_overlay(uint16_t del, uint16_t width, boolean reverse) {
  // Error check
  if(width < 1) {
    width = 1;
  }
  
  static uint16_t index = 0;

  EVERY_N_MILLISECONDS(del) {
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
  
  for(uint8_t ww = 0; ww < width; ww++) {
    leds(in_LED_tot + ((index+ww)%out_strips)*strip_len, in_LED_tot + ((index+ww+1)%out_strips)*strip_len -1 ) = led_tmplt;
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
        leds[pxl] = CHSV(255, 0, random8());
      }
    }
  }

}

void randomize_combinations() {
  // Randomize Palettes
  iPaletteCounter = random8(numPalettes);
  oPaletteCounter = random8(numPalettes);

  // Randomize Animations
  iAnimCounter = random8(iNumAnimation);
  oAnimCounter = random8(oNumAnimation);
}

void do_not_press_response() {
  // First flash the whole thing red, alternating between inner and outer shells
  for(uint8_t itr = 0; itr < 14; itr++) {
    if (itr %2 == 0) {
      fill_solid(in_leds, in_LED_tot, CHSV(0, 255, maxBrightness));
    }
    else {
      fill_solid(out_leds, out_LED_tot, CHSV(0, 255, maxBrightness));
    }
    merge_animations();
    clear_all();
    LEDS.delay(75);
  }

  // Next send the chaser mod going opposite directions. Stop it once it gets to the
  // maximum length between packets
  uint8_t temp_index = 0;
  while(temp_index < 150) {
    update_RedBlack_p(INNER);
    chaser_mod(leds, led_tot, temp_index, iPalette);
    temp_index++;
    LEDS.show();
    LEDS.delay(20);
  }

  // Now randomize and resume the show  
  randomize_combinations();
}
