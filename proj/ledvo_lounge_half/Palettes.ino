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
void update_PalBlack_p() {
  // Instead of blacking out, fill backgroud with rainbow, then
  // fill every thing with black except
  fill_palette(gPalette, 16, gHue, 16, gScheme, gBrightness, gBlending);
  for (uint8_t i = 0 ; i < 16; i++) {
    if ( i % 4 == 0) {
      continue;
    }
    gPalette[i] = CRGB::Black;
  }
}

void update_Rainbow_p() {
  fill_palette(gPalette, 16, gHue, 16, gScheme, gBrightness, gBlending);
}

// W|RB|RB|RB repeating
void update_WhitePal_p() {
  fill_palette(gPalette, 16, gHue, 16, gScheme, gBrightness, gBlending);
  gPalette[0] = CHSV(255, 0, gBrightness);
  gPalette[4] = CHSV(255, 0, gBrightness);
  gPalette[8] = CHSV(255, 0, gBrightness);
  gPalette[12] = CHSV(255, 0, gBrightness);
}

// W|X|X|X repeating
void update_WhiteCol_p() {
  fill_solid(gPalette, 16, ColorFromPalette(gScheme, gHue, gBrightness + val_offset, gBlending));
  gPalette[0] = CHSV(255, 0, gBrightness);
  gPalette[4] = CHSV(255, 0, gBrightness);
  gPalette[8] = CHSV(255, 0, gBrightness);
  gPalette[12] = CHSV(255, 0, gBrightness);
}

// Hue adjustable palettes!!!
const uint8_t val_offset = 100;

void update_PureCol_p() {
  fill_solid(gPalette, 16, ColorFromPalette(gScheme, gHue, gBrightness + val_offset, gBlending));
}

// X_LEAD|X_BACK|X_BACK|X_BACK repeating
void update_TwoCol_p() {
  for (uint8_t i = 0; i < 16; i++) {
    if (i % 4 == 0) {
      gPalette[i] = ColorFromPalette(gScheme, gHue, gBrightness + val_offset, gBlending);
    }
    else {
      gPalette[i] = ColorFromPalette(gScheme, (gHue + 128) % 255, gBrightness + val_offset, gBlending);
    }
  }
}

void update_FourCol_p() {
  for (uint8_t i = 0; i < 16; i++) {
    if (i % 4 == 0) {
      gPalette[i] = ColorFromPalette(gScheme, gHue, gBrightness + val_offset, gBlending);
    }
    else if (i % 4 == 1) {
      gPalette[i] = ColorFromPalette(gScheme, (gHue + 64) % 255, gBrightness + val_offset, gBlending);
    }
    else if (i % 4 == 2) {
      gPalette[i] = ColorFromPalette(gScheme, (gHue + 128) % 255, gBrightness + val_offset, gBlending);
    }
    else if (i % 4 == 3) {
      gPalette[i] = ColorFromPalette(gScheme, (gHue + 192) % 255, gBrightness + val_offset, gBlending);
    }
  }

}

// One color at the front and nothing else
void update_ColLead_p() {
  for (uint8_t i = 0; i < 16; i++) {
    if (i % 4 == 0) {
      gPalette[i] = ColorFromPalette(gScheme, gHue, gBrightness + val_offset, gBlending);
    }
    else {
      gPalette[i] = CRGB::Black;
    }
  }
}

