// LED library
#include <FastLED.h>

// C++ Libraries (yay!)
//#include <StandardCplusplus.h>
//#include <vector>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Analog Pins
#define RATE_POT 0 // Potentiometer for animation rate
#define HUE_POT 0 // Potentiometer for global hue
#define VAL_POT 0 // Potentiometer for global brightness
#define DJ_POT 0 // Potentiometer for DJ animation rates

// Digital Pins for LED output
#define LED_IH 6
#define LED_OH 7
#define LED_D0 22
#define LED_D1 24
#define LED_D2 26
#define LED_D3 28
#define LED_D4 30
#define LED_D5 32

// Digital Pins for interrupts
#define PAL_INC_INT 2 // Palette incrementing interrupt
#define PAL_DEC_INT 2 // Palette decrementing interrupt
#define PAL_AUTO_INT 3 // Autopilot palette mode interrupt
#define ANIM_INC_INT 3 // Animation incrementing interrupt
#define ANIM_DEC_INT 3 // Animation decrementing interrupt
#define ANIM_AUTO_INT 3 // Animation autopilot interrupt
#define STROBE_INT 3 // Strobe animation interrupt
#define BLKOUT_INT 3 // Blackout animation interrupt
#define WHTOUT_INT 3 // Whiteout animation interrupt
#define BLKSTROBE_INT 3 // Black strobe interrupt
#define REVERS_INT 3 // Reversing animations interrupt


// Variables for the LED strand
const uint8_t frameRate = 100; // FPS
const uint8_t maxBrightness = 200;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER
uint8_t led_5ft_strand = 90; // The size of one module

// Variables for LED strands
const uint8_t ih_LED_per_strand = 180; // 90 LED per 5' strand with 10' hexagon sides on the inside
const uint8_t ih_LED_total = 6*ih_LED_per_strand;
CRGBArray<ih_LED_per_strand * 6> ih_leds;

const uint16_t oh_LED_per_strand = 360; // 90 LED per 5' strand with 20' hexagon sides on outside
const uint16_t oh_LED_total = 6*oh_LED_per_strand;
CRGBArray<oh_LED_per_strand*6> oh_leds;

const uint16_t d_LED_per_strand = 45; // 90 LED per 5' strand with 10' diagonals
CRGB d_leds[6][d_LED_per_strand];

// Variables for pin interrupts (there's a lot of these babies)
uint32_t debounce_time = 15;
volatile uint32_t last_micros; // In milliseconds
volatile boolean power_state = true; // Used for kill switch
volatile uint8_t gPaletteCounter; // For choosing index of palette
volatile boolean palette_autopilot = false; // Flag for auto cycling palettes
volatile uint8_t gAnimCounter; // Counter for animation
volatile boolean anim_autopilot = false; // Flag for auto cycling animations
volatile boolean anim_reverse = false; // Flag for running animations in reverse
volatile boolean dj_control = false; // Flag if dj is controlling animations
volatile boolean run_strobe = false; // Flag for strobe interrupt animation
volatile boolean run_blackout = false; // Flag for blackout animation
volatile boolean run_whiteout = false; // Flag for whiteout animation
volatile boolean run_blkstrobe = false; // Flag for black strobing


// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette;
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index
extern const uint8_t numPalettes;
extern const TProgmemPalette16 WhiteBlack_p PROGMEM;
extern const TProgmemPalette16 WhiteBlack2_p PROGMEM;

// To control hue globally through a potentiometer input
CRGB gRGB;
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 10;

