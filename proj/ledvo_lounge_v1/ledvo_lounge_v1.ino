// LED library
#include <FastLED.h>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Analog Pins
#define RATE_POT 0 // Potentiometer for animation rate
#define HUE_POT 1 // Potentiometer for global hue
#define VAL_POT 2 // Potentiometer for global brightness

// Digital Pins for LED output
#define LED_OH 7
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
CRGB ih_leds[ih_LED_total];

const uint16_t oh_LED_per_strand = 4 * led_5ft_strand; // 90 LED per 5' strand with 20' hexagon sides on outside
const uint16_t oh_LED_total = 6 * oh_LED_per_strand;
CRGB oh_leds[oh_LED_total];

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
  FastLED.addLeds<WS2812B, LED_OH, GRB>(oh_leds, oh_LED_total / 2);
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

// THESE ARE THE ACTUAL FULL BODY ANIMATIONS. THESE WILL MAKE THE SLICES AND

// Flash the whole diagonal strips in various orders depending on the elapsed time. First
// flash from 0 -> 6, then flash opposite sides, then 0 -> 6 -> 0, then ranndom strips
void diagonal_flash_timed() {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;
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
      fill_solid(ih_leds, ih_LED_total, CRGB::Black);
      fill_solid(oh_leds, oh_LED_total, CRGB::Black);
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
        if (current_strand == 0) {
          pal_index += 16;
        }
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
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
        if ( current_strand == 0) {
          pal_index += 16;
        }
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(d_leds[current_strand + 3], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
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
          pal_index += 16;
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

    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
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
        pal_index += 16;
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
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

// Simple theater chase animations, except that it snakes between inner perimeter,
// diagonals, and outer perimeter. Switch the orientation every once in a while, for a little
// added pizzaz
void theater_saw(uint8_t theat_opt) {
  // Incrememnt this to get the theater chase animations going.
  static uint8_t pal_index = 0;

  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 200));
    pal_index++;
  }

  // Tell us which orientation we should be using
  static boolean orient = true;

  EVERY_N_MINUTES(2) {
    if (orient) {
      orient = false;
    }
    else {
      orient = true;
    }
    // Reset all the strands for a clean transition
    reset_all();
  }

  // Now, depending on the orientation, use the variables above to actually run the
  // animations on the selected features
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Bounce
  // theat_opt == 2 : Theater Chase Mod
  if (orient) {
    if (theat_opt == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 0) {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
        // Now set the inner hex sides
        theater_chase(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase(oh_leds, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 1) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 0) {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
        // Now set the inner hex sides
        theater_chase_tri(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_tri(oh_leds, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 2) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 0) {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
        // Now set the inner hex sides
        theater_chase_mod(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_mod(oh_leds, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
  }
  else {
    if (theat_opt == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 1) {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        // Now set the inner hex sides
        theater_chase(ih_leds, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase(oh_leds + oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 1) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 1) {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        // Now set the inner hex sides
        theater_chase_tri(ih_leds, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_tri(oh_leds + oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 1) {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        // Now set the inner hex sides
        theater_chase_mod(ih_leds, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_mod(oh_leds + oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
  }
  FastLED.show();
}

// Has the same overall pattern as the theater_saw, except that we illuminate whole sections,
// slowly building up the shape. Then we go black and build it up for the other orientation.
// Maybe we'll flash the whole shape a couple of times before switching orientations. (but not for now)
// Can switch the color every time we switch orientation.
void saw_solid() {
  // Use this to track which panel we are illuminating, since there are the same number of
  // panels regardless orientation. Also use pal_index to control the color of the illuminated
  // panels
  static uint8_t pan_index = 0;
  static uint8_t pal_index = 0;

  // Use these to track the orientation and if we should blackout the whole thing
  static boolean switched = true;
  static boolean orient = true;

  // Set everything to black if we are switching orientations, or if we are running
  // this animation for the first time
  if (switched) {
    reset_all();
    switched = false;
  }

  // Increment the panel we're supposed to illuminate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0 , 1253, 1, 200));
    pan_index = (pan_index + 1) % 12; // 12 Sides at a time
  }

  // Lets see how this looks by going from off to on without any ramping
  if (pan_index == 0) {
    fill_solid(d_leds[0], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 1) {
    if (orient) {
      fill_solid(oh_leds, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(ih_leds, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 2) {
    fill_solid(d_leds[1], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 3) {
    if (orient) {
      fill_solid(ih_leds + ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(oh_leds + oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 4) {
    fill_solid(d_leds[2], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 5) {
    if (orient) {
      fill_solid(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 6) {
    fill_solid(d_leds[3], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 7) {
    if (orient) {
      fill_solid(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 8) {
    fill_solid(d_leds[4], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 9) {
    if (orient) {
      fill_solid(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 10) {
    fill_solid(d_leds[5], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 11) {
    if (orient) {
      fill_solid(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    // Now switch the orientation and set flag to reset the strands
    if (orient) {
      orient = false;
    }
    else {
      orient = true;
    }
    switched = true;
    pal_index += 16;
  }

  FastLED.show();
}

// This animation runs theater chase in loops around alternating trapazoids made up of two diagonals and
// adjacent inner and outer hexagon panels
void theater_trap_eo(uint8_t theat_opt) {
  // Use this to iterate on the theater chase animations
  static uint8_t pal_index = 0;

  // So we know which trapazoids to illuminate
  static boolean orient = true;

  // Control the theater_chase animations at an input determined rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 150));
    pal_index++;
  }

  // Switch up the orientation every once in a while
  EVERY_N_MINUTES(1) {
    if (orient) {
      orient = false;
    }
    else {
      orient = true;
    }
    // Reset the strands whenever we switch
    reset_all();
  }

  // Now run the appropriate animation on the appropriate trapazoids
  if (orient) {
    if (theat_opt == 0) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase(ih_leds, ih_LED_per_strand, 255 - pal_index);
      theater_chase(oh_leds, oh_LED_per_strand, pal_index);

      theater_chase(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);

      theater_chase(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
    }
    if (theat_opt == 1) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_tri(ih_leds, ih_LED_per_strand, 255 - pal_index);
      theater_chase_tri(oh_leds, oh_LED_per_strand, pal_index);

      theater_chase_tri(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_tri(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);

      theater_chase_tri(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_tri(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
    }
    if (theat_opt == 2) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_mod(ih_leds, ih_LED_per_strand, 255 - pal_index);
      theater_chase_mod(oh_leds, oh_LED_per_strand, pal_index);

      theater_chase_mod(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_mod(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);

      theater_chase_mod(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_mod(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
    }
  }
  else {
    if (theat_opt == 0) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase(oh_leds + oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);
    }
    if (theat_opt == 1) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_tri(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_tri(oh_leds + oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_tri(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_tri(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_tri(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_tri(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);
    }
    if (theat_opt == 2) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_mod(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_mod(oh_leds + oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_mod(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_mod(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_mod(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_mod(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);
    }
  }
}

// Same idea as theater_trap, except that we illuminate the solid trapazoids and alternate
// the subset at a user controlled rate.
void trap_eo_solid() {
  // Keep track of which side we are illuminating and the color of them
  static uint8_t pan_index = 0;
  static uint8_t pal_index = 0;


  // Use this to select which set of animations we want to run. This way we can step through a couple
  // different variations of the solid trapazoid shit
  static uint8_t anim_set = 0;
  static uint8_t num_runs = 0; // Use this to keep track of how many times we've run one set

  // First flash individual trapezoids
  if (anim_set == 0) {
    // Control the rate at which we illuminate panels at a user controlled rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 50, 200));
      pan_index = (pan_index + 1) % 6;

      // Turn everything off each time we switch sides
      reset_all();
    }

    // Illuminate the appropriate trapazoid.
    draw_trapezoid(pan_index, pal_index);
    if (pan_index == 5) {
      // Now change the color and increment the number of runs
      pal_index += 16;
      num_runs++;
    }

    FastLED.show();
    // Check how many times we've looped over this set, and move onto the next if we're at that point
    if (num_runs == 20) {
      anim_set = 1;
      num_runs = 0;
      pan_index = 0;
      reset_all();
    }
  }
  // Flash opposing trapezoids 2x the usual amount since it runs 2x as fast
  else if (anim_set == 1) {
    // Control the rotation at a user input rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analagRead(RATE_POT), 0, 1253, 50, 200));
      pan_index = (pan_index + 1) % 3;
    }

    //

  }
  // Now turn the whole structure on one trapezoid at a time. The same idea as the previous animation
  // set except that we only run it once and we don't reset the structure after each trapezoid
  else if (anim_set == 2) {
    // Control the rate at which we illuminate panels at a user controlled rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 50, 200));
      pan_index = (pan_index + 1) % 6;
    }

    // Check which panel we have selected and illuminate the appropriate trapazoid.
    draw_trapezoid(pan_index, pal_index);
    // Now change the color and increment the number of runs if we're on the last panel
    if (pan_index == 5) {      
      pal_index += 16;
      num_runs++;
    }

    FastLED.show();
    // Check how many times we've looped over this set, and move onto the next if we're at that point
    if (num_runs == 1) {
      anim_set = 3;
      num_runs = 0;
      pan_index = 0;
      reset_all();
    }
  }
  // Now flash the whole structure on and off a few times, lets say 20?
  else if (anim_set == 3) {
    // Control flashing at a user input rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 50, 200));
      pan_index++;
    }

    if (pan_index % 2 == 0) {
      fill_all(pal_index);
    }
    else {
      reset_all();
      // Now change the color and increment the number of times we've looped over this animation set
      pal_index += 16;
      num_runs++;
    }
    FastLED.show();

    if (num_runs == 20) {
      anim_set = 4;
      pan_index = 0;
      num_runs = 0;
      reset_all();
    }
  }
  // Now flash every other set of trapezoids, alternating evens and odds for about 20x as well before reseting the whole animatino loop
  else if (anim_set == 5) {
    // Control the flashing at a user input rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 50, 200));
      pan_index++;

      // Reset the whole array every time we increment
      reset_all();
    }

    // If even, turn on all even trapezoids
    if ( pan_index % 2 == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        fill_solid(d_leds[dd], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        if (dd % 2 == 0) {
          fill_solid(ih_leds + dd * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
          fill_solid(oh_leds + dd * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        }
      }
    }
    // If odd, turn on all odd trapezoids
    else {
      for (uint8_t dd = 0; dd < 6; dd++) {
        fill_solid(d_leds[dd], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        if (dd % 2 == 1) {
          fill_solid(ih_leds + dd * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
          fill_solid(oh_leds + dd * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        }
      }
      // Increment color and run counter
      num_runs++;
      pal_index += 16;
    }

    FastLED.show();

    if (num_runs == 20) {
      anim_set = 0;
      num_runs = 0;
      pan_index = 0;
      reset_all();
    }
  }

}