void update_WhiteBlack_p() {
  for (uint8_t i = 0; i < 16; i++) {
    if (i % 4 == 0) {
      gPalette[i] = CHSV(255, 0, gBrightness);
    }
    else {
      gPalette[i] = CRGB::Black;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////
// Static templates. These are templates that do not need to be update          //
// since they are not modular and don't have any dependence on global variables //
//////////////////////////////////////////////////////////////////////////////////

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

// Gradient palette "GMT_no_green_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_no_green.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 128 bytes of program space.

DEFINE_GRADIENT_PALETTE( GMT_no_green_gp ) {
  0,   1, 29, 255,
  15,   1, 29, 255,
  15,   1, 90, 255,
  31,   1, 90, 255,
  31,   1, 135, 255,
  47,   1, 135, 255,
  47,   0, 161, 255,
  63,   0, 161, 255,
  63,   2, 255, 255,
  79,   2, 255, 255,
  79,  14, 255, 255,
  95,  14, 255, 255,
  95,  41, 255, 255,
  111,  41, 255, 255,
  111,  88, 255, 255,
  127,  88, 255, 255,
  127, 255, 255, 15,
  143, 255, 255, 15,
  143, 255, 223,  0,
  159, 255, 223,  0,
  159, 255, 135,  0,
  175, 255, 135,  0,
  175, 255, 101,  0,
  191, 255, 101,  0,
  191, 255, 66,  0,
  207, 255, 66,  0,
  207, 255, 41,  0,
  223, 255, 41,  0,
  223, 255, 18,  0,
  239, 255, 18,  0,
  239, 255,  0,  0,
  255, 255,  0,  0
};

// Gradient palette "GMT_sealand_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_sealand.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( GMT_sealand_gp ) {
  0,  53, 33, 255,
  14,  23, 33, 255,
  28,  23, 68, 255,
  42,  23, 115, 255,
  56,  23, 178, 255,
  70,  23, 255, 255,
  84,  23, 255, 170,
  99,  23, 255, 103,
  113,  23, 255, 56,
  127,  23, 255, 25,
  141,  53, 255, 25,
  155, 100, 255, 25,
  170, 167, 255, 25,
  170, 255, 255, 87,
  184, 255, 193, 87,
  198, 255, 141, 87,
  212, 255, 99, 87,
  226, 255, 115, 135,
  240, 255, 135, 182,
  255, 255, 156, 223
};

// Gradient palette "GMT_cool_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_cool.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

DEFINE_GRADIENT_PALETTE( GMT_cool_gp ) {
  0,   0, 255, 255,
  255, 255,  0, 255
};

// Gradient palette "aspectcolr_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/grass/tn/aspectcolr.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( aspectcolr_gp ) {
  0, 255, 255, 255,
  0, 255, 255,  0,
  63,   0, 255,  0,
  127,   0, 255, 255,
  191, 255,  0,  0,
  255, 255, 255,  0
};

// Gradient palette "haxby_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/grass/tn/haxby.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 44 bytes of program space.

DEFINE_GRADIENT_PALETTE( haxby_gp ) {
  0,   1,  9, 99,
  25,   2, 55, 245,
  51,   3, 133, 255,
  76,  26, 213, 255,
  102,  51, 215, 98,
  127, 144, 255, 82,
  153, 217, 215, 39,
  178, 255, 131, 17,
  204, 255, 92,  9,
  229, 255, 127, 50,
  255, 255, 255, 255
};

// Gradient palette "differences_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/grass/tn/differences.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE( differences_gp ) {
  0,   0,  0, 255,
  127, 255, 255, 255,
  255, 255,  0,  0
};

// Gradient palette "cool_warm_d05_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/km/tn/cool-warm-d05.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( cool_warm_d05_gp ) {
  0,   8, 38, 180,
  51,   8, 38, 180,
  51,  36, 111, 230,
  102,  36, 111, 230,
  102, 179, 189, 182,
  153, 179, 189, 182,
  153, 255, 37, 27,
  204, 255, 37, 27,
  204, 232,  2,  6,
  255, 232,  2,  6
};

// Gradient palette "purple_orange_d07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/km/tn/purple-orange-d07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 56 bytes of program space.

DEFINE_GRADIENT_PALETTE( purple_orange_d07_gp ) {
  0,  53, 27, 91,
  36,  53, 27, 91,
  36, 121, 55, 111,
  72, 121, 55, 111,
  72, 179, 107, 137,
  109, 179, 107, 137,
  109, 179, 189, 182,
  145, 179, 189, 182,
  145, 234, 152, 59,
  182, 234, 152, 59,
  182, 227, 92, 11,
  218, 227, 92, 11,
  218, 165, 40,  1,
  255, 165, 40,  1
};


