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
#define HUE_POT 1 // Potentiometer for global hue
#define VAL_POT 2 // Potentiometer for global brightness

// Digital Pins for LED output
#define LED_IH 25
#define LED_OH 26
#define LED_D0 27
#define LED_D1 28
#define LED_D2 14
//#define LED_D3 15
//#define LED_D4 29
//#define LED_D5 11

// Digital Pins for interrupts
// Digital Pins for interrupts
#define KILL_INT 42 // Button to turn on or off the lights
#define PAL_INC_INT 44 // Palette incrementing interrupt
#define PAL_DEC_INT 48 // Palette decrementing interrupt
#define PAL_AUTO_INT 52 // Autopilot palette mode interrupt
#define ANIM_INC_INT 45 // Animation incrementing interrupt
#define ANIM_DEC_INT 49 // Animation decrementing interrupt
#define ANIM_AUTO_INT 53 // Animation autopilot interrupt
#define STROBE_INT 50 // Strobe animation interrupt
#define BLKSTROBE_INT 51 // Black strobe interrupt
#define REVERS_INT 43 // Reversing animations interrupt
// unused 46
//unused 47

// Variables for the LED strand
const uint8_t frameRate = 100; // FPS
const uint8_t maxBrightness = 100;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Variables for LED strands
const uint16_t led_5ft_strand = 90; // The size of one module

const uint16_t ih_LED_per_strand = 2*led_5ft_strand; // 90 LED per 5' strand with 10' hexagon sides on the inside
const uint16_t ih_LED_total = 3*ih_LED_per_strand;
CRGBArray<ih_LED_total> ih_leds;

const uint16_t oh_LED_per_strand = 4*led_5ft_strand; // 90 LED per 5' strand with 20' hexagon sides on outside
const uint16_t oh_LED_total = 3*oh_LED_per_strand;
CRGBArray<oh_LED_total> oh_leds;

const uint16_t d_LED_num = 2*led_5ft_strand; // 90 LED per 5' strand with 10' diagonals
CRGBArray<d_LED_num> d_leds[3];

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

  // Setup Random Number Generator
  random16_set_seed(4832);
  random16_add_entropy(analogRead(5));

  // Setup strands of LEDs
  FastLED.addLeds<WS2812B, LED_IH, GRB>(ih_leds, ih_LED_total);
  FastLED.addLeds<WS2812B, LED_OH, GRB>(oh_leds, oh_LED_total);
  FastLED.addLeds<WS2812B, LED_D0, GRB>(d_leds[0], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D1, GRB>(d_leds[1], d_LED_num);
  FastLED.addLeds<WS2812B, LED_D2, GRB>(d_leds[2], d_LED_num);
//  FastLED.addLeds<WS2812B, LED_D3, GRB>(d_leds[3], d_LED_num);
//  FastLED.addLeds<WS2812B, LED_D4, GRB>(d_leds[4], d_LED_num);
//  FastLED.addLeds<WS2812B, LED_D5, GRB>(d_leds[5], d_LED_num);
  FastLED.setBrightness(maxBrightness);
  FastLED.show();

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
  attachInterrupt(digitalPinToInterrupt(BLKSTROBE_INT), debounce_blk_strobe, CHANGE);

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
    gBrightness = map(analogRead(VAL_POT), 0, 687, 0, maxBrightness);

    // Read color from potentiometer input
    gRGB = CHSV(map(analogRead(HUE_POT), 0, 687, 0, 255), 255, gBrightness);

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
      if(gAnimCounter == 0) {
          theater_chase();
      }
      else if(gAnimCounter == 1) {
          fill_all_pal();
      }
      else if (gAnimCounter == 2) {
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
    reset_all();
    FastLED.show();
    FastLED.delay(1000);
  }
}
