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
#define DJ_POT 3 // Potentiometer for strobing DJ effects

// Digital Pins for LED output
#define LED_IH 8
#define LED_OH 9
#define LED_D0 10
#define LED_D1 11
#define LED_D2 12

// Digital Pins for interrupts
#define KILL_INT 42 // Button to turn on or off the lights
#define PAL_INC_INT 44 // Palette incrementing interrupt
#define PAL_DEC_INT 48 // Palette decrementing interrupt
#define PAL_AUTO_INT 52 // Autopilot palette mode interrupt
#define ANIM_INC_INT 45 // Animation incrementing interrupt
#define ANIM_DEC_INT 49 // Animation decrementing interrupt
#define ANIM_AUTO_INT 53 // Animation autopilot interrupt
#define STROBE_INT 50 // Strobe animation interrupt
#define BLKSTROBE_INT 51 // Black strobe interrupt
#define REVERS_INT 43 // Reversing animations interrupt

// Variables for the LED strand
const uint8_t maxBrightness = 150;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Variables for LED strands
const uint16_t led_strand = 90; // The size of one module
CRGBArray<led_strand> led_tmplt; // Used for animations that we just replicate across the array
CRGBArray<2 * led_strand> led_tmplt_2; // When we need to replicate 2 strips
CRGBArray<4 * led_strand> led_tmplt_4; // When we need to replicate 4 strips

const uint8_t  ih_num_strand = 6;
const uint16_t ih_LED_total = ih_num_strand * led_strand;
CRGBArray<ih_LED_total> ih_leds;

const uint8_t oh_num_strand = 12;
const uint16_t oh_LED_total = oh_num_strand * led_strand;
CRGBArray<oh_LED_total> oh_leds;

const uint8_t num_diag = 3;
const uint8_t d_num_strand_tot = 2; // 90 LED per 5' strand with 10' diagonals
const uint16_t d_LED_num = d_num_strand_tot * led_strand;
CRGBArray<d_LED_num> d_leds[num_diag];

// Variables for pin interrupts. There's a lot of these babies ;^)
uint32_t debounce_time = 100;
volatile uint32_t last_micros; // In milliseconds
volatile boolean power_on = true; // Used for kill switch
volatile boolean power_switched = false;
volatile uint8_t gPaletteCounter; // For choosing index of palette
volatile boolean palette_autopilot = false; // Flag for auto cycling palettes
volatile uint8_t gAnimCounter; // Counter for animation
volatile boolean anim_autopilot = false; // Flag for auto cycling animations
volatile boolean anim_switch = false;
volatile boolean anim_reverse = false; // Flag for running animations in reverse
volatile boolean dj_control = false; // Flag if dj is controlling animations
volatile boolean run_strobe = false; // Flag for strobe interrupt animation
volatile boolean run_blkstrobe = false; // Flag for black strobing

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette; // This is the palette that all animations look at
CRGBPalette16 gScheme, nScheme; // These hold the actual color scheme that gets transformed into gPalette
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index
uint8_t gSchemeNum = 0; // For keeping track of color scheme
extern const uint8_t numPalettes, numPalStyles;

// To control hue globally through a potentiometer input
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 8;
uint8_t chaser_opt = 0; // For choosing the sub patterns on the animations that use chasers

void setup() {
  delay(3000); // Safely power up

  // Setup Random Number Generator
  random16_set_seed(4832);
  random16_add_entropy(analogRead(5));

  // Setup strands of LEDs
  FastLED.addLeds<WS2812B, LED_IH, GRB>(ih_leds, ih_LED_total);
  FastLED.addLeds<WS2812B, LED_OH, GRB>(oh_leds, oh_LED_total);
  FastLED.addLeds<WS2812B, LED_D0, GRB>(d_leds[0], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D1, GRB>(d_leds[1], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D2, GRB>(d_leds[2], d_LED_num);
  FastLED.setBrightness(maxBrightness);
  FastLED.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = LINEARBLEND;
  gIndex = 0;
  gHue = 0;
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
  attachInterrupt(digitalPinToInterrupt(BLKSTROBE_INT), debounce_blk_strobe, CHANGE);

  // Interrupt for reversing animation direction. Look for change since this will
  // be a latching button
  pinMode(REVERS_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REVERS_INT), debounce_anim_reverse, CHANGE);

  pinMode(KILL_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KILL_INT), debounce_kill_power, CHANGE);
}