// Gradient palette "Lucy_in_the_Sky_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/vredeling/tn/Lucy_in_the_Sky.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Lucy_in_the_Sky_gp ) {
  0, 175,  1, 44,
  51, 175,  1, 44,
  51, 194, 14, 35,
  102, 194, 14, 35,
  102, 213, 40, 27,
  153, 213, 40, 27,
  153, 234, 80, 21,
  204, 234, 80, 21,
  204, 255, 136, 15,
  255, 255, 136, 15
};

// Gradient palette "Alive_And_Kicking_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/rphnick/tn/Alive_And_Kicking.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Alive_And_Kicking_gp ) {
  0, 192, 50, 207,
  51, 192, 50, 207,
  51,  87, 50, 207,
  102,  87, 50, 207,
  102,  37, 73, 207,
  153,  37, 73, 207,
  153,  37, 127, 207,
  204,  37, 127, 207,
  204,  37, 213, 140,
  255,  37, 213, 140
};

// Gradient palette "firestrm_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/fractint/base/tn/firestrm.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 2040 bytes of program space.

DEFINE_GRADIENT_PALETTE( firestrm_gp ) {
  0,  57,  1, 194,
  0,  57,  1, 194,
  0,  60,  1, 190,
  1,  60,  1, 190,
  1,  64,  1, 186,
  3,  64,  1, 186,
  3,  67,  1, 182,
  4,  67,  1, 182,
  4,  71,  1, 178,
  4,  71,  1, 178,
  4,  74,  1, 172,
  5,  74,  1, 172,
  5,  78,  1, 168,
  7,  78,  1, 168,
  7,  82,  1, 164,
  8,  82,  1, 164,
  8,  86,  1, 160,
  9,  86,  1, 160,
  9,  90,  1, 155,
  9,  90,  1, 155,
  9,  94,  1, 151,
  10,  94,  1, 151,
  10,  98,  1, 145,
  12,  98,  1, 145,
  12, 103,  1, 142,
  13, 103,  1, 142,
  13, 107,  0, 137,
  13, 107,  0, 137,
  13, 112,  0, 133,
  14, 112,  0, 133,
  14, 117,  0, 128,
  15, 117,  0, 128,
  15, 120,  0, 123,
  17, 120,  0, 123,
  17, 125,  0, 120,
  18, 125,  0, 120,
  18, 30,   8, 10, 255,
  231,   8, 10, 255,
  231,   9,  9, 255,
  231,   9,  9, 255,
  231,   9,  8, 255,
  232,   9,  8, 255,
  232,  10,  7, 252,
  233,  10,  7, 252,
  233,  12,  6, 252,
  235,  12,  6, 252,
  235,  13,  6, 250,
  236,  13,  6, 250,
  236,  14,  5, 250,
  236,  14,  5, 250,
  236,  16,  4, 247,
  237,  16,  4, 247,
  237,  17,  4, 247,
  239,  17,  4, 247,
  239,  19,  3, 245,
  240,  19,  3, 245,
  240,  20,  3, 242,
  241,  20,  3, 242,
  241,  22,  2, 240,
  241,  22,  2, 240,
  241,  24,  2, 237,
  242,  24,  2, 237,
  242,  26,  2, 235,
  244,  26,  2, 235,
  244,  27,  1, 233,
  245,  27,  1, 233,
  245,  30,  1, 230,
  245,  30,  1, 230,
  245,  32,  1, 226,
  246,  32,  1, 226,
  246,  35,  1, 223,
  247,  35,  1, 223,
  247,  37,  1, 221,
  249,  37,  1, 221,
  249,  39,  1, 216,
  250,  39,  1, 216,
  250,  42,  1, 214,
  250,  42,  1, 214,
  250,  45,  1, 210,
  251,  45,  1, 210,
  251,  48,  1, 205,
  253,  48,  1, 205,
  253,  51,  1, 201,
  254,  51,  1, 201,
  254,  54,  1, 197,
  255,  54,  1, 197
};

