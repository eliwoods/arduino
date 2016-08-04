// LED Includes
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

// Arduino Includes
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Constants describing numbers of LEDs and strips
const uint16_t strip_len = 90; // Number of LED per strip
const uint8_t num_per_group = 6; // Number of strips per OctoWS2811 group

const uint8_t in_strips = 18; // Number of strips on inner shell
const uint8_t out_strips = 24; // Number of strips on outer shell

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
  LEDS.addLeds<OCTOWS2811>(leds, strip_len); // No need to declare pin numbers since they are preset with parallel output
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
    // First let's try filling each group with a different color based on Ethan's diagram
    fill_solid(leds(0, strip_len * num_per_group - 1), strip_len * num_per_group, CRGB::Yellow);
    fill_solid(leds(strip_len * 1 * num_per_group, strip_len * 2 * num_per_group - 1), strip_len * num_per_group, CRGB::Pink);
    fill_solid(leds(strip_len * 2 * num_per_group, strip_len * 3 * num_per_group - 1), strip_len * num_per_group, CRGB::Cyan);
    fill_solid(leds(strip_len * 3 * num_per_group, strip_len * 4 * num_per_group - 1), strip_len * num_per_group, CRGB::Green);
    fill_solid(leds(strip_len * 4 * num_per_group, strip_len * 5 * num_per_group - 1), strip_len * num_per_group, CRGB::Purple);
    fill_solid(leds(strip_len * 5 * num_per_group, strip_len * 6 * num_per_group - 1), strip_len * num_per_group, CRGB::Blue);
    fill_solid(leds(strip_len * 6 * num_per_group, strip_len * 7 * num_per_group - 1), strip_len * num_per_group, CRGB::Orange);
    fill_solid(leds(strip_len * 7 * num_per_group, strip_len * 8 * num_per_group - 1), strip_len * num_per_group, CRGB::Red);
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

  // Now lets see if we can send chasers all in one direction along each shell
  if (gAnimCounter == 2) {
    
  }

}
