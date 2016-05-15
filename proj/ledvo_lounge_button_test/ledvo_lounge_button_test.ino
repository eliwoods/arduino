// LED library
#include <FastLED.h>

// C++ Libraries (yay!)
//#include <StandardCplusplus.h>
//#include <vector>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Analog Pins
#define RATE_POT 0 // Potentiometer for animation rate
#define HUE_POT 0 // Potentiometer for global hue
#define VAL_POT 0 // Potentiometer for global brightness


// Digital Pins for interrupts
#define KILL_INT 18 // Button to turn on or off the lights
#define PAL_INC_INT 14 // Palette incrementing interrupt
#define PAL_DEC_INT 15 // Palette decrementing interrupt
#define PAL_AUTO_INT 18 // Autopilot palette mode interrupt
#define ANIM_INC_INT 3 // Animation incrementing interrupt
#define ANIM_DEC_INT 3 // Animation decrementing interrupt
#define ANIM_AUTO_INT 3 // Animation autopilot interrupt
#define STROBE_INT 16 // Strobe animation interrupt
#define BLKSTROBE_INT 3 // Black strobe interrupt
#define REVERS_INT 17 // Reversing animations interrupt


// Variables for the LED strand
const uint8_t frameRate = 100; // FPS
const uint8_t maxBrightness = 200;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

const uint8_t num_strip = 8;
const uint8_t numLED = 90;

CRGB leds[num_strip * numLED];

// Variables for pin interrupts. There's a lot of these babies ;^)
uint32_t debounce_time = 15;
volatile uint32_t last_micros; // In milliseconds
volatile boolean power_on = true; // Used for kill switch
volatile boolean power_switched = false;
volatile uint8_t gPaletteCounter; // For choosing index of palette
volatile boolean palette_autopilot = false; // Flag for auto cycling palettes
volatile uint8_t gAnimCounter; // Counter for animation
volatile boolean anim_autopilot = false; // Flag for auto cycling animations
volatile boolean anim_reverse = false; // Flag for running animations in reverse
volatile boolean dj_control = false; // Flag if dj is controlling animations
volatile boolean run_strobe = false; // Flag for strobe interrupt animation
volatile boolean run_blkstrobe = false; // Flag for black strobing

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette;
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index
extern const uint8_t numPalettes;
extern const TProgmemPalette16 WhiteBlack_p PROGMEM;
extern const TProgmemPalette16 WhiteBlack2_p PROGMEM;

// To control hue globally through a potentiometer input
CRGB gRGB;
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 3;

void setup() {
  delay(3000); // Safely power up

  // Setup serial for debugging
  Serial.begin(57600);

  // Setup Random Number Generator
  random16_set_seed(4832);
  random16_add_entropy(analogRead(5));

  // Setup strands of LEDs
  LEDS.addLeds<WS2811_PORTD, num_strip>(leds, numLED);
  LEDS.setBrightness(maxBrightness);
  LEDS.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = LINEARBLEND;
  gHue = 0;
  gIndex = 0;
  gRGB = CRGB::Red;
  gPaletteCounter = 0;
  gAnimCounter = 0;

  // Interrupts to switch palette choice
  pinMode(PAL_INC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAL_INC_INT), debounce_palette_inc, RISING);

  pinMode(PAL_DEC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAL_DEC_INT), debounce_palette_dec, RISING);

  // Interrupt to turn on and off palette autopilot mode. Use change since the button
  // will be latching
  pinMode(PAL_AUTO_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAL_AUTO_INT), debounce_palette_auto, CHANGE);

  // Interrupts to switch animation
  pinMode(ANIM_INC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANIM_INC_INT), debounce_anim_inc, RISING);

  pinMode(ANIM_DEC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANIM_DEC_INT), debounce_anim_dec, RISING);

  // Interrupt to turn on and off animation autopilot mode. Use change since button
  // will be latching.
  pinMode(ANIM_AUTO_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANIM_AUTO_INT), debounce_anim_auto, CHANGE);

  // Interrupt for strobe DJ animation. Use change so animation starts on button press
  // and ends on button release
  pinMode(STROBE_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(STROBE_INT), debounce_strobe, CHANGE);

  // Interrupt for strobing black over current animation.
  pinMode(BLKSTROBE_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLKSTROBE_INT), debounce_blk_strobe, RISING);

  // Interrupt for reversing animation direction. Look for change since this will
  // be a latching button
  pinMode(REVERS_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REVERS_INT), debounce_anim_reverse, CHANGE);

  pinMode(KILL_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KILL_INT), debounce_kill_power, CHANGE);
}

void loop() {
  // Read in global brightness value
  if (power_on) {
    gBrightness = map(analogRead(VAL_POT), 0, 1253, 0, maxBrightness);

    // Read color from potentiometer input
    gRGB = CHSV(map(analogRead(HUE_POT), 0, 1253, 0, 255), 255, gBrightness);

    // Increment global hue index for extra groovy rainbows
    EVERY_N_MILLISECONDS(10) {
      gHue++;
    }

    // Check the palette counter and switch acordingly or go into autopilot mode
    if (palette_autopilot) {
      EVERY_N_SECONDS(5) {
        gPaletteCounter = (gPaletteCounter + 1) % numPalettes;
      }
      updateGPalette();
    }
    else {
      updateGPalette();
    }

    // Check if we want to autopilot the animations
    if (anim_autopilot) {
      EVERY_N_SECONDS(30) {
        gAnimCounter = (gAnimCounter + 1) % numAnimation;
      }
    }

    // For flashing black over current animation. We want it to work
    // with the animation so we want it separate from the DJ controls
    if (run_blkstrobe) {
      strobe_black();
    }

    // Select animation to run based on global counter
    if (!dj_control) {
      switch ( gAnimCounter ) {
        case 0:
          theater_chase();
        case 1:
          theater_chase_tri();
        case 2:
          theater_chase_mod();
      }
    }

    // The following are all checks for DJ animations that
    // interrupt the normal animations for some added IN YO FACE
    if (dj_control) {
      if (run_strobe) {
        strobes();
      }
    }
  }
  else {
    if(power_switched) {
      reset_all();
      LEDS.show();
      power_switched = false;
    }
    // Do nothing.
  }
}