// Gradient palette "royal_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/fractint/base/tn/royal.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 1896 bytes of program space.

DEFINE_GRADIENT_PALETTE( royal_gp ) {
  0,   5,  0, 14,
  0,   5,  0, 14,
  1,   5,  0, 14,
  1,   5,  0, 15,
  3,   5,  0, 15,
  3,   7,  0, 15,
  4,   7,  0, 15,
  4,   7,  0, 15,
  4,   7,  0, 17,
  5,   7,  0, 17,
  7,   7,  0, 17,
  7,   8,  0, 17,
  8,   8,  0, 17,
  8,   8,  0, 19,
  9,   8,  0, 19,
  9,   8,  0, 19,
  10,   8,  0, 19,
  10,   9,  0, 22,
  12,   9,  0, 22,
  13,   9,  0, 22,
  13,   9,  0, 22,
  13,   9,  0, 24,
  14,   9,  0, 24,
  14,  10,  0, 24,
  15,  10,  0, 24,
  17,  10,  0, 24,
  17,  10,  0, 27,
  18,  10,  0, 27,
  18,  10,  0, 27,
  19,  10,  0, 27,
  19,  12,  0, 27,
  21,  12,  0, 27,
  21,  12,  0, 29,
  22,  12,  0, 29,
  23,  12,  0, 29,
  23,  12,  0, 29,
  23,  14,  0, 28,  46, 29,  3,
  228,  46, 27,  3,
  230,  46, 27,  3,
  230,  42, 24,  4,
  231,  42, 24,  4,
  231,  39, 22,  4,
  231,  39, 22,  4,
  231,  35, 19,  4,
  232,  35, 19,  4,
  232,  32, 17,  5,
  233,  32, 17,  5,
  233,  32, 15,  5,
  235,  32, 15,  5,
  235,  30, 13,  5,
  236,  30, 13,  5,
  236,  27, 12,  6,
  236,  27, 12,  6,
  236,  24, 10,  6,
  237,  24, 10,  6,
  237,  24,  9,  6,
  239,  24,  9,  6,
  239,  22,  7,  8,
  240,  22,  7,  8,
  240,  20,  6,  8,
  241,  20,  6,  8,
  241,  18,  5,  8,
  241,  18,  5,  8,
  241,  16,  4,  9,
  242,  16,  4,  9,
  242,  16,  3,  9,
  244,  16,  3,  9,
  244,  14,  2,  9,
  245,  14,  2,  9,
  245,  12,  1, 10,
  245,  12,  1, 10,
  245,  10,  1, 10,
  246,  10,  1, 10,
  246,  10,  1, 10,
  247,  10,  1, 10,
  247,   9,  1, 12,
  249,   9,  1, 12,
  249,   8,  1, 12,
  250,   8,  1, 12,
  250,   7,  1, 12,
  250,   7,  1, 12,
  250,   5,  0, 14,
  251,   5,  0, 14,
  253,   5,  0, 14,
  254,   5,  0, 14,
  255,   5,  0, 14
};

// Gradient palette "moldy_bread_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/phill/tn/moldy_bread.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( moldy_bread_gp ) {
  0,  32, 105, 51,
  51,  32, 105, 51,
  51,  34, 139, 60,
  102,  34, 139, 60,
  102, 110, 182, 19,
  153, 110, 182, 19,
  153, 184, 203, 22,
  204, 184, 203, 22,
  204, 239, 248, 72,
  255, 239, 248, 72
};

// Gradient palette "Floating_Away_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/lightningmccarl/tn/Floating_Away.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Floating_Away_gp ) {
  0,  82, 205, 223,
  51,  82, 205, 223,
  51,  39, 144, 155,
  102,  39, 144, 155,
  102,  46, 112, 112,
  153,  46, 112, 112,
  153,  39, 70, 62,
  204,  39, 70, 62,
  204,  22, 33, 25,
  255,  22, 33, 25
};

