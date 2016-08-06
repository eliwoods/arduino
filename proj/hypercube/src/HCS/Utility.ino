// Copies the inner and outer led templates to the main array and
// pushes them to the LEDs
void copy_pasta_dump() {
  leds(0, in_LED_tot - 1) = in_leds;
  leds(in_LED_tot, led_tot - 1) = out_leds;
  LEDS.show();
}

// Just resets the color of all the leds, doesn't push it though since
// we'll usually do something with them right after calling this.
void clear_all() {
  fill_solid(leds, led_tot, CRGB::Black);
}

