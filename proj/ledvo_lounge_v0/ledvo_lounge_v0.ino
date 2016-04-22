// LED libraries
#include <FastLED.h>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Pin Variables
#define LED_IN 6
#define S_POT 0
#define ANIM_INT 2

// Variables for the LED strand
const uint8_t maxBrightness = 100;
const uint8_t numLED = 30;
CRGB *leds = new CRGB[numLED];
uint8_t gHue = 0;

// So that we can

void setup() {
  delay(3000); // Safely power up

  // Setup serial for debugging
  Serial.begin(57600);

  // Setup Random Number Generator
  random16_set_seed(4832);
  random16_add_entropy(analogRead(5));

  // Setup strands of LED
  FastLED.addLeds<NEOPIXEL, LED_IN>(leds, numLED);
  FastLED.setBrightness(maxBrightness);
  FastLED.show();

  // Setup Interrupts
  //attachInterrupt(
}

void loop() {
  //theater_chase_mod();
  rainbow_mod_bounce();
}

// Simple rainbow modulation shit, but its just so sleek with the FastLED ENM.
void rainbow_mod() {
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(S_POT), 0, 1253, 1, 200));
  }
  fill_rainbow(leds, numLED, 10);
  FastLED.show();
}

// rainbow_mod with a packet of white that bounces back and forth
void rainbow_mod_bounce() {
  static uint8_t pkt = 0; // Only declared once(?)
  uint8_t del = map(analogRead(S_POT), 0, 1253, 1, 200);

  // These should led us update the background palette at a different speed
  // than the packet is traveling.
  EVERY_N_MILLISECONDS(1) {
    gHue++;
  }
  // This version of EVERY_N_MILLISECONDS lets us set the timebase and change
  // it on the fly
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(S_POT), 0, 1253, 1, 200));
    pkt = (pkt + 1) % (2 * numLED);
  }

  // First fill whole strip with the color palette (rainbow for now)
  fill_rainbow(leds, numLED, gHue, 10);

  // Now fill a small packet of 3 white led lights. Do some checks at the ends
  // so that we don't run into overflows and underflows.

  // Forward Direction
  if (pkt < numLED) {
    if (pkt == 0) {
      fill_solid(leds, 2, CRGB::White);
    }
    else if (pkt == numLED - 1) {
      fill_solid(leds - 1 + pkt, 2, CRGB::White);
    }
    else {
      fill_solid(leds - 1 + pkt, 3, CRGB::White);
    }
  }
  // Now Backwards
  else {
    if ((2*numLED - pkt) == 0) {
      fill_solid(leds, 2, CRGB::White);
    }
    else if (pkt - numLED == numLED - 1) {
      fill_solid(leds - 1 + (2*numLED - pkt), 2, CRGB::White);
    }
    else {
      fill_solid(leds - 1 + (2*numLED - pkt), 3, CRGB::White);
    }
  }

  FastLED.show();

}

// Theater chase where packets of light grow and shrink as they go along. I don't think this is really
// what's going on in the animation but that's life.
void theater_chase_mod() {
  uint8_t max_spacing = 20;
  //for(uint8_t col = 0; col < 256;
  for (uint8_t i = 1; i < max_spacing; i++) {
    if (i < max_spacing / 2) {
      for (uint8_t pkt = 0; pkt < numLED; pkt = pkt + i) {
        leds[pkt + i - 1] = CRGB::Red;
      }
    }
    else {
      for (uint8_t pkt = 0; pkt < numLED; pkt = pkt + (max_spacing - i)) {
        leds[pkt + (max_spacing - i - 1)] = CRGB::Red;
      }
    }
    FastLED.show();
    delay(200);
    if (i < max_spacing / 2) {
      for (uint8_t pkt = 0; pkt < numLED; pkt = pkt + i) {
        leds[pkt + i - 1] = CRGB::Black;
      }
    }
    else {
      for (uint8_t pkt = 0; pkt < numLED; pkt = pkt + (max_spacing - i)) {
        leds[pkt + (max_spacing - i - 1)] = CRGB::Black;
      }
    }
  }
}