// Gradient palette "3_girls_reclining_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/evad/tn/3_girls_reclining.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( three_girls_reclining_gp ) {
  0,   6, 45, 16,
  51,   6, 45, 16,
  51, 135, 201, 108,
  102, 135, 201, 108,
  102, 227, 221, 151,
  153, 227, 221, 151,
  153,  83, 182, 186,
  204,  83, 182, 186,
  204, 159, 127, 178,
  255, 159, 127, 178
};

// Gradient palette "iCreate_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/electroluv/tn/iCreate.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( iCreate_gp ) {
  0, 210, 80, 22,
  20, 210, 80, 22,
  20, 255, 23, 17,
  58, 255, 23, 17,
  58, 167,  1, 21,
  107, 167,  1, 21,
  107,  46,  1,  8,
  163,  46,  1,  8,
  163,  12,  1,  4,
  255,  12,  1,  4
};

// Gradient palette "Opaque_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/drumma/tn/Opaque.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Opaque_gp ) {
  0, 192, 203, 112,
  51, 192, 203, 112,
  51, 132, 169, 112,
  102, 132, 169, 112,
  102,  27, 99, 83,
  153,  27, 99, 83,
  153,  46, 36, 28,
  204,  46, 36, 28,
  204,  71,  7, 19,
  255,  71,  7, 19
};

// Gradient palette "500_LOVERS_ThankYou_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/adgrapho/tn/500_LOVERS_ThankYou.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( FiveHundo_LOVERS_ThankYou_gp ) {
  0, 222, 10,  7,
  51, 222, 10,  7,
  51, 255, 71, 21,
  102, 255, 71, 21,
  102, 242, 173, 47,
  153, 242, 173, 47,
  153, 184, 233, 91,
  204, 184, 233, 91,
  204,  39, 182, 137,
  255,  39, 182, 137
};

// Gradient palette "mellon_ball_surprise_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/mellon_ball_surprise.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( mellon_ball_surprise_gp ) {
  0, 152, 227, 85,
  51, 152, 227, 85,
  51, 215, 244, 106,
  102, 215, 244, 106,
  102, 255, 142, 56,
  153, 255, 142, 56,
  153, 255, 90, 45,
  204, 255, 90, 45,
  204, 229, 36, 62,
  255, 229, 36, 62
};

// Gradient palette "njbath_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/njgs/tn/njbath.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 192 bytes of program space.

DEFINE_GRADIENT_PALETTE( njbath_gp ) {
    0,   1,  1, 64,
    5,   1,  1, 64,
    5,   1,  1, 71,
   11,   1,  1, 71,
   11,   1,  3, 73,
   22,   1,  3, 73,
   22,   1,  7, 92,
   33,   1,  7, 92,
   33,   1,  8, 96,
   44,   1,  8, 96,
   44,   1, 10,105,
   55,   1, 10,105,
   55,   1, 17,115,
   66,   1, 17,115,
   66,   1, 21,130,
   77,   1, 21,130,
   77,   1, 30,144,
   88,   1, 30,144,
   88,   1, 31,144,
   99,   1, 31,144,
   99,   1, 44,162,
  110,   1, 44,162,
  110,   1, 54,178,
  121,   1, 54,178,
  121,   2, 59,182,
  133,   2, 59,182,
  133,   5, 68,190,
  144,   5, 68,190,
  144,  10, 85,190,
  155,  10, 85,190,
  155,  14, 95,190,
  166,  14, 95,190,
  166,  21,105,207,
  177,  21,105,207,
  177,  32,117,207,
  188,  32,117,207,
  188,  38,128,207,
  199,  38,128,207,
  199,  52,151,207,
  210,  52,151,207,
  210,  58,161,207,
  221,  58,161,207,
  221,  73,180,214,
  232,  73,180,214,
  232,  90,197,226,
  243,  90,197,226,
  243, 101,213,226,
  255, 101,213,226};

// Gradient palette "cyan_mauve_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/km/tn/cyan-mauve.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 260 bytes of program space.