void setup() {
  delay(3000); // Safely power up

  // Setup serial for debugging
  Serial.begin(57600);

  // Setup Random Number Generator
  random16_set_seed(4832);
  random16_add_entropy(analogRead(5));

  // Setup strands of LEDs
  //  FastLED.addLeds<WS2812B, LED_IH, GRB>(ih_leds, ih_LED_total*6);
  //  FastLED.addLeds<WS2812B, LED_OH, GRB>(oh_leds, oh_LED_per_strand*6);
  FastLED.addLeds<WS2812B, LED_D0, GRB>(d_leds[0], d_LED_per_strand);
  FastLED.addLeds<WS2812B, LED_D1, GRB>(d_leds[1], d_LED_per_strand);
  FastLED.addLeds<WS2812B, LED_D2, GRB>(d_leds[2], d_LED_per_strand);
  FastLED.addLeds<WS2812B, LED_D3, GRB>(d_leds[3], d_LED_per_strand);
  FastLED.addLeds<WS2812B, LED_D4, GRB>(d_leds[4], d_LED_per_strand);
  FastLED.addLeds<WS2812B, LED_D5, GRB>(d_leds[5], d_LED_per_strand);
  FastLED.setBrightness(maxBrightness);
  FastLED.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = LINEARBLEND;
  gHue = 0;
  gIndex = 0;
  gRGB = CRGB::Red;
  gPaletteCounter = 0;
  gAnimCounter = 0;

  // Interrupts to switch palette choice
  pinMode(PAL_INC_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PAL_INC_INT), debounce_palette_inc, RISING);

  //pinMode(PAL_DEC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_DEC_INT), debounce_palette_dec, RISING);

  // Interrupt to turn on and off palette autopilot mode. Use change since the button
  // will be latching
  //pinMode(PAL_AUTO_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_AUTO_INT), debounce_palette_auto, CHANGE);

  // Interrupts to switch animation
  pinMode(ANIM_INC_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(ANIM_INC_INT), debounce_anim_inc, RISING);

  //pinMode(ANIM_DEC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ANIM_DEC_INT), debounce_anim_dec, RISING);

  // Interrupt to turn on and off animation autopilot mode. Use change since button
  // will be latching.
  //pinMode(ANIM_AUTO_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ANIM_AUTO_INT), debounce_anim_auto, CHANGE);

  // Interrupt for strobe DJ animation. Use change so animation starts on button press
  // and ends on button release
  //pinMode(STROBE_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(STROBE_INT), debounce_strobe, CHANGE);

  // Interrupt for blackout DJ animation. Look for change so we can hold and release
  //pinMode(BLKOUT_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BLKOUT_INT), debounce_blackout, CHANGE);

  // Interrupt for whiteout DJ animation. Look for change so we can hold and release
  //pinMode(WHTOUT_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(WHTOUT_INT), debounce_whiteout, CHANGE);

  // Interrupt for strobing black over current animation.
  //pinMode(BLKSTROBE_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BLKSTROBE_INT), debounce_blk_strobe, RISING);

  // Interrupt for reversing animation direction. Look for change since this will
  // be a latching button
  //pinMode(REVERS_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(REVERS_INT), debounce_anim_reverse, CHANGE);
}

void loop() {
  // Read in global brightness value
  //gBrightness = map(analogRead(VAL_POT), 0, 1253, 0, maxBrightness);

  // Read color from potentiometer input
  gRGB = CHSV(map(analogRead(HUE_POT), 0, 1253, 0, 255), 255, gBrightness);

  // Increment global hue index for extra groovy rainbows
  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }

  // Check the palette counter and switch acordingly or go into autopilot mode
  if (palette_autopilot) {
    EVERY_N_SECONDS(30) {
      gPaletteCounter = (gPaletteCounter + 1) % numPalettes;
    }
    updateGPalette();
  }
  else {
    updateGPalette();
  }

  // Check if we want to autopilot the animations
  if (anim_autopilot) {
    EVERY_N_SECONDS(30) {
      gAnimCounter = (gAnimCounter + 1) % numAnimation;
    }
  }

  // For flashing black over current animation. We want it to work
  // with the animation so we want it separate from the DJ controls
  //  if (run_blkstrobe) {
  //    strobe_black();
  //  }

  // Select animation to run based on global counter
  /*if (!dj_control) {
    switch( gAnimCounter ) {

    }
    }*/

  //diagonal_eq();
  //diagonal_theater_eo();
  diagonal_flash_timed();

  // The following are all checks for DJ animations that
  // interrupt the normal animations for some added IN YO FACE
  //  if (dj_control) {
  //    if (run_strobe) {
  //      strobes();
  //    }
  //    if (run_blackout) {
  //      blackout();
  //    }
  //    if (run_whiteout) {
  //      whiteout();
  //    }
  //  }

  // For a framerate (probably gonna scrap this)
  //FastLED.delay(1000./ frameRate);
}

/////////////////////////////////////////////////////////////////////////////////////////
// These are the high level animations. Basically they use the animation base function //
// and some clever rearranging to get some particularly snazzy animations.             //
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////
// DIAGONAL ANIMATIONS //
/////////////////////////

