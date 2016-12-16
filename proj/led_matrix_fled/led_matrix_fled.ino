// C Includes
#include <math.h>
#include <algorithm>

// Arduino Includes
#include <FastLED.h>
//#include <EnableInterrupt.h> // Scope this for extra interrupts on the uno
#ifdef __AVR__
#include <avr/power.h>
#include <avr/interrupt.h>
#endif

// Variables for pins on the arduino board
#define LED 6
#define POW_INT 4 // IDK
#define PAL_INT 3
#define ANIM_INT 2

#define VAL_POT 0
#define RATE_POT 1

// Variables for pin interrupts
uint32_t debounce_time = 15;
volatile uint32_t last_micros; // In milliseconds
volatile boolean power_on = true; // Used for kill switch
volatile uint8_t anim = 0; // For choosing index of animation
volatile boolean anim_switch = false;

// Important Light Variables
const uint8_t numLED = 97;
const uint8_t maxCrcl = 10;
const uint8_t gBrightness = 200;
boolean *ledState = new boolean[numLED]; // Second array for holding future states (first is built in neopixel class)
CHSV *cols = new CHSV[maxCrcl]; // Global random color array
uint8_t neigh[6]; // Store nearest neighbors
uint16_t global_delay;

// Variables for fastled
CRGBArray<numLED> leds;

void setup() {
  // Safety first
  delay(2000);

  // Utility setup
  randomSeed(analogRead(5));

  // Initialize pixelStates to off
  for (int8_t i = 0; i < numLED; i++) {
    ledState[i] = false;
  }

  // Interrupts
  pinMode(POW_INT, INPUT_PULLUP);

  pinMode(PAL_INT, INPUT_PULLUP);

  pinMode(ANIM_INT, INPUT_PULLUP);

  // Initialize array of random colors to be cycled.
  for (uint8_t col = 0; col < maxCrcl; col++) {
    cols[col] = CHSV(random8(), 255, gBrightness);
  }

  // FastLED setup
  FastLED.addLeds<WS2812B, LED, GRB>(leds, numLED);
  FastLED.show();
}

// Just for testing functions ATM
void loop() {

  /////////// SOME EXAMPLES ///////////////////
  // YOU'LL NEED TO UNCOMMENT THE LINES TO RUN THEM.
  //////////////////////////////////////////////

  global_delay = 200;

  //  spiral(wheel(random(256)), global_delay, 1, false, true);
  //  ripple_smooth(global_delay, 1, false);
  //  ripple_single_rev(global_delay, 1);
  //  random_walk(wheel(random(256)), global_delay, false, false);
  //  game_of_life(global_delay, .3, false);

}
