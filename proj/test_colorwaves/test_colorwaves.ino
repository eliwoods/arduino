// ColorWavesWithPalettes
// Animated shifting color waves, with several cross-fading color palettes.
// by Mark Kriegsman, August 2015
//
// Color palettes courtesy of cpt-city and its contributors:
//   http://soliton.vm.bytemark.co.uk/pub/cpt-city/
//
// Color palettes converted for FastLED using "PaletteKnife" v1:
//   http://fastled.io/tools/paletteknife/
//

#include "FastLED.h"
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define COLOR_ORDER RGB
#define BRIGHTNESS  80
#define SECONDS_PER_PALETTE 20
#define NUM_LEDS_PART_A 30  
CRGB ledsA[NUM_LEDS_PART_A];


void setup() {
  delay(3000); // 3 second delay for recovery
  
FastLED.addLeds<WS2811, 6>(ledsA, NUM_LEDS_PART_A);

    //.setCorrection(TypicalLEDStrip) 
    //setDither(BRIGHTNESS < 255);
    set_max_power_in_volts_and_milliamps(5, 1000);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;
uint8_t gCurrentPaletteNumber = 0;
CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );


void loop()
{
  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }

  EVERY_N_MILLISECONDS(40) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
  }
  
  colorwaves( ledsA, NUM_LEDS_PART_A, gCurrentPalette);

  FastLED.show();
  FastLED.delay(30);
}


// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds-1) - pixelnumber;
    
    nblend( ledarray[pixelnumber], newcolor, 128);
  }
}

// Alternate rendering function just scrolls the current palette 
// across the defined LED strip.
//void palettetest( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette)
//{
//  static uint8_t startindex = 0;
//  startindex--;
// fill_palette( ledarray, NUM_LEDS_PART_A, startindex, (256 / NUM_LEDS_PART_A) + 1, gCurrentPalette, 255, LINEARBLEND);
// fill_palette( ledarray, NUM_LEDS_PART_B, startindex, (256 / NUM_LEDS_PART_B) + 1, gCurrentPalette, 255, LINEARBLEND);
// fill_palette( ledarray, NUM_LEDS_PART_C, startindex, (256 / NUM_LEDS_PART_C) + 1, gCurrentPalette, 255, LINEARBLEND);
//}


// Gradient palette "froth3_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/fractint/base/tn/froth3.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 1524 bytes of program space.

// Gradient palette "pjs_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/pj/classic/tn/pjs.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 48 bytes of program space.

DEFINE_GRADIENT_PALETTE( pjs_gp ) {
    0, 201,  0,132,
   21, 227,  0, 23,
   22, 255,  0,  0,
   45, 255, 53,  0,
   63, 255,246,  0,
   99,   1,118,  6,
  140,   0,211,255,
  172,   0, 46,138,
  193,   0,  1, 61,
  214,  32,  1, 92,
  239, 201,  0,132,
  255, 201,  0,132};

// Gradient palette "aspectrum_2_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/pj/1/tn/aspectrum-2.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE( aspectrum_2_gp ) {
    0, 237,  1,  9,
   19, 239, 18,  3,
   51, 239, 57,  1,
   84, 242,217,  1,
  124,   9,144, 36,
  163,   1,156,186,
  201,  16, 13,255,
  237,  78,  1,156,
  255,  78,  1,156};

// Gradient palette "aspectrum_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/pj/1/tn/aspectrum.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE( aspectrum_gp ) {
    0, 237,  1,  9,
   19, 239, 18,  3,
   51, 239, 57,  1,
   84, 242,217,  1,
  124,   9,144, 36,
  163,   1,156,186,
  201,  16, 13,255,
  237,  78,  1,156,
  255,  78,  1,156};

// Gradient palette "saga_19_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-19.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_19_gp ) {
    0,  22,  0, 69,
   13,  10,  0,103,
   26,   3,  0,144,
   40,   1,  0,194,
   53,   0,  0,255,
   67,   0,  7,145,
   80,   0, 33, 71,
   93,   0, 82, 25,
  107,   0,156,  4,
  120,   0,255,  0,
  134,   3,156,  0,
  147,  23, 82,  0,
  161,  67, 33,  0,
  174, 142,  7,  0,
  187, 255,  0,  0,
  201, 192,  0,  1,
  214, 140,  0,  4,
  228, 100,  0, 12,
  241,  66,  0, 25,
  255,  41,  0, 44};
  
