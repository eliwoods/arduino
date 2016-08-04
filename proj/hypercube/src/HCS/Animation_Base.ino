// Simple theater chase where packets move continuously
void theater_chase(CRGB *leds, uint16_t numLED, uint8_t pal_index) {
  fill_palette(leds, numLED, pal_index, 4, gPalette, gBrightness, gBlending);
}