void loop() {
  // Read in global brightness value
  if (power_on) {
    gBrightness = map(analogRead(VAL_POT), 0, 1023, 0, maxBrightness);

    // Read color from potentiometer input
    gHue = map(analogRead(HUE_POT), 0, 1023, 0, 255);

    // Check the palette counter and switch acordingly or go into autopilot mode. Also run
    // some checks that we aren't using palettes with black when we fill whole bars
    if (palette_autopilot) {
      EVERY_N_SECONDS(200) {
        gPaletteCounter = (gPaletteCounter + 1) % numPalStyles;
        if (gPaletteCounter == 0) {
          gSchemeNum = (gSchemeNum + 1) % numPalettes;
        }
      }
      // Keep us from selecting certain palettes for certain animatinos
      if (gAnimCounter >= 3 && gAnimCounter <= 6) {
        while (gPaletteCounter == 3 || gPaletteCounter == 6 ) {
          gPaletteCounter = (gPaletteCounter + 1) % numPalettes;
        }
      }
      if (gAnimCounter == 2 || gAnimCounter ==  7) {
        while (gPaletteCounter == 0) {
          gPaletteCounter++;
        }
      }
      // Now update the palette with our choice
      updatePaletteScheme();
      updateGPalette();
    }
    else {
      // Keep us from selecting certain palettes for certain animatinos
      if (gAnimCounter >= 3 && gAnimCounter <= 6) {
        while (gPaletteCounter == 3 || gPaletteCounter == 6 ) {
          gPaletteCounter = (gPaletteCounter + 1) % numPalettes;
        }
      }
      if (gAnimCounter == 2 || gAnimCounter ==  7) {
        while (gPaletteCounter == 0) {
          gPaletteCounter++;
        }
      }
      // Now update the palette with our choice
      updatePaletteScheme();
      updateGPalette();
    }

    // Check if we want to autopilot the animations
    if (anim_autopilot) {
      EVERY_N_MINUTES(5) {
        gAnimCounter = (gAnimCounter + 1) % numAnimation;
        anim_switch = true;
      }
    }

    // Update the chaser options if we are one of those animations
    EVERY_N_SECONDS(60) {
      if (gAnimCounter == 2) {
        chaser_opt = (chaser_opt + 1) % 9;
      }
      else if (gAnimCounter == 6 || gAnimCounter == 7) {
        chaser_opt = (chaser_opt + 1) % 6;
      }
      else if (gAnimCounter == 11) {
        chaser_opt = (chaser_opt + 1) % 3;
      }
    }

    // For flashing black over current animation. We want it to work
    // with the animation so we want it separate from the DJ controls
    if (run_blkstrobe) {
      strobe_black();
    }

    // Reset the data on the strips if we are getting a new animation
    if (anim_switch) {
      reset_all();
      anim_switch = false;
    }

    // Select animation to run based on global counter
    if (!dj_control) {
      if (gAnimCounter == 0) {
        fill_all();
      }
      else if (gAnimCounter == 1) {
        fill_all_grad();
      }
      else if (gAnimCounter == 2) {
        theater_perim_opp(chaser_opt);
      }
      else if (gAnimCounter == 3) {
        whole_eq_overlay();
      }
      else if (gAnimCounter == 4) {
        whole_eq_3();
      }
      else if (gAnimCounter == 5) {
        diagonal_flash_timed();
      }
      else if (gAnimCounter == 6) {
        trap_solid();
      }
      else if (gAnimCounter == 7) {
        all_trap_chaser(chaser_opt);
      }
    }

    // The following are all checks for DJ animations that
    // interrupt the normal animations for some added IN YO FACE
    if (dj_control) {
      if (run_strobe) {
        strobes();
      }
    }
  }
  else {
    reset_all();
    FastLED.show();
    FastLED.delay(1000);
  }
}