// This gives a fake eq effect along the diagonals. Maybe we'll flash the inner or outer hex
// every once in a while for some added effect
void diagonal_eq() {
  static uint8_t pal_index[] = {0, 0, 0, 0, 0, 0};
  static uint16_t lead_max[] = {d_LED_per_strand / 2, d_LED_per_strand / 2,
                                d_LED_per_strand / 2, d_LED_per_strand / 2,
                                d_LED_per_strand / 2, d_LED_per_strand / 2
                               };
  static uint16_t lead[] = {0, 0, 0, 0, 0, 0};

  // Fill the bars at a input dependent rate. Lets try this with a triangular wave at first
  EVERY_N_MILLISECONDS(50) {
    //thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    for (uint8_t dd = 0; dd < 6; dd++) {
      lead[dd] = (lead[dd] + 1) % (lead_max[dd] * 2);
    }
  }

  // Check if the diagonals bars are about to be empty. Increment the palette index and
  // generate a new max length if so
  for (uint8_t dd = 0; dd < 6; dd++) {
    if (lead[dd] == 0) {
      pal_index[dd] += 16;
      lead_max[dd] = random16(d_LED_per_strand / 3, d_LED_per_strand); // from 1/3 length -> length
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill
  for (uint8_t dd = 0; dd < 6; dd++) {
    if (lead[dd] < lead_max[dd]) {
      fill_solid(d_leds[dd], lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    else {
      fill_solid(d_leds[dd], 2 * lead_max[dd] - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
  }
  FastLED.show();
  for (uint8_t dd = 0; dd < 6; dd++ ) {
    fadeToBlackBy(d_leds[dd], d_LED_per_strand, 20);
  }

}

void diagonal_tri() {
  static uint8_t pal_index[] = {0, 0, 0, 0, 0, 0};
  // Since the offsets are constant let's just set them here
  static uint8_t lead[] = {0, 4, 8, 12, 16, 20};
  static boolean first_pass = true;

  // Fill the bars at a input dependent rate. This basically runs a triangular
  // wave on each strip, but we offset them so you can see the whole waveform
  EVERY_N_MILLISECONDS(60) {
    //thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    for (uint8_t dd = 0; dd < 6; dd++) {
      lead[dd] = (lead[dd] + 1) % (2 * d_LED_per_strand);
    }
    if (first_pass) {
      first_pass = false;
    }
  }

  // Check if the first strand is about to be empty and increment the color index. The
  // first if statement is to make sure the first strip doesn't get a color rotation ahead
  if (!first_pass) {
    for (uint8_t dd = 0; dd < 6; dd++) {
      if ( lead[dd] == 0 ) {
        pal_index[dd] += 16;
      }
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill
  for (uint8_t dd = 0; dd < 6; dd++) {
    // Going up
    if (lead[dd] < d_LED_per_strand) {
      fill_solid(d_leds[dd], lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    // Going down
    else {
      fill_solid(d_leds[dd], 2 * d_LED_per_strand - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
  }

  FastLED.show();

  for (uint8_t dd = 0; dd < 6; dd++ ) {
    fadeToBlackBy(d_leds[dd], d_LED_per_strand, 20);
  }
}

void diagonal_theater() {
  static uint8_t pal_index = 0;

  // Move the palette index in the appropriate direction depending on
  // which strip we're on
  EVERY_N_MILLISECONDS(10) {
    pal_index = 0;
  }

  for (uint8_t dd = 0 ; dd < 6; dd++) {
    theater_chase(d_leds[dd], d_LED_per_strand, pal_index);
  }
}

// Run theater chase in opposite directions along every other strip
void diagonal_theater_eo() {
  // First index is for increasing, second index is for decreasing
  static uint8_t pal_index[] = {0, 0};

  // Move the palette index in the appropriate direction depending on
  // which strip we're on
  EVERY_N_MILLISECONDS(10) {
    pal_index[0]++;
    pal_index[1]--;
  }

  for (uint8_t dd = 0 ; dd < 6; dd++) {
    if ( dd % 2 == 0 ) {
      theater_chase(d_leds[dd], d_LED_per_strand, pal_index[0]);
    }
    else {
      theater_chase(d_leds[dd], d_LED_per_strand, pal_index[1]);
    }
  }

}

// Flash the whole diagonal strips in various orders depending on the elapsed time. First
// flash from 0 -> 6, then flash opposite sides, then 0 -> 6 -> 0, then ranndom strips
void diagonal_flash_timed() {
  static uint8_t brightness = 0;
  static uint8_t pal_index[]  = {0, 0, 0, 0, 0, 0};
  static uint8_t anim_set = 0;
  static uint8_t current_strand = 0;
  static boolean just_switched = true;

  // First, keep track of how long we've been running this animation
  /*EVERY_N_SECONDS(10) {
    anim_set = (anim_set + 1) % 4;
    // Reset some shit every time we switch animations
    for (uint8_t dd = 0; dd < 6; dd++) {
      brightness[dd] = 0;
      current_strand = 0;
    }
    just_switched = true;
    }*/

  EVERY_N_MILLISECONDS(10) {
    brightness += 16;
    if (just_switched) {
      just_switched = false;
    }
  }

  if (!just_switched) {
    if (brightness == 0) {
      if (current_strand < 6) {
        fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB::Black);
      }
      else {
        fill_solid(d_leds[12 - current_strand], d_LED_per_strand, CRGB::Black);
      }
      FastLED.show();
      current_strand = (current_strand + 1) % 12;
      just_switched = true;
    }
  }

  if (current_strand < 6) {
    fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB(brightness, 0, 0));
  }
  else {
    fill_solid(d_leds[12 - current_strand], d_LED_per_strand, CRGB(brightness, 0, 0));
  }
  FastLED.show();

  // We have to use the just_switched flag in order keep the code from running over certain
  // conditions until they have actually changed.
  /*switch (anim_set) {
    // Flash 0 -> 6
    case 0:
      EVERY_N_MILLISECONDS(10) {
        brightness += 16;
        if ( just_switched) {
          just_switched = false;
        }
      }
      if (!just_switched) {
        if (brightness == 0) {
          fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB::Black);
          FastLED.show();
          current_strand = (current_strand + 1) % 6;
          just_switched = true;
        }
      }
      fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB(brightness, 0, 0));
      FastLED.show();
    // Flash opposite sides 0 -> 3 (yay symmetry!!!)
    case 1:
      EVERY_N_MILLISECONDS(10) {
        brightness += 16;
        if ( just_switched) {
          just_switched = false;
        }
      }
      if (!just_switched) {
        if (brightness == 0) {
          fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB::Black);
          fill_solid(d_leds[current_strand + 3], d_LED_per_strand, CRGB::Black);
          FastLED.show();
          current_strand = (current_strand + 1) % 3;
          just_switched = true;
        }
      }
      fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB(brightness, 0, 0));
      fill_solid(d_leds[current_strand + 3], d_LED_per_strand, CRGB(brightness, 0, 0));
      FastLED.show();
    // Flash strands 0 -> 6 -> 0
    case 2:
      EVERY_N_MILLISECONDS(10) {
        brightness += 16;
        if (just_switched) {
          just_switched = false;
        }
      }

      if (!just_switched) {
        if (brightness == 0) {
          if (current_strand < 6) {
            fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB::Black);
          }
          else {
            fill_solid(d_leds[12 - current_strand], d_LED_per_strand, CRGB::Black);
          }
          FastLED.show();
          current_strand = (current_strand + 1) % 12;
          just_switched = true;
        }
      }

      if (current_strand < 6) {
        fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB(brightness, 0, 0));
      }
      else {
        fill_solid(d_leds[12 - current_strand], d_LED_per_strand, CRGB(brightness, 0, 0));
      }
      FastLED.show();
    // Flash random strands
    case 3:
      EVERY_N_MILLISECONDS(25) {
        brightness[current_strand] += 16;
      }
      if (brightness[current_strand] == 0) {
        current_strand = random8(6);
      }
      fill_solid(d_leds[current_strand], d_LED_per_strand, CRGB::Red);
      FastLED.show();
    }*/

}

//////////////////////////
// Inner Hex Animations //
//////////////////////////

void ih_theater_eo() {
  static uint8_t pal_index = 0;

  EVERY_N_MILLISECONDS(50) {
    pal_index++;
  }

  for (uint8_t dd = 0; dd < 6; dd++) {
    if (dd % 2 == 0) {
      theater_chase(ih_leds(dd, dd + led_5ft_strand), led_5ft_strand, pal_index);
    }
  }
}

void ih_theater() {
  static uint8_t pal_index = 0;

  EVERY_N_MILLISECONDS(50) {
    pal_index++;
  }

  theater_chase(ih_leds, ih_LED_total, pal_index);
}

//////////////////////////
// Outer Hex Animations //
//////////////////////////

void oh_theater_eo() {
  static uint8_t pal_index = 0;

  EVERY_N_MILLISECONDS(50) {
     pal_index++;
  }

  for (uint8_t dd = 0; dd < 6; dd++) {
    if (dd % 2 == 1) {
      theater_chase(oh_leds(dd, dd+oh_LED_per_strand), oh_LED_per_strand, pal_index);
    }
  }
}