DEFINE_GRADIENT_PALETTE( cyan_mauve_gp ) {
    0,   0, 82,123,
    3,   1, 82,120,
    7,   1, 81,117,
   11,   1, 81,114,
   15,   1, 80,111,
   19,   1, 80,108,
   23,   1, 79,105,
   27,   1, 79,102,
   31,   1, 78, 99,
   35,   1, 78, 98,
   39,   1, 77, 95,
   43,   1, 77, 92,
   47,   1, 75, 89,
   51,   1, 75, 87,
   55,   2, 75, 84,
   59,   2, 74, 82,
   63,   3, 74, 80,
   67,   3, 73, 78,
   71,   4, 73, 75,
   75,   4, 72, 73,
   79,   5, 72, 71,
   83,   6, 71, 69,
   87,   7, 71, 67,
   91,   8, 70, 65,
   95,   9, 70, 63,
   99,  10, 69, 62,
  103,  11, 69, 60,
  107,  12, 68, 57,
  111,  13, 68, 56,
  115,  15, 67, 54,
  119,  16, 67, 52,
  123,  18, 66, 52,
  127,  19, 65, 50,
  131,  21, 65, 48,
  135,  22, 63, 47,
  139,  24, 63, 45,
  143,  26, 62, 44,
  147,  28, 62, 42,
  151,  30, 61, 42,
  155,  32, 61, 40,
  159,  35, 60, 38,
  163,  38, 60, 37,
  167,  40, 59, 36,
  171,  43, 58, 35,
  175,  46, 58, 34,
  179,  48, 57, 33,
  183,  51, 57, 31,
  187,  54, 56, 31,
  191,  58, 56, 30,
  195,  61, 55, 29,
  199,  65, 55, 28,
  203,  68, 55, 27,
  207,  72, 54, 26,
  211,  75, 54, 25,
  215,  80, 53, 24,
  219,  84, 53, 23,
  223,  88, 52, 22,
  227,  92, 51, 22,
  231,  97, 51, 21,
  235, 103, 50, 20,
  239, 107, 50, 19,
  243, 112, 49, 19,
  247, 117, 48, 18,
  251, 123, 48, 17,
  255, 128, 47, 17};

// Gradient palette "purple_orange_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/km/tn/purple-orange.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 260 bytes of program space.

DEFINE_GRADIENT_PALETTE( purple_orange_gp ) {
    0,  29, 19, 80,
    3,  33, 20, 83,
    7,  39, 22, 85,
   11,  44, 24, 87,
   15,  49, 26, 89,
   19,  56, 28, 92,
   23,  63, 31, 93,
   27,  69, 33, 96,
   31,  77, 36, 98,
   35,  84, 39,100,
   39,  91, 42,102,
   43, 100, 45,105,
   47, 107, 49,106,
   51, 115, 53,109,
   55, 123, 56,111,
   59, 132, 61,114,
   63, 139, 66,115,
   67, 146, 71,119,
   71, 153, 77,122,
   75, 159, 82,123,
   79, 165, 88,127,
   83, 171, 95,130,
   87, 175,101,133,
   91, 179,108,137,
   95, 184,117,140,
   99, 186,124,145,
  103, 188,133,149,
  107, 188,141,153,
  111, 188,151,158,
  115, 188,159,164,
  119, 186,169,170,
  123, 184,180,176,
  127, 179,189,182,
  131, 188,187,164,
  135, 197,186,147,
  139, 203,182,132,
  143, 210,178,117,
  147, 215,175,103,
  151, 222,169, 91,
  155, 227,164, 79,
  159, 232,159, 69,
  163, 234,154, 60,
  167, 237,147, 52,
  171, 239,141, 43,
  175, 239,135, 37,
  179, 239,128, 31,
  183, 239,121, 26,
  187, 239,115, 21,
  191, 237,108, 17,
  195, 232,101, 14,
  199, 229, 95, 11,
  203, 224, 87,  9,
  207, 220, 81,  7,
  211, 213, 74,  5,
  215, 208, 69,  4,
  219, 201, 62,  3,
  223, 192, 57,  2,
  227, 186, 52,  1,
  231, 177, 46,  1,
  235, 169, 42,  1,
  239, 159, 37,  1,
  243, 152, 33,  1,
  247, 142, 29,  1,
  251, 133, 26,  1,
  255, 125, 22,  1};

