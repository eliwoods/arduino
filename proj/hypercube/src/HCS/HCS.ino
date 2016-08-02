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
  // put your main code here, to run repeatedly:

}
