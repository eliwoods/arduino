#include <FastLED.h>

#define LED 6
#define HEART_IN A0
#define RIGHT_IN A1
#define LEFT_IN A2

// LED stuff
const uint16_t num_heart= 30;
CRGB leds[num_led];

// Stuff for the velostat
const uint16_t vel_thresh = 30;

void setup() {
  // Serial setup
  Serial.begin(9600);

  // Setup the FastLED controller
  FastLED.addLeds<WS2812B, LED, GRB>(leds, num_led);
  FastLED.show();

  // Initialize A0 to input pullup. The applies a voltage
  // to the pin internally, so effectively our input pin
  // is also powering the sensor
  pinMode(A0, INPUT_PULLUP);
}

void loop() {
  static uint8_t led_on = 0;

  //// Check if we have are below the velostat threshold
  //if(analogRead(0) <= vel_thresh) {
  //  // Turn on each led in steps. Do this so that we don't interrupt the readings
  //  if(led_on == num_led) {
  //    fill_solid(leds, num_led, CHSV(0, 255, beatsin8(70)));
  //    FastLED.show();
  //  }
  //  else {
  //    EVERY_N_MILLISECONDS(20) {
  //      led_on++;
  //    }
  //    fill_solid(leds, led_on, CRGB::Red);
  //    FastLED.show();
  //  }
  //}
  //// If the pad isn't being pressed, rapidly turn off the leds one at a time
  //// and then keep it at black
  //else {
  //  // Reset the whole strip, otherwise this won't look right
  //  fill_solid(leds, num_led, CRGB::Black);
  //  if(led_on > 0) {
  //    EVERY_N_MILLISECONDS(5) {
  //      // Until we have turned off all the leds, rapidly turn off one
  //      // at a time
  //      led_on--;
  //      fill_solid(leds, led_on, CRGB::Red);
  //    }
  //  }
  //  // Show the strip
  //  FastLED.show();
  //}
  if(analogRead(0) <= vel_thresh) {
    fill_solid(leds, num_led, CRGB::Red);
    FastLED.show();
  }
  else {
    fill_solid(leds, num_led, CRGB::Black);
    FastLED.show();
  }
}