// Gradient palette "green_purple_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/km/tn/green-purple.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 260 bytes of program space.

DEFINE_GRADIENT_PALETTE( green_purple_gp ) {
    0,   1, 63,  4,
    3,   1, 69,  5,
    7,   1, 74,  7,
   11,   1, 80,  9,
   15,   1, 86, 11,
   19,   1, 92, 13,
   23,   2, 99, 16,
   27,   2,104, 19,
   31,   3,111, 22,
   35,   4,117, 27,
   39,   5,124, 31,
   43,   7,130, 35,
   47,   8,136, 41,
   51,  10,142, 47,
   55,  12,149, 52,
   59,  15,154, 60,
   63,  18,159, 66,
   67,  22,164, 73,
   71,  26,169, 82,
   75,  31,175, 89,
   79,  36,178, 98,
   83,  42,182,105,
   87,  49,186,114,
   91,  57,189,122,
   95,  66,191,130,
   99,  77,193,138,
  103,  87,193,145,
  107, 100,195,155,
  111, 113,195,160,
  115, 128,195,168,
  119, 144,193,174,
  123, 161,193,178,
  127, 179,189,182,
  131, 184,180,176,
  135, 186,169,170,
  139, 188,159,164,
  143, 188,151,158,
  147, 188,141,153,
  151, 188,133,149,
  155, 186,124,145,
  159, 184,117,140,
  163, 179,108,137,
  167, 175,101,133,
  171, 171, 95,130,
  175, 165, 88,127,
  179, 159, 82,123,
  183, 153, 77,122,
  187, 146, 71,119,
  191, 139, 66,115,
  195, 132, 61,114,
  199, 123, 56,111,
  203, 115, 53,109,
  207, 107, 49,106,
  211, 100, 45,105,
  215,  91, 42,102,
  219,  84, 39,100,
  223,  77, 36, 98,
  227,  69, 33, 96,
  231,  63, 31, 93,
  235,  56, 28, 92,
  239,  49, 26, 89,
  243,  44, 24, 87,
  247,  39, 22, 85,
  251,  33, 20, 83,
  255,  29, 19, 80};

// Gradient palette "blue_tan_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/km/tn/blue-tan.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 260 bytes of program space.

DEFINE_GRADIENT_PALETTE( blue_tan_gp ) {
    0,   4, 61,201,
    3,   5, 66,203,
    7,   6, 69,205,
   11,   7, 73,207,
   15,   7, 77,210,
   19,   8, 81,212,
   23,  10, 86,212,
   27,  11, 90,214,
   31,  12, 95,214,
   35,  14, 99,214,
   39,  16,103,216,
   43,  18,108,216,
   47,  21,112,216,
   51,  23,117,214,
   55,  26,122,214,
   59,  30,127,214,
   63,  33,131,212,
   67,  38,136,212,
   71,  42,141,210,
   75,  47,144,210,
   79,  53,149,207,
   83,  59,154,205,
   87,  66,157,203,
   91,  74,162,201,
   95,  82,166,199,
   99,  91,169,199,
  103, 101,173,197,
  107, 112,176,194,
  111, 123,180,192,
  115, 137,182,190,
  119, 150,186,186,
  123, 165,187,184,
  127, 179,189,182,
  131, 182,189,168,
  135, 184,189,153,
  139, 186,187,140,
  143, 186,186,127,
  147, 188,184,114,
  151, 188,182,103,
  155, 188,178, 92,
  159, 188,175, 83,
  163, 186,171, 73,
  167, 186,168, 65,
  171, 184,164, 56,
  175, 182,159, 50,
  179, 179,154, 43,
  183, 177,149, 37,
  187, 175,144, 32,
  191, 171,139, 27,
  195, 167,133, 23,
  199, 163,128, 19,
  203, 159,122, 16,
  207, 155,117, 14,
  211, 152,111, 11,
  215, 146,105,  9,
  219, 140,100,  7,
  223, 135, 93,  6,
  227, 130, 88,  5,
  231, 125, 82,  3,
  235, 120, 78,  2,
  239, 115, 72,  2,
  243, 109, 68,  1,
  247, 103, 62,  1,
  251,  98, 57,  1,
  255,  92, 53,  1};

