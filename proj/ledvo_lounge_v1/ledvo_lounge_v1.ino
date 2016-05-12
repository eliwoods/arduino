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

// Digital Pins for LED output
#define LED_OH1 7
#define LED_OH2 8
#define LED_D0 22
#define LED_D1 24
#define LED_D2 26
#define LED_D3 28
#define LED_D4 30
#define LED_D5 32
#define LED_IH 34

// Digital Pins for interrupts
#define KILL_INT 18 // Button to turn on or off the lights
#define PAL_INC_INT 14 // Palette incrementing interrupt
#define PAL_DEC_INT 15 // Palette decrementing interrupt
#define PAL_AUTO_INT 18 // Autopilot palette mode interrupt
#define ANIM_INC_INT 3 // Animation incrementing interrupt
#define ANIM_DEC_INT 3 // Animation decrementing interrupt
#define ANIM_AUTO_INT 3 // Animation autopilot interrupt
#define STROBE_INT 16 // Strobe animation interrupt
#define BLKSTROBE_INT 3 // Black strobe interrupt
#define REVERS_INT 17 // Reversing animations interrupt


// Variables for the LED strand
const uint8_t frameRate = 100; // FPS
const uint8_t maxBrightness = 200;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Variables for LED strands
const uint16_t led_5ft_strand = 90; // The size of one module

const uint16_t ih_LED_per_strand = 2 * led_5ft_strand; // 90 LED per 5' strand with 10' hexagon sides on the inside
const uint16_t ih_LED_total = 6 * ih_LED_per_strand;
CRGBArray<ih_LED_total> ih_leds;

const uint16_t oh_LED_per_strand = 4 * led_5ft_strand; // 90 LED per 5' strand with 20' hexagon sides on outside
const uint16_t oh_LED_total = 6 * oh_LED_per_strand;
CRGBArray<oh_LED_total> oh_leds;

const uint16_t d_LED_num = 2 * led_5ft_strand; // 90 LED per 5' strand with 10' diagonals
CRGB d_leds[6][d_LED_num];

// Variables for pin interrupts. There's a lot of these babies ;^)
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
  FastLED.addLeds<WS2812B, LED_IH, GRB>(ih_leds, ih_LED_total);
  FastLED.addLeds<WS2812B, LED_OH1, GRB>(oh_leds, oh_LED_total / 2);
  //  FastLED.addLeds<WS2812B, LED_OH2, GRB>(oh_leds+oh_LED_total/2, oh_LED_total/2);
  FastLED.addLeds<WS2812B, LED_D0, GRB>(d_leds[0], d_LED_num / 2);
  FastLED.addLeds<WS2812B, LED_D1, GRB>(d_leds[1], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D2, GRB>(d_leds[2], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D3, GRB>(d_leds[3], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D4, GRB>(d_leds[4], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D5, GRB>(d_leds[5], d_LED_num);
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
  pinMode(PAL_INC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAL_INC_INT), debounce_palette_inc, RISING);

  pinMode(PAL_DEC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAL_DEC_INT), debounce_palette_dec, RISING);

  // Interrupt to turn on and off palette autopilot mode. Use change since the button
  // will be latching
  pinMode(PAL_AUTO_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAL_AUTO_INT), debounce_palette_auto, CHANGE);

  // Interrupts to switch animation
  pinMode(ANIM_INC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANIM_INC_INT), debounce_anim_inc, RISING);

  pinMode(ANIM_DEC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANIM_DEC_INT), debounce_anim_dec, RISING);

  // Interrupt to turn on and off animation autopilot mode. Use change since button
  // will be latching.
  pinMode(ANIM_AUTO_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANIM_AUTO_INT), debounce_anim_auto, CHANGE);

  // Interrupt for strobe DJ animation. Use change so animation starts on button press
  // and ends on button release
  pinMode(STROBE_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(STROBE_INT), debounce_strobe, CHANGE);

  // Interrupt for strobing black over current animation.
  pinMode(BLKSTROBE_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLKSTROBE_INT), debounce_blk_strobe, RISING);

  // Interrupt for reversing animation direction. Look for change since this will
  // be a latching button
  pinMode(REVERS_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REVERS_INT), debounce_anim_reverse, CHANGE);
}

