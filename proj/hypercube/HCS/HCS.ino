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
CRGBPalette16 oPalette, iPalette; // Lets try using two separate palettes for the shells
TBlendType gBlending;
uint8_t iPaletteCounter, oPaletteCounter; // Global Palette
uint8_t gIndex; // Global Palette Index
extern const uint8_t numPalettes;

// To control hue globally through a potentiometer input
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 12; //CHANGE THIS, MIGHT NOT EVEN BE NECESSARY
uint8_t chaser_opt = 0; // For choosing the sub patterns on the animations that use chasers


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
  gIndex = 0;
  gHue = 0;

  // Setup for the interrupts

}

void loop() {
  // Switch the animation according to some timer
  static uint8_t iAnimCounter = 0;
  static uint8_t oAnimCounter = 0;
  EVERY_N_SECONDS(90) {
    iAnimCounter = (iAnimCounter + 1) % 7;
  }
  EVERY_N_SECONDS(30) {
    oAnimCounter = (oAnimCounter + 1) % 7;
  }

  // Update global index for animations based on palette drawing
  EVERY_N_MILLISECONDS(5) {
    gIndex++;
  }

  // Update global hue for palettes with variability
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }

  // Update the palette selections. This also ends up running any routines to update
  // the non static palettes
  EVERY_N_SECONDS(45) {
    oPaletteCounter = (oPaletteCounter +1) % numPalettes;
  }
  EVERY_N_SECONDS(135) {
    iPaletteCounter = (iPaletteCounter + 1) % numPalettes;
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
      EVERY_N_MILLISECONDS(200) {
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
  EVERY_N_MINUTES(2) {
    run_circles = !run_circles;
  }
  merge_animations();
  if (run_circles) {
    ring_bounce_opp(20, 5);
    LEDS.show();
  }

}
