
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