// Gradient palette "es_autumn_02_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/autumn/tn/es_autumn_02.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_autumn_02_gp ) {
    0,  86,  6,  1,
  127, 255,255,125,
  153, 255,255,125,
  242, 194, 96,  1,
  255, 194, 96,  1};

// Gradient palette "es_emerald_dragon_01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_emerald_dragon_01_gp ) {
    0,   1,  1,  1,
   79,   1, 19,  7,
  130,   1, 59, 25,
  229,  28,255,255,
  255,  28,255,255};

// Gradient palette "Whisper_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/lightningmccarl/tn/Whisper.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Whisper_gp ) {
    0, 210,133,210,
   12, 210,133,210,
   12, 161, 79,160,
   38, 161, 79,160,
   38, 120, 45,120,
  104, 120, 45,120,
  104,  61, 29, 63,
  132,  61, 29, 63,
  132,  30, 20, 31,
  255,  30, 20, 31};

// This has to be hand counted and updated whenever we add more palettes. There are also some predefined palettes
// that need to get accounted for :(
//const uint8_t numPalettes = 29;
const uint8_t numPalettes = 31;
const uint8_t numPalStyles = 9;

// This handles the switching of palettes based on the global palette counter. Need to add palettes to this function
// whenever they are added up above. Declare it down here so we can get away with not having to use the Gradient Palette
// Pointer array.
void updatePaletteScheme() {
  switch (gPaletteCounter) {
    case 0:
      nScheme = chroma_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 1:
      nScheme = sprinkles_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 2:
      nScheme = GMT_no_green_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 3:
      nScheme = GMT_sealand_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 4:
      nScheme = GMT_cool_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 5:
      nScheme = aspectcolr_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 6:
      nScheme = haxby_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 7:
      nScheme = differences_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 8:
      nScheme = cool_warm_d05_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 9:
      nScheme = purple_orange_d07_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 10:
      nScheme = Lucy_in_the_Sky_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 11:
      nScheme = Alive_And_Kicking_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 12:
      nScheme = firestrm_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 13:
      nScheme = royal_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 14:
      nScheme = moldy_bread_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 15:
      nScheme = Floating_Away_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 16:
      nScheme = three_girls_reclining_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 17:
      nScheme = iCreate_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 18:
      nScheme = Opaque_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 19:
      nScheme = FiveHundo_LOVERS_ThankYou_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 20:
      nScheme = mellon_ball_surprise_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 21:
      nScheme = RainbowColors_p;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 22:
      nScheme = njbath_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 23:
      nScheme = cyan_mauve_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 24:
      nScheme = purple_orange_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 25:
      nScheme = green_purple_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 26:
      nScheme = blue_tan_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 27:
      nScheme = es_autumn_02_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 28:
      nScheme = es_emerald_dragon_01_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 29:
      nScheme = Whisper_gp;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
    case 30:
      nScheme = Rainbow_p;
      nblendPaletteTowardPalette(gScheme, nScheme);
      break;
  }
}

void updateGPalette() {
  case 0:
    update_PureCol_p();
    break;
  case 1:
    update_WhiteCol_p();
    break;
  case 2:
    update_TwoCol_p();
    break;
  case 3:
    update_ColLead_p();
    break;
  case 4:
    update_Rainbow_p();
    break;
  case 5:
    update_WhiteBlack_p();
    break;
  case 6:
    update_PalBlack_p();
    break;
  case 7:
    update_WhitePal_p();
    break;
  case 8:
    update_FourCol_p();
    break;
}
