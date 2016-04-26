// LED libraries
#include <FastLED.h>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Pin Variables
#define LED_IN 6
#define S_POT 0 // Potentiometer for animation rate
#define C_POT 0 // Potentiometer for global hue
#define PAL_INT 2 // Interrupt pin for palette switching
//#define PAL_AUTO_INT 3 // Interrupt for autopilot palette mode
#define ANIM_INT 3 // Interrupt pin for animation switching

// Variables for the LED strand
const uint8_t frameRate = 30; // FPS
const uint8_t maxBrightness = 200;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER
const uint8_t numLED = 30;
CRGB *leds = new CRGB[numLED];

// Variables for pin interrupts
uint32_t debounce_time = 15;
volatile uint32_t last_micros; // In milliseconds
volatile boolean power_state = true; // Used for kill switch
volatile uint8_t gPaletteCounter = 0; // For choosing index of palette
volatile boolean palette_autopilot = false; // For choosing
volatile uint8_t gAnimCounter; // Counter for animation

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette;
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index
uint8_t gHue; // Global Hue Index

extern const uint8_t numPalettes;
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const TProgmemPalette16 WhiteBlack_p PROGMEM;

// To control hue globally through a potentiometer input
CRGB gRGB;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 6;


// For animation counter

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

  // Initialize global color variables
  gBrightness = maxBrightness;
  gPalette = gGradientPalettes[0];
  gBlending = LINEARBLEND;
  gHue = 0;
  gIndex = 0;
  gRGB = CRGB::Red;
  gAnimCounter = 0;

  // Interrupt to switch palette choice
  pinMode(PAL_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PAL_INT), debounce_palette, RISING);

  // Interrupt to turn on and off palette autopilot mode
  //pinMode(PAL_AUTO_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_AUTO_INT), debounce_palette_auto, RISING);

  // Interrupt to switch animation
  pinMode(ANIM_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(ANIM_INT), debounce_anim, RISING);
}

void loop() {
  // Read color from potentiometer input
  gRGB = CHSV(map(analogRead(C_POT), 0, 1253, 0, 255), 255, gBrightness);

  // Check the palette counter and switch acordingly
  if (palette_autopilot) {
    updateGPaletteTimer();
  }
  else {
    updateGPalette();
  }

  // Select animation to run, or go in autopilot
  /*switch( gAnimCounter ) {
    case 0:
      palette_mod();
      break;
    case 1:
      fill_mod_smooth();
      break;
    case 2:
      palette_eq();
      break;
    case 3:
      theater_chase();
      break;
    case 4:
      theater_chase_bounce();
      break;
    case 5:
      theater_chase_mod();
      break;
  }*/

  theater_chase_bounce();

  // For a framerate
  //FastLED.delay(1000./(
}

///////////////////////////////////////
// Some simple modulating animations //
///////////////////////////////////////

// Simple rainbow modulation shit, but its just so sleek with the FastLED ENM.
void palette_mod() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(S_POT), 0, 1253, 1, 200));
    pal_index++;
  }

  fill_palette(leds, numLED, pal_index, 4, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

// Fill the bar in one fast motion, then slowly ramp down based on an input length
void fill_mod_ramp() {

}

// Fill the whole bar and fade to black in a smooth oscillating fashion. Also, slowly
// work our way through the global template
void fill_mod_smooth() {
  static uint8_t brightness = 0;
  static uint8_t prev_brightness = 0;
  static uint8_t pal_index = 0;

  // Modulate brigthness at input dependent rate
  prev_brightness = brightness;
  //brightness = beatsin8(map(analogRead(S_POT), 0, 1253, 120, 10), 0, maxBrightness);
  brightness = beatsin8(80, 0, maxBrightness);

  // If we are about to fade to black, grab the next color in the palette
  if (brightness == 1 && (brightness - prev_brightness) < 0) {
    pal_index += 16;
  }

  // Fill the whole strip
  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();
}

// A single equalizer type bar
void palette_eq() {
  static uint8_t pal_index = 0;
  static uint8_t lead_max = numLED / 2;
  static uint8_t lead = 0;
  static uint8_t prev = 0;

  // Fill bar at input dependent rate. First record the previous lead value
  // so that we can check which diretion we are heading on the sin wave later on
  prev = lead;
  //lead = beatsin8(map(analogRead(S_POT), 0, 1253, 120, 10), 0, lead_max);
  lead = beatsin8(120, 0, lead_max);

  // If the eq bar is about to be empty, generate a new max length to fill.
  // Also increment the index of the palette, for a little more modulation cuz
  // why not.
  if (lead == 1 && (lead - prev) < 0) {
    pal_index++;
    lead_max = random8(numLED / 3, numLED);
  }

  fill_palette(leds, lead, pal_index, 16, gPalette, maxBrightness, gBlending);
  FastLED.show();
  fadeToBlackBy(leds, numLED, 20);
}

///////////////////////////////////////////////////////////////////////////////////////////
// All of the following animations are some kind of theater marquee style. This means    //
// at the core, its  packets of light chasing eachother across the strip, although we'll //
// change up how they chase in different animations.                                     //
///////////////////////////////////////////////////////////////////////////////////////////

// A theater chase where packets switch directions every once in a while
void theater_chase_bounce() {
  static uint8_t pal_index = 0;
  static uint8_t last_index;
  static uint8_t bpm = map(analogRead(S_POT), 0 , 1253, 60, 20);

  // Update global hue position
  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }
  // Update lead LED position at an input dependent rate
  last_index = pal_index;
  pal_index = beatsin8(bpm);

  if( pal_index == 1 && (last_index - pal_index) < 0) {
    bpm = map(analogRead(S_POT), 0 , 1253, 60, 20);
  }
  
  fill_palette(leds, numLED, pal_index, 4, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

// Like the bounce but packets move continuously through, not switching direction
void theater_chase() {
  // Indices
  static uint8_t pal_index = 0;

  // Update global hue position
  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }

  // Update lead LED position at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(S_POT), 0, 1253, 1, 200));
    pal_index++;
  }
  fill_palette(leds, numLED, pal_index, 6, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

void theater_chase_mod() {
  // Index shit
  static uint8_t pal_index = 0;
  static uint8_t col_inc = 0;

  // Update global hue position
  EVERY_N_MILLISECONDS(100) {
    gHue++;
  }

  // Move the palette index every 10 ms
  EVERY_N_MILLISECONDS(10) {
    pal_index++;
  }
  // Update the fill_palette spacing at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 5) {
    thisTimer.setPeriod(map(analogRead(S_POT), 0, 1253, 1, 4000));
    col_inc++;
  }
  if (col_inc < 256 / 2) {
    fill_palette( leds, numLED, pal_index, col_inc, gPalette, maxBrightness, gBlending);
  }
  else {
    fill_palette( leds, numLED, pal_index, 256 - col_inc, gPalette, maxBrightness, gBlending);
  }
  FastLED.show();
}
