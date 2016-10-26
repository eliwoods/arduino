#include <FastLED.h>

// For the pin variables
#define HEART_OUT 1
#define RIGHT_OUT 2
#define LEFT_OUT 3
#define HEART_IN A0
#define RIGHT_IN A1
#define LEFT_IN A2

// LED stuff
const uint16_t num_heart= 124;
CRGBArray<num_heart> heart;

const uint8_t max_arm = 34;
const uint8_t min_arm = 27;
const uint16_t num_arm = max_arm+2*min_arm;

CRGBArray<num_arm> left;
CRGBArray<max_arm> left_tmplt;

CRGBArray<num_arm> right;
CRGBArray<max_arm> right_tmplt;

// Stuff for the velostat
const uint16_t vel_thresh = 30;

// Variables for the heartbeat effect
uint8_t heart_bright = 0;
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
  FastLED.addLeds<WS2812B, HEART_OUT, GRB>(heart, num_heart);
  FastLED.addLeds<WS2812B, LEFT_OUT, GRB>(left, num_arm);
  FastLED.addLeds<WS2812B, RIGHT_OUT, GRB>(right, num_arm);
  FastLED.show();

  // Initialize velostat sensors to input pullup. The applies a voltage
  // to the pin internally, so effectively our input pin is also powering
  // the sensor
  pinMode(LEFT_IN, INPUT_PULLUP);
  pinMode(RIGHT_IN, INPUT_PULLUP);
}

void loop() {
  // General idea is that we turn on each led in steps. Do this so that we don't
  // interrupt the readings from the hands. Since the arms are in a serpentine pattern,
  // I'm using CRGBArrays so that I only have to fill on template. It's way easier that
  // directly filling the led array and just as fast (citation needed on that one you lazy fuck)

  // Reset both of the templates so the filling works properly
  fill_solid(left_tmplt, max_arm, CRGB::Black);
  fill_solid(right_tmplt, max_arm, CRGB::Black);

  // Check if the left hand is being pressed
  if(analogRead(LEFT_IN) <= vel_thresh) {
    // If it's been pressed for long enough, just fill the whole array
    if(left_on == max_arm) {
      fill_solid(left, num_arm, CHSV(0, 255, beatsin8(bpm, max_brightness/4., max_brightness)));
    }
    else {
      // Increase the number of leds turned on
      EVERY_N_MILLISECONDS(5) {
        left_on++;
      }
      fill_solid(left_tmplt, left_on, CHSV(0, 255, max_brightness));

      // "Paste" the template onto the real array that FastLED sees
      left(0, min_arm-1) = left_tmplt(0, min_arm-1);
      left(min_arm+max_arm, num_arm-1) = left_tmplt(0, min_arm-1);
      left(max_arm+min_arm-1, min_arm) = left_tmplt;
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
    left(0, min_arm-1) = left_tmplt(0, min_arm-1);
    left(min_arm+max_arm, num_arm-1) = left_tmplt(0, min_arm-1);
    left(max_arm+min_arm-1, min_arm) = left_tmplt;
  }

  // Check if the right hand is being pressed
  if(analogRead(RIGHT_IN) <= vel_thresh) {
    // If it's been pressed for long enough, just fill the whole array
    if(right_on == max_arm) {
      fill_solid(right, num_arm, CHSV(0, 255, beatsin8(bpm, max_brightness/4, max_brightness)));
    }
    else {
      // Increase the number of leds turned on
      EVERY_N_MILLISECONDS(5) {
        right_on++;
      }
      fill_solid(right_tmplt, right_on, CHSV(0, 255, max_brightness));

      // "Paste" the template onto the real array that FastLED sees
      right(0, min_arm-1) = right_tmplt(0, min_arm-1);
      right(min_arm+max_arm, num_arm-1) = right_tmplt(0, min_arm-1);
      right(max_arm+min_arm-1, min_arm) = right_tmplt;
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
    right(0, min_arm-1) = right_tmplt(0, min_arm-1);
    right(min_arm+max_arm, num_arm-1) = right_tmplt(0, min_arm-1);
    right(max_arm+min_arm-1, min_arm) = right_tmplt;
  }

  // Check if the heart circuit has been close
  if(analogRead(HEART_IN) == 1023) {
    // If the heart isn't at max brightness, ramp it up in increments
    // of 10 at the same rate that the veins light up;
    if(heart_bright < max_brightness) {
      EVERY_N_MILLISECONDS(5) {
        heart_bright = (heart_bright +  max_brightness/10)%max_brightness;
        // Incase we increment over the max_brightness limit
        if(heart_bright == 0) {
          heart_bright == max_brightness;
        }
      }
      fill_solid(heart, num_heart, CHSV(0, 255, heart_bright));
    }
    // If we're at max brightness, oscillate the brightness to mimic 
    // a heartbeat
    else {
      fill_solid(heart, num_heart, CHSV(0, 255, beatsin8(bpm, max_brightness/4, max_brightness)));
    }
  }
  // If the heart circuit isn't closed, fade it slowly until it's off
  else {
    if(heart_bright > 0) {
      EVERY_N_MILLISECONDS(5) {
        heart_bright--;
      }
      fill_solid(heart, num_heart, CHSV(0, 255, heart_bright));
    }
  }

  // Now we've done manipulations on all of the led arrays. Dump the info
  // from the controller to the strips just once
  FastLED.show();
}