// Gradient palette "saga_20_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-20.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_20_gp ) {
    0,  41, 55,  0,
   13,  66, 33,  0,
   26, 100, 17,  0,
   40, 140,  7,  0,
   53, 192,  1,  0,
   67, 255,  0,  0,
   80, 142,  0,  4,
   93,  67,  0, 25,
  107,  23,  0, 71,
  120,   3,  0,145,
  134,   0,  0,255,
  147,   0,  7,145,
  161,   0, 33, 71,
  174,   0, 82, 25,
  187,   0,156,  4,
  201,   0,255,  0,
  214,   1,201,  0,
  228,   3,154,  0,
  241,  10,115,  0,
  255,  22, 81,  0};

// Gradient palette "saga_21_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-21.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_21_gp ) {
    0,  22, 81,  0,
   13,  10,115,  0,
   26,   3,154,  0,
   40,   1,201,  0,
   53,   0,255,  0,
   67,   0,156,  4,
   80,   0, 82, 25,
   93,   0, 33, 71,
  107,   0,  7,145,
  120,   0,  0,255,
  134,   3,  0,145,
  147,  23,  0, 71,
  161,  67,  0, 25,
  174, 142,  0,  4,
  187, 255,  0,  0,
  201, 192,  1,  0,
  214, 140,  7,  0,
  228, 100, 17,  0,
  241,  66, 33,  0,
  255,  41, 55,  0};

// Gradient palette "saga_14_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-14.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_14_gp ) {
    0,   0,  0,255,
   13,   1,  0,221,
   26,   1,  0,192,
   40,   2,  0,164,
   53,   4,  0,140,
   67,   7,  0,117,
   80,  12,  0, 98,
   93,  18,  0, 80,
  107,  26,  0, 64,
  120,  35,  0, 51,
  134,  47,  0, 38,
  147,  60,  0, 29,
  161,  77,  0, 20,
  174,  94,  0, 14,
  187, 113,  0,  9,
  201, 137,  0,  5,
  214, 161,  0,  2,
  228, 190,  0,  1,
  241, 220,  0,  1,
  255, 255,  0,  0};
  
// Gradient palette "saga_22_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-22.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_22_gp ) {
    0,  41,  0, 44,
   13,   6,  0,123,
   26,   0,  0,255,
   40,   0, 22,255,
   53,   0,104,255,
   67,   0,255,255,
   80,   0,209, 92,
   93,   0,169, 16,
  107,   0,135,  0,
  120,  14,169,  0,
  134,  88,209,  0,
  147, 255,255,  0,
  161, 255,169,  0,
  174, 255,103,  0,
  187, 255, 55,  0,
  201, 255, 22,  0,
  214, 255,  4,  0,
  228, 255,  0,  0,
  241, 157,  0,  0,
  255,  87,  0,  0};

// Gradient palette "saga_01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_01_gp ) {
    0,   0,  1, 45,
   13,   0,  1, 88,
   26,   0,  1,142,
   40,   0,  8,197,
   53,   0, 24,237,
   67,   1, 55,255,
   80,   1,100,237,
   93,   1,152,197,
  107,   4,203,142,
  120,  16,239, 88,
  134,  42,255, 45,
  147,  84,239, 17,
  161, 139,203,  5,
  174, 194,152,  1,
  187, 237,100,  1,
  201, 255, 55,  1,
  214, 237, 24,  0,
  228, 194,  8,  0,
  241, 139,  1,  0,
  255,  84,  1,  0};

// Gradient palette "GMT_seis_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_seis.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( GMT_seis_gp ) {
    0,  88,  0,  0,
   28, 255,  0,  0,
   56, 255, 22,  0,
   85, 255,104,  0,
  113, 255,255,  0,
  141, 255,255,  0,
  169,  17,255,  1,
  198,   0,223, 31,
  226,   0, 19,255,
  255,   0,  0,147};
  
// Gradient palette "Sunset_Real_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Sunset_Real.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255,   0,  0,160};


const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {

pjs_gp,
aspectrum_2_gp,
aspectrum_gp,
saga_19_gp,
saga_20_gp,
saga_21_gp,
saga_14_gp,
saga_22_gp,
saga_01_gp,
GMT_seis_gp, 
Sunset_Real_gp,

   };

const uint8_t gGradientPaletteCount = sizeof( gGradientPalettes)/ sizeof( TProgmemRGBGradientPalettePtr);

// Count of how many cpt-city gradients are defined:
//const uint8_t gGradientPaletteCount = 
//  sizeof( gGradientPalettes) / sizeof( TProgmemRGBGradientPalettePtr );﻿