void loop() {
  // Read in global brightness value
  gBrightness = map(analogRead(VAL_POT), 0, 1253, 0, maxBrightness);

  // Read color from potentiometer input
  gRGB = CHSV(map(analogRead(HUE_POT), 0, 1253, 0, 255), 255, gBrightness);

  // Increment global hue index for extra groovy rainbows
  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }

  // Check the palette counter and switch acordingly or go into autopilot mode
  if (palette_autopilot) {
    EVERY_N_SECONDS(5) {
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

  static uint8_t theat_opt = 0;
  EVERY_N_SECONDS(60) {
    theat_opt = (theat_opt + 1) % 3;
  }

  // The following are all checks for DJ animations that
  // interrupt the normal animations for some added IN YO FACE
  if (dj_control) {
    if (run_strobe) {
      strobes();
    }
  }
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
  static uint16_t lead_max[] = {d_LED_num / 2, d_LED_num / 2,
                                d_LED_num / 2, d_LED_num / 2,
                                d_LED_num / 2, d_LED_num / 2
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
      lead_max[dd] = random16(d_LED_num / 3, d_LED_num); // from 1/3 length -> length
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
    fadeToBlackBy(d_leds[dd], d_LED_num, 20);
  }

}

void whole_eq() {
  static uint8_t pal_index[] = {0, 0, 0, 0, 0, 0};
  static uint16_t lead_max[] = {d_LED_num / 2, d_LED_num / 2,
                                d_LED_num / 2, d_LED_num / 2,
                                d_LED_num / 2, d_LED_num / 2
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
      lead_max[dd] = random16(d_LED_num / 3, d_LED_num); // from 1/3 length -> length
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
    fadeToBlackBy(d_leds[dd], d_LED_num, 20);
  }

}

// Diagonal eq except that it is mirror at both ends,
void diagonal_eq_opp() {
  static uint8_t pal_index[] = {0, 0, 0, 0, 0, 0};
  static uint16_t lead_max[] = {d_LED_num / 4, d_LED_num / 4,
                                d_LED_num / 4, d_LED_num / 4,
                                d_LED_num / 4, d_LED_num / 4
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
      lead_max[dd] = random16((d_LED_num / 2) / 3, d_LED_num / 2); // from 1/3 length -> length
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill
  for (uint8_t dd = 0; dd < 6; dd++) {
    if (lead[dd] < lead_max[dd]) {
      //fill_solid(d_leds[dd], lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
      fill_solid(d_leds[dd] + d_LED_num / 2, d_LED_num / 2 - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    else {
      //fill_solid(d_leds[dd], 2 * lead_max[dd] - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
      fill_solid(d_leds[dd] + d_LED_num / 2, lead[dd] - lead_max[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
  }
  FastLED.show();
  for (uint8_t dd = 0; dd < 6; dd++ ) {
    fadeToBlackBy(d_leds[dd], d_LED_num, 20);
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
      lead[dd] = (lead[dd] + 1) % (2 * d_LED_num);
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
    if (lead[dd] < d_LED_num) {
      fill_solid(d_leds[dd], lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    // Going down
    else {
      fill_solid(d_leds[dd], 2 * d_LED_num - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
  }

  FastLED.show();

  for (uint8_t dd = 0; dd < 6; dd++ ) {
    fadeToBlackBy(d_leds[dd], d_LED_num, 20);
  }
}

// Just runs the theater chase that's selected on the diagonals
// theat_opt == 0 : theater_chase
// theat_opt == 1 : theater_chase_saw
// theat_opt == 2 : theater_chase_mod
void diagonal_theater(uint8_t theat_opt) {
  static uint8_t pal_index = 0;

  // Check that the option is in range.
  if (theat_opt > 3) {
    theat_opt = 0;
  }

  // Move the palette index in the appropriate direction depending on
  // which strip we're on
  EVERY_N_MILLISECONDS(10) {
    pal_index++;
  }

  if (theat_opt == 0) {
    for (uint8_t dd = 0; dd < 6; dd++) {
      theater_chase(d_leds[dd], d_LED_num, pal_index);
    }
  }
  else if (theat_opt == 1) {
    for (uint8_t dd = 0 ; dd < 6; dd++) {
      theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
    }
  }
  else if (theat_opt == 2) {
    for (uint8_t dd = 0 ; dd < 6; dd++) {
      theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
    }
  }
}

// Run theater chase in opposite directions along every other strip
void diagonal_theater_eo(uint8_t theat_opt) {
  // First index is for increasing, second index is for decreasing
  static uint8_t pal_index[] = {0, 0};

  // Move the palette index in the appropriate direction depending on
  // which strip we're on
  EVERY_N_MILLISECONDS(10) {
    pal_index[0]++;
    pal_index[1]--;
  }

  if (theat_opt == 0) {
    for (uint8_t dd = 0 ; dd < 6; dd++) {
      if ( dd % 2 == 0 ) {
        theater_chase(d_leds[dd], d_LED_num, pal_index[0]);
      }
      else {
        theater_chase(d_leds[dd], d_LED_num, pal_index[1]);
      }
    }
  }
  if (theat_opt == 1) {
    for (uint8_t dd = 0 ; dd < 6; dd++) {
      if ( dd % 2 == 0 ) {
        theater_chase_tri(d_leds[dd], d_LED_num, pal_index[0]);
      }
      else {
        theater_chase_tri(d_leds[dd], d_LED_num, pal_index[1]);
      }
    }
  }
  if (theat_opt == 2) {
    for (uint8_t dd = 0 ; dd < 6; dd++) {
      if ( dd % 2 == 0 ) {
        theater_chase_mod(d_leds[dd], d_LED_num, pal_index[0]);
      }
      else {
        theater_chase_mod(d_leds[dd], d_LED_num, pal_index[1]);
      }
    }
  }

}

// THESE ARE THE ACTUAL FULL BODY ANIMATIONS. THESE WILL MAKE THE SLICES AND 

// Flash the whole diagonal strips in various orders depending on the elapsed time. First
// flash from 0 -> 6, then flash opposite sides, then 0 -> 6 -> 0, then ranndom strips
void diagonal_flash_timed() {
  static uint8_t brightness = 0;
  static uint8_t pal_index[]  = {0, 0, 0, 0, 0, 0};
  static uint8_t anim_set = 0;
  static uint8_t current_strand = 0;
  static uint8_t next_strand = 0;
  static boolean just_switched = true;
  static boolean flagged = false;

  // First, keep track of how long we've been running this animation
  EVERY_N_SECONDS(10) {
    anim_set = (anim_set + 1) % 4;
    // Reset some shit every time we switch animations
    for (uint8_t dd = 0; dd < 6; dd++) {
      fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
    }
    brightness = 0;
    current_strand = 0;
    just_switched = true;
    flagged = false;
  }

  // We have to use the just_switched flag in order keep the code from running over certain
  // conditions until they have actually changed.
  if (anim_set == 0) {
    EVERY_N_MILLISECONDS(10) {
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        current_strand = (current_strand + 1) % 6;
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, CRGB(brightness, 0, 0));
    fill_solid(ih_leds, ih_LED_total, CRGB(brightness, 0, 0));
    fill_solid(oh_leds, oh_LED_total, CRGB(brightness, 0, 0));
    FastLED.show();
  }
  // Flash opposite sides 0 -> 3 (yay symmetry!!!)
  if (anim_set == 1) {
    EVERY_N_MILLISECONDS(10) {
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(d_leds[current_strand + 3], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        current_strand = (current_strand + 1) % 3;
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, CRGB(brightness, 0, 0));
    fill_solid(d_leds[current_strand + 3], d_LED_num, CRGB(brightness, 0, 0));
    fill_solid(ih_leds, ih_LED_total, CRGB(brightness, 0, 0));
    fill_solid(oh_leds, oh_LED_total, CRGB(brightness, 0, 0));
    FastLED.show();
  }
  // Flash strands 0 -> 6 -> 0
  if (anim_set == 2) {
    EVERY_N_MILLISECONDS(10) {
      brightness += 16;
      if (just_switched) {
        just_switched = false;
      }
    }

    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        if (current_strand == 0) {
          flagged = false;
        }
        if (current_strand == 5) {
          flagged = true;
        }
        if (flagged) {
          current_strand--;
        }
        else {
          current_strand++;
        }
        just_switched = true;
      }
    }

    fill_solid(d_leds[current_strand], d_LED_num, CRGB(brightness, 0, 0));
    fill_solid(ih_leds, ih_LED_total, CRGB(brightness, 0, 0));
    fill_solid(oh_leds, oh_LED_total, CRGB(brightness, 0, 0));
    FastLED.show();
  }
  // Flash random strands
  if (anim_set == 3) {
    EVERY_N_MILLISECONDS(10) {
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        next_strand = random8(6);
        while (next_strand == current_strand) {
          next_strand = random8(6);
        }
        current_strand = next_strand;
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, CRGB(brightness, 0, 0));
    fill_solid(ih_leds, ih_LED_total, CRGB(brightness, 0, 0));
    fill_solid(oh_leds, oh_LED_total, CRGB(brightness, 0, 0));
    FastLED.show();
  }

}

// Just do theater chase animations on only inner and outer perimaters and run
// them in opposite directions
void theater_perim_opp(uint8_t theat_opt) {
  // Variable that we increment to get the actual animations
  static uint8_t pal_index;

  // Increment the palette index at a user controlled rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 100));
    pal_index++;
  }

  // Now fill the two hexagons. Options are as follows:
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Bounce
  // theat_opt == 2 : Theater Chase Mod
  if (theat_opt == 0) {
    theater_chase(ih_leds, ih_LED_total, pal_index);
    theater_chase(oh_leds, oh_LED_total, 255 - pal_index);
  }
  else if (theat_opt == 1) {
    theater_chase_tri(ih_leds, ih_LED_total, pal_index);
    theater_chase_tri(oh_leds, oh_LED_total, 255 - pal_index);
  }
  else if (theat_opt == 2) {
    theater_chase_mod(ih_leds, ih_LED_total, pal_index);
    theater_chase_mod(oh_leds, oh_LED_total, 255 - pal_index);
  }
  FastLED.show();

}

