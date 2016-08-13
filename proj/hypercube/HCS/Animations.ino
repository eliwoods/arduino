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
  static uint8_t in_pal_index = 0;
  static uint8_t out_pal_index = 0;

  // Some boolean flags to let us know which direction we're headinging
  static boolean _rev = false;

  // First draw the two shells
  draw_circle(strip_index, width, ColorFromPalette(iPalette, in_pal_index, maxBrightness, gBlending), INNER);
  draw_circle(strip_len - strip_index, width, ColorFromPalette(oPalette, out_pal_index, maxBrightness, gBlending), OUTER);

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
    in_pal_index += 12; // Try this here since it's technically the starting position
    out_pal_index += 12; 
    while(is_black(ColorFromPalette(oPalette, out_pal_index, maxBrightness, gBlending))) {
      out_pal_index += 12;
    }
    while(is_black(ColorFromPalette(oPalette, in_pal_index, maxBrightness, gBlending))) {
      in_pal_index += 12;
    }
  }

}


// Lets try
void helix_spiral_overlay(uint16_t del, uint16_t width) {
  // Quick error check
  if(width < 1 ) {
    width = 1 ;
  }
  static uint16_t index = 0;

  static uint8_t in_pal_index = 0;
  static uint8_t out_pal_index = 0;

  EVERY_N_MILLISECONDS(del) {
    index = (index+1)%strip_len;
  }

  helix_overlay(index, width, 16, ColorFromPalette(iPalette, in_pal_index, maxBrightness, gBlending), INNER, false);
  helix_overlay(index, width, 16, ColorFromPalette(oPalette, out_pal_index, maxBrightness, gBlending), OUTER, true);

  if(index == strip_len -1) {
    in_pal_index += 12;
    out_pal_index += 12;
    while(is_black(ColorFromPalette(iPalette, in_pal_index, maxBrightness))) {
      in_pal_index += 12;
    }
    while(is_black(ColorFromPalette(oPalette, out_pal_index, maxBrightness))) {
      out_pal_index += 12;
    }
  }
}
