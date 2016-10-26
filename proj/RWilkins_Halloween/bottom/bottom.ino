#include <FastLED.h>

// For the pin variables
#define RIGHT_OUT 1
#define LEFT_OUT 2
#define RIGHT_IN A0
#define LEFT_IN A1

// LED stuff
const uint8_t max_leg = 34;
const uint8_t min_leg = 27;
const uint16_t num_leg = max_leg+2*min_leg;

CRGBArray<num_leg> left;
CRGBArray<max_leg> left_tmplt;

CRGBArray<num_leg> right;
CRGBArray<max_leg> right_tmplt;

// Stuff for the velostat
const uint16_t vel_thresh = 30;

// Variables for the heartbeat effect
const uint8_t max_brightness = 255;
const uint8_t bpm = 70; // A little bit faster than the average human

// We'll use these to determine what lights and how many
// are turned on
uint16_t left_on = 0;
uint16_t right_on = 0;

void setup() {
  // Serial setup
  Serial.begin(9600);

  // Setup the FastLED controller
  FastLED.addLeds<WS2812B, LEFT_OUT, GRB>(left, num_leg);
  FastLED.addLeds<WS2812B, RIGHT_OUT, GRB>(right, num_leg);
  FastLED.show();

  // Initialize velostat sensors to input pullup. The applies a voltage
  // to the pin internally, so effectively our input pin is also powering
  // the sensor
  pinMode(LEFT_IN, INPUT_PULLUP);
  pinMode(RIGHT_IN, INPUT_PULLUP);
}

void loop() {
  // General idea is that we turn on each led in steps. Do this so that we don't
  // interrupt the readings from the hands. Since the legs are in a serpentine pattern,
  // I'm using CRGBArrays so that I only have to fill on template. It's way easier that
  // directly filling the led array and just as fast (citation needed on that one you lazy fuck)

  // Reset both of the templates so the filling works properly
  fill_solid(left_tmplt, max_leg, CRGB::Black);
  fill_solid(right_tmplt, max_leg, CRGB::Black);

  // Check if the left hand is being pressed
  if(analogRead(LEFT_IN) <= vel_thresh) {
    // If it's been pressed for long enough, just fill the whole array
    if(left_on == max_leg) {
      fill_solid(left, num_leg, CHSV(0, 255, beatsin8(bpm, max_brightness/4., max_brightness)));
    }
    else {
      // Increase the number of leds turned on
      EVERY_N_MILLISECONDS(5) {
        left_on++;
      }
      fill_solid(left_tmplt, left_on, CHSV(0, 255, max_brightness));

      // "Paste" the template onto the real array that FastLED sees
      left(0, min_leg-1) = left_tmplt(0, min_leg-1);
      left(min_leg+max_leg, num_leg-1) = left_tmplt(0, min_leg-1);
      left(max_leg+min_leg-1, min_leg) = left_tmplt;
    }
  }
  // If the pad isn't being pressed, rapidly turn off the leds one at a time
  // and then keep it at black
  else {
    // Make sure that we don't go below zero. If we haven't gotten there, decrease
    // the number of leds turned on one at a time and
    if(left_on > 0) {
      EVERY_N_MILLISECONDS(5) {
        // Until we have turned off all the leds, rapidly turn off one
        // at a time
        left_on--;
        fill_solid(left_tmplt, left_on, CHSV(0, 255, max_brightness));
      }
    }

    // Paste the decreasing templates into the main array
    left(0, min_leg-1) = left_tmplt(0, min_leg-1);
    left(min_leg+max_leg, num_leg-1) = left_tmplt(0, min_leg-1);
    left(max_leg+min_leg-1, min_leg) = left_tmplt;
  }

  // Check if the right hand is being pressed
  if(analogRead(RIGHT_IN) <= vel_thresh) {
    // If it's been pressed for long enough, just fill the whole array
    if(right_on == max_leg) {
      fill_solid(right, num_leg, CHSV(0, 255, beatsin8(bpm, max_brightness/4, max_brightness)));
    }
    else {
      // Increase the number of leds turned on
      EVERY_N_MILLISECONDS(5) {
        right_on++;
      }
      fill_solid(right_tmplt, right_on, CHSV(0, 255, max_brightness));

      // "Paste" the template onto the real array that FastLED sees
      right(0, min_leg-1) = right_tmplt(0, min_leg-1);
      right(min_leg+max_leg, num_leg-1) = right_tmplt(0, min_leg-1);
      right(max_leg+min_leg-1, min_leg) = right_tmplt;
    }
  }
  // If the pad isn't being pressed, rapidly turn off the leds one at a time
  // and then keep it at black
  else {
    // Make sure that we don't go below zero. If we haven't gotten there, decrease
    // the number of leds turned on one at a time and
    if(right_on > 0) {
      EVERY_N_MILLISECONDS(5) {
        // Until we have turned off all the leds, rapidly turn off one
        // at a time
        right_on--;
        fill_solid(right_tmplt, right_on, CHSV(0, 255, max_brightness));
      }
    }

    // Paste the decreasing templates into the main array
    right(0, min_leg-1) = right_tmplt(0, min_leg-1);
    right(min_leg+max_leg, num_leg-1) = right_tmplt(0, min_leg-1);
    right(max_leg+min_leg-1, min_leg) = right_tmplt;
  }

  // Now we've done manipulations on all of the led arrays. Dump the info
  // from the controller to the strips just once
  FastLED.show();
}
