// LED Includes
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

// Arduino Includes
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Digital Pins for Interrupts (may be subject to change)
#define LASER_0 0
#define LASER_1 1
#define LASER_2 17
#define LASER_3 22
#define PIEZO_0 18
#define PIEZO_1 19
// NEED TO DECIDE ON A PIN TO USE FOR LASER BREAK RESET SWITCH

#define UNUSED 13

// For describing the shells easier in code
#define INNER 0
#define OUTER 1

// Constants describing numbers of LEDs and strips
const uint16_t strip_len = 90; // Number of LED per strip
const uint8_t num_per_group = 6; // Number of strips per OctoWS2811 group

const uint8_t in_strips = 3 * num_per_group; // Number of strips on inner shell
const uint8_t out_strips = 5 * num_per_group; // Number of strips on outer shell

const uint16_t in_LED_tot = in_strips * strip_len;
const uint16_t out_LED_tot = out_strips * strip_len;
const uint16_t led_tot = in_LED_tot + out_LED_tot;

// Some templates that we can copy paste to the main LED array
CRGBArray<strip_len> led_tmplt;
CRGBArray<in_LED_tot> in_leds;
CRGBArray<out_LED_tot> out_leds;

// The array that ultimately gets pushed to the Octo Controller
CRGBArray<led_tot> leds;

// For controlling the brightness, again might not need this variability.
const uint8_t maxBrightness = 50;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette, nPalette; // These point towards the background palette that we fill the templates with
CRGBPalette16 oPalette, iPalette; // Lets try using two separate palettes for the shells
CRGBPalette16 oTargetPalette, iTargetPalette; // These are for when we switch between color palettes
boolean iPaletteSwitch, oPaletteSwitch; // So that we update the right palette
uint8_t iPaletteSwitchCount, oPaletteSwitchCount;
TBlendType gBlending;
uint8_t iPaletteCounter, oPaletteCounter, gPaletteCounter; // Global Palette
uint8_t gIndex; // Global Palette Index
extern const uint8_t numPalettes, numPalStyles;

// To control hue globally through accelerometer input
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t iNumAnimation = 7; 
const uint8_t oNumAnimation = 7; 
uint8_t iAnimCounter, oAnimCounter;
boolean iAnimSwitch, oAnimSwitch; // Use this flag so that we fade the color palette into each animation
uint8_t iAnimSwitchCount, oAnimSwitchCount; // Use this to count how many times we've faded to the 

void setup() {
  // Initialize the leds, specifially to use OctoWS2811 controller
  LEDS.addLeds<OCTOWS2811>(leds, strip_len * num_per_group); // No need to declare pin numbers since they are preset with parallel output
  LEDS.setBrightness(maxBrightness); // LETS SEE HOW FAR WE CAN PUSH IT
  LEDS.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = NOBLEND;
  iPaletteCounter = 0;
  oPaletteCounter = iPaletteCounter+3;
  iPaletteSwitch = false;
  oPaletteSwitch = false;
  iPaletteSwitchCount = 0;
  oPaletteSwitchCount = 0;

  iAnimCounter = 0;
  oAnimCounter = 0;
  iAnimSwitch = false;
  oAnimSwitch = false;
  iAnimSwitchCount = 0;
  oAnimSwitchCount = 0;
  gIndex = 0;
  gHue = 0;

  // Random number generation for the noise overlap
  random16_set_seed(analogRead(UNUSED));

  // Setup for the interrupts

}

void loop() {
  // Switch the animation according to some timer
  EVERY_N_SECONDS(30) {
    iAnimCounter = (iAnimCounter + 1) % iNumAnimation;
    iAnimSwitch = true;
  }
  EVERY_N_SECONDS(10) {
    oAnimCounter = (oAnimCounter + 1) % oNumAnimation;
    oAnimSwitch = true;
  }

  // Update global index for animations based on palette drawing
  EVERY_N_MILLISECONDS(5) {
    gIndex++;
  }

  // Update global hue for palettes with variability
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }

  // Update the global color palette. This is just the color scheme
  // that we will then repackage to have different spacial distributions
  EVERY_N_SECONDS(200) {
    gPaletteCounter = (gPaletteCounter+1)%numPalettes;
  }
  updatePaletteScheme();

  // Update the palette style for the inner and outer shells. This
  // is basically how we package the given color palette
  EVERY_N_SECONDS(45) {
    oPaletteCounter = (oPaletteCounter +1) % numPalStyles;
    oPaletteSwitch = true;
  }
  EVERY_N_SECONDS(135) {
    iPaletteCounter = (iPaletteCounter + 1) % numPalStyles;
    iPaletteSwitch = true;
  }
  updateGPalette();

  // Lets try some different combinations of things by switching between basic animations
  // on the two shells separately
  switch (iAnimCounter) {
    case 0:
      chase_straight(INNER, false);
      break;
    case 1:
      chase_spiral(INNER, 16, false);
      break;
    case 2:
      static uint8_t iOffset = 0;
      EVERY_N_MILLISECONDS(250) {
        iOffset++;
      }
      chase_spiral(INNER, iOffset, false);
      break;
    case 3:
      shell_wrap(INNER, 0, false);
      break;
    case 4:
      shell_wrap(INNER, 1, false);
      break;
    case 5:
      shell_wrap(INNER, 2, false);
      break;
    case 6:
      shell_wrap(INNER, 3, false);
      break;
    case 7:
      chase_helix(INNER, 16, false);
      break;
  }
  switch (oAnimCounter) {
    case 0:
      chase_straight(OUTER, true);
      break;
    case 1:
      chase_spiral(OUTER, 4, true);
      break;
    case 2:
      static uint8_t oOffset = 0;
      EVERY_N_MILLISECONDS(100) {
        oOffset++;
      }
      chase_spiral(OUTER, oOffset, true);
      break;
    case 3:
      shell_wrap(OUTER, 0, true);
      break;
    case 4:
      shell_wrap(OUTER, 1, true);
      break;
    case 5:
      shell_wrap(OUTER, 2, true);
      break;
    case 6:
      shell_wrap(OUTER, 3, true);
      break;
    case 7:
      chase_helix(OUTER, 4, true);
      break;
  }
  // Just to test it out for meow.
  static boolean run_circles = false;
  static boolean run_helix = false;
  EVERY_N_SECONDS(45) {
    run_circles = !run_circles;
  }
  merge_animations();
  if (run_circles) {
    ring_bounce_opp(20, 5);
    LEDS.show();
  }
  EVERY_N_SECONDS(50) {
    run_helix = !run_helix;
  }
  if (run_helix) {
    helix_spiral_overlay(20, 4);
    LEDS.show();
  }


}
