#include <FastLED.h>

#ifndef __AVR__
#include <avr/interrupt.h>
#endif

// Analog Inputs for Photoresistors (doesn't actually do shit)
#define RES0 0
#define RES1 1
#define RES2 2
#define RES3 3

// Digital Pins (I/O)
#define LASER_POWER 3

// Interrupts
#define POWER_INT 2
volatile boolean power_on = true;
volatile uint32_t last_micros;
uint32_t debounce_time = 100;

// FastLED Shit (just for testing it works for now)
#define LED_IN 7
#define NUMLED 90

CRGB leds[NUMLED];

// Some useful global variables for stuff we want
const uint16_t threshold = 500; //This is totally arbitrary, will need some tuning.
boolean triggered[] = {false, false, false, false};

void setup() {
  // FastLED Setup
  FastLED.addLeds<WS2812B, LED_IN, GRB>(leds, NUMLED);
  FastLED.setBrightness(100);
  FastLED.show();

  // Arduino Pin Setup
  pinMode(LASER_POWER, OUTPUT);

  // Interrupt for power control
  pinMode(POWER_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(POWER_INT), debounce_power, CHANGE);

}

void loop() {
  // If the power is on, then power the lasers (duh). Also run everything else, so that
  // we don't pick up noise when it should be off
  if (power_on) {
    digitalWrite(LASER_POWER, HIGH);

    // Check the value of each of the analog inputs. If they are above the threshold, then
    // trigger the send the pins number out of the arduino. This will be interpretted by
    // the teensy that is actually running all this shit.
    for (uint8_t ch = 0; ch < 4; ch++) {
      // Check if we have already triggered this channel, this is incase someone
      // is holding their hand in front of the lasers or something dumb like that.
      if (!triggered[ch]) {
        if (analogRead(ch) < threshold) {
          triggered[ch] = true;
          // So that we can visualize it on the test strip
          if (ch == 0) {
            fill_solid(leds, NUMLED, CRGB::Red);
            FastLED.show();
          }
          else if (ch == 1) {
            fill_solid(leds, NUMLED, CRGB::Blue);
            FastLED.show();
          }
          else if (ch == 2) {
            fill_solid(leds, NUMLED, CRGB::Green);
            FastLED.show();
          }
          else if (ch == 3) {
            fill_solid(leds, NUMLED, CRGB::White);
            FastLED.show();
          }
        }
      }
      else {
        if (analogRead(ch) > threshold) {
          triggered[ch] = false;
        }
      }
    } // End loop over channels
    
  } //End of power check
  else {
    digitalWrite(LASER_POWER, LOW);
  }
}
