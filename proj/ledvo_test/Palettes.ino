// Just to clarify comments, I'm describing the palette pattern
// in terms of the palettes index. Or it'll be copied from the
// PaletteKnife shit
//
// W == White
// B == Black
// RB == Rainbow, uses global hue position
// X == Input to the function


//////////////////////////////////////////////////////////////////////////////////
// These template need to be updated before each fill call. This is because     //
// they are either dependend on an input or they rely on some global variable   //
// that is changed in the animations.                                           //
//////////////////////////////////////////////////////////////////////////////////


// RB|B|B|B repeating
void update_RainbowBlack_p() {
  // Instead of blacking out, fill backgroud with rainbow, then
  // fill every thing with black except
  fill_rainbow(gPalette, 16, gHue, 10);
  for (uint8_t i = 0 ; i < 16; i++) {
    if ( i % 4 == 0) {
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

// W|X|X|X repeating
void update_WhiteCol_p() {
  fill_solid( gPalette, 16, gRGB);
  gPalette[0] = CRGB::Gray;
  gPalette[4] = CRGB::Gray;
  gPalette[8] = CRGB::Gray;
  gPalette[12] = CRGB::Gray;
}

// X_LEAD|X_BACK|X_BACK|X_BACK repeating
void update_ColCol_p() {
//  fill_solid(gPalette, 8, gRGB);
//  // Fill second color halfway up the hue scale from the first one.
//  fill_solid(gPalette+8, 8, CHSV((map(analogRead(C_POT), 0, 1253, 0, 255)+128)%255, 255, maxBrightness));
  for (uint8_t i = 0; i < 16; i++) {
    if (i%2 == 0) {
      gPalette[i] = gRGB;
    }
    else {
      gPalette[i] = CHSV((map(analogRead(HUE_POT), 0, 1253, 0, 255)+128)%255, 255, maxBrightness);
    }
  }
}

// One color at the front and nothing else
void update_ColLead_p() {
  fill_solid(gPalette, 16, CRGB::Black);
  for (uint8_t i = 0; i < 4; i++) {
    gPalette[i] = gRGB;
  }
}
//////////////////////////////////////////////////////////////////////////////////
// Static templates. These are templates that do not need to be update          //
// since they are not modular and don't have any dependence on global variables //
//////////////////////////////////////////////////////////////////////////////////

// W|B|B|B repeating
const TProgmemPalette16 WhiteBlack_p PROGMEM = {
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
// Gradient palette "chroma_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/fractint/base/tn/chroma.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 2840 bytes of program space.

DEFINE_GRADIENT_PALETTE( chroma_gp ) {
  0,   4,  6,  5,
  0,   4,  6,  5,
  0,   7,  6,  8,
  1,   7,  6,  8,
  1,   9,  6, 10,
  2,   9,  6, 10,
  2,  12,  6, 14,
  2,  12,  6, 14,
  2,  16,  6, 17,
  3,  16,  6, 17,
  3,  20,  6, 22,
  4,  20,  6, 22,
  4,  24,  6, 27,
  5,  24,  6, 27,
  5,  30,  6, 32,
  5,  30,  6, 32,
  5,  35,  6, 38,
  6,  35,  6, 38,
  6,  42,  6, 45,
  7,  42,  6, 45,
  7,  49,  6, 52,
  7,  49,  6, 52,
  7,  57,  6, 61,
  8,  57,  6, 61,
  8,  66,  6, 69,
  9,  66,  6, 69,
  9,  75,  6, 79,
  10,  75,  6, 79,
  10,  86,  6, 89,
  10,  86,  6, 89,
  10,  97,  6, 100,
  11,  97,  6, 100,
  11, 109,  6, 112,
  12, 109,  6, 112,
  12, 121,  6, 125,
  12, 121,  6, 125,
  12, 135, 37, 182,  9,  3,
  237, 182,  9,  3,
  237, 182,  6,  3,
  238, 182,  6,  3,
  238, 165,  6,  3,
  239, 165,  6,  3,
  239, 150,  6,  3,
  239, 150,  6,  3,
  239, 135,  6,  3,
  240, 135,  6,  3,
  240, 121,  6,  3,
  241, 121,  6,  3,
  241, 109,  6,  3,
  242, 109,  6,  3,
  242,  97,  6,  3,
  242,  97,  6,  3,
  242,  86,  6,  3,
  243,  86,  6,  3,
  243,  75,  6,  3,
  244,  75,  6,  3,
  244,  66,  6,  3,
  244,  66,  6,  3,
  244,  57,  6,  3,
  245,  57,  6,  3,
  245,  49,  6,  3,
  246,  49,  6,  3,
  246,  42,  6,  3,
  247,  42,  6,  3,
  247,  35,  6,  3,
  247,  35,  6,  3,
  247,  30,  6,  3,
  248,  30,  6,  3,
  248,  24,  6,  3,
  249,  24,  6,  3,
  249,  20,  6,  3,
  249,  20,  6,  3,
  249,  16,  6,  3,
  250,  16,  6,  3,
  250,  12,  6,  3,
  251,  12,  6,  3,
  251,   9,  6,  3,
  252,   9,  6,  3,
  252,   7,  6,  3,
  252,   7,  6,  3,
  252,   4,  6,  3,
  253,   4,  6,  3,
  253,   3,  6,  3,
  254,   3,  6,  3,
  254,   2,  6,  3,
  255,   2,  6,  3
};

// Gradient palette "sprinkles_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/alpen/tn/sprinkles.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( sprinkles_gp ) {
  0,  67,  1, 10,
  109,  67,  1, 10,
  109, 194,  3,  8,
  145, 194,  3,  8,
  145, 229, 39,  2,
  158, 229, 39,  2,
  158, 217, 119,  8,
  170, 217, 119,  8,
  170,  55, 217, 242,
  255,  55, 217, 242
};

// Gradient palette "Crystal_Ice_Palace_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/Crystal_Ice_Palace.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Crystal_Ice_Palace_gp ) {
  0, 242, 223, 252,
  51, 242, 223, 252,
  51, 227, 237, 235,
  102, 227, 237, 235,
  102, 188, 235, 226,
  153, 188, 235, 226,
  153, 128, 217, 192,
  204, 128, 217, 192,
  204,  92, 176, 137,
  255,  92, 176, 137
};


// This array holds references to the palettes in PROGMEM. That way, we can define them down here instead
// at the top cluterring it up. Any palette that gets added needs to get put in this array
const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {chroma_gp, sprinkles_gp, Crystal_Ice_Palace_gp};
const uint8_t numGradientPalettes = sizeof(gGradientPalettes)/sizeof(TProgmemRGBGradientPalettePtr);

// This has to be hand counted right now :(
const uint8_t numPalettes = numGradientPalettes + 5;

// Function that updates the global palette every N seconds
void updatePaletteAuto(uint16_t seconds) {
  // Use these to keep track of the time. Not very accurate since
  // arduino has crap timing but who cares
  uint16_t second = (millis()/1000)%(seconds*numPalettes);
  static uint16_t last_second = 99;

  // Since we only check the palette once every second, we need some way to
  // know about palettes that need continuous background refreshing. The numbering
  // will just go down the list, mirroring the order in their definitions above
  static uint8_t refresh_pal = 0;

  // Since this runs 1000x a second, make sure we're not in the same second
  if(second != last_second) {
    last_second = second;
    if( second == 0)  { update_RainbowBlack_p(); refresh_pal = 0;}
    if( second == seconds) { update_WhiteRainbow_p(); refresh_pal = 1;}
    if( second == seconds*2) { update_WhiteCol_p(); refresh_pal = 2;}
    if( second == seconds*3) { update_ColCol_p(); refresh_pal = 3;}
    if( second == seconds*4) { update_ColLead_p(); refresh_pal = 4;}
    // These are static palettes, make the first one set refresh_pal to our error num
    // so we stop refreshing the background palettes
    if( second == seconds*5) { gPalette = WhiteBlack_p; refresh_pal = 99;}
    if( second == seconds*6) { gPalette = chroma_gp;}
    if( second == seconds*7) { gPalette = sprinkles_gp;}
    if( second == seconds*8) { gPalette = Crystal_Ice_Palace_gp;}
  }

  // Check if we need to refresh anything in the background
  switch( refresh_pal ) {
    case 0:
      update_RainbowBlack_p();
      break;
    case 1:
      update_WhiteRainbow_p();
      break;
    case 2:
      update_WhiteCol_p();
      break;
    case 3:
      update_ColCol_p();
      break;
    case 4:
      update_ColLead_p();
      break;
    case 99:
      break;
  }
  
}

