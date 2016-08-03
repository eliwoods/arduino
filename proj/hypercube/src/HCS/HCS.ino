// LED Includes
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

// Arduino Includes
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Variables for the LED strips
const uint16_t led_per_strand = 90;
const uint8_t strands_per_group = 6;

const uint8_t in_strands = 18; // Number of strips on inner shell
const uint8_t out_strands = 24; // Number of strips on outer shell

CRGBArray<led_per_strand> led_tmplt;
CRGBArray < led_per_strand*(in_strands + out_strands) > leds;

const uint16_t in_LED_tot = in_strands * led_per_strand;
CRGBArray<in_LED_tot> in_leds;

const uint16_t out_LED_tot = out_strands * led_per_strand;
CRGBArray<out_LED_tot> out_leds;

// For controlling the brightness, again might not need this variability.
const uint8_t maxBrightness = 150;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette;
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index
extern const uint8_t numPalettes;

// To control hue globally through a potentiometer input
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 12; //CHANGE THIS, MIGHT NOT EVEN BE NECESSARY
uint8_t chaser_opt = 0; // For choosing the sub patterns on the animations that use chasers


void setup() {
  // Initialize the leds, specifially to use OctoWS2811 controller
  LEDS.addLeds<OCTOWS2811>(leds, led_per_strand); // No need to declare pin numbers since they are preset with parallel output
  LEDS.setBrightness(150);
  LEDS.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = LINEARBLEND;
  gIndex = 0;
  gHue = 0;

}

void loop() {
  static uint8_t gAnimCounter = 0;
  EVERY_N_SECONDS(10) {
    gAnimCounter = (gAnimCounter + 1) % 3;
  }

  if (gAnimCounter == 0) {
    // First let's try filling each group with a different color
    fill_solid(leds(0, led_per_strand * strands_per_group - 1), led_per_strand * strands_per_group, CRGB::Yellow);
    fill_solid(leds(leds_per_strand * 1 * strands_per_group, leds_per_strand * 2 * strands_per_group - 1), led_per_strand * strands_per_group, CRGB::Pink);
    fill_solid(leds(leds_per_strand * 2 * strands_per_group, leds_per_strand * 3 * strands_per_group - 1), leds_per_strand * strands_per_group, CRGB::Cyan);
    fill_solid(leds(leds_per_strand * 3 * strands_per_group, leds_per_strand * 4 * strands_per_group - 1), leds_per_strand * strands_per_group, CRGB::Green);
    fill_solid(leds(leds_per_strand * 4 * strands_per_group, leds_per_strand * 5 * strands_per_group - 1), leds_per_strand * strands_per_group, CRGB::Purple);
    fill_solid(leds(leds_per_strand * 5 * strands_per_group, leds_per_strand * 6 * strands_per_group - 1), leds_per_strand * strands_per_group, CRGB::Blue);
    fill_solid(leds(leds_per_strand * 6 * strands_per_group, leds_per_strand * 7 * strands_per_group - 1), leds_per_strand * strands_per_group, CRGB::Orange);
    fill_solid(leds(leds_per_strand * 7 * strands_per_group, leds_per_strand * 8 * strands_per_group - 1), leds_per_strand * strands_per_group, CRGB::Red);
    LEDS.show();
  }

  // Now try filling the inner and outer shells two different colors
  if (gAnimCounter == 1) {
    fill_solid(in_leds, in_LED_tot, CRGB::Red);
    fill_solid(out_leds, out_LED_tot, CRGB::Blue);
    leds(0, in_LED_tot-1) = in_leds;
    leds(in_LED_tot, in_LED_tot+out_LED_tot-1) = out_leds;
    LEDS.show();
  }

  // Now lets see if we
  if (gAnimCounter == 2) {
    
  }

}
