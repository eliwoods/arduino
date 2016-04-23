// Just to clarify comments, I'm describing the palette pattern
// in terms of the palettes index. Or it'll be copied from the 
// PaletteKnife shit
//
// W == White
// B == Black
// RB == Rainbow, uses global hue position
// X == Input to the function


// RB|B|B|B repeating
void update_RainbowBlack_p() {
  // Instead of blacking out, fill backgroud with rainbow, then
  // fill every thing with black except
  fill_rainbow(gPalette, 16, gHue, 10);
  for(uint8_t i =0 ; i< 16; i++) {
    if( i%4 == 0) {
      continue;
    }
    gPalette[i] = CRGB::Black;
  }
}

// W|RB|RB|RB repeating
void update_WhiteRainbow_p() {
  fill_rainbow(gPalette, 16, gHue, 10);
  gPalette[0] = CRGB::White;
  gPalette[4] = CRGB::White;
  gPalette[8] = CRGB::White;
  gPalette[12] = CRGB::White;
}

// Modulate 
//void update_RainbowBlack

// W|X|X|X repeating
void update_WhiteCol_p(CRGB col) {
  fill_solid( gPalette, 16, col);
  gPalette[0] = CRGB::Gray;
  gPalette[4] = CRGB::Gray;
  gPalette[8] = CRGB::Gray;
  gPalette[12] = CRGB::Gray;
}

// W|B|B|B repeating
const TProgmemPalette16 BlackWhite_p PROGMEM = {
  CRGB::White,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
};

