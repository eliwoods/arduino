// LED libraries
#include <FastLED.h>
#include <StandardCplusplus.h>
#include <vector>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Analog Pins
#define LED_IN 6
#define RATE_POT 0 // Potentiometer for animation rate
#define HUE_POT 0 // Potentiometer for global hue
#define VAL_POT 0 // Potentiometer for global brightness

// Digital Pins
#define PAL_INC_INT 2 // Interrupt pin for palette incrementing
#define PAL_DEC_INT 2 // Interrupt pin for palette decrementing
#define PAL_AUTO_INT 3 // Interrupt for autopilot palette mode
#define ANIM_INC_INT 3 // Interrupt pin for animation incrementing
#define ANIM_DEC_INT 3 // Interrupt pin for animation decrementing
#define ANIM_AUTO_INT 3 // Interrupt pin for animation autopilot
#define STROBE_INT 3 // Interrupt pin for strobe animation
#define BLKOUT_INT 3 // Interrupt pin for blackout animation
#define WHTOUT_INT 3 // Interrupt pin for whiteout animation
#define REVERS_INT 3 // Interrupt pin for reversing animations


// Variables for the LED strand
const uint8_t frameRate = 100; // FPS
const uint8_t maxBrightness = 200;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER
const uint8_t numLED = 30;
CRGB *leds = new CRGB[numLED];

// Variables for pin interrupts (there's a lot of these babies)
uint32_t debounce_time = 15;
volatile uint32_t last_micros; // In milliseconds
volatile boolean power_state = true; // Used for kill switch
volatile uint8_t gPaletteCounter; // For choosing index of palette
volatile boolean palette_autopilot = false; // Flag for auto cycling palettes
volatile uint8_t gAnimCounter; // Counter for animation
volatile boolean anim_autopilot = false; // Flag for auto cycling animations
volatile boolean anim_reverse = false; // Flag for running animations in reverse
volatile boolean dj_control = false; // Flag if dj is controlling animations
volatile boolean run_strobe = false; // Flag for strobe interrupt animation
volatile boolean run_blackout = false; // Flag for blackout animation
volatile boolean run_whiteout = false; // Flag for whiteout animation


// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette;
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index


extern const uint8_t numPalettes;
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const TProgmemPalette16 WhiteBlack_p PROGMEM;

// To control hue globally through a potentiometer input
CRGB gRGB;
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 6;

void setup() {
  delay(3000); // Safely power up

  // Setup serial for debugging
  Serial.begin(57600);

  // Setup Random Number Generator
  random16_set_seed(4832);
  random16_add_entropy(analogRead(5));

  // Setup strands of LED
  FastLED.addLeds<WS2812, LED_IN, GRB>(leds, numLED).setCorrection(TypicalLEDStrip);;
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

  //// Interrupts to switch palette choice
  //pinMode(PAL_INC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_INC_INT), debounce_palette_inc, RISING);

  //pinMode(PAL_DEC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_DEC_INT), debounce_palette_dec, RISING);

  //// Interrupt to turn on and off palette autopilot mode. Use change since the button
  //// will be latching
  ////pinMode(PAL_AUTO_INT, INPUT);
  ////attachInterrupt(digitalPinToInterrupt(PAL_AUTO_INT), debounce_palette_auto, CHANGE);

  //// Interrupts to switch animation
  ////pinMode(ANIM_INC_INT, INPUT);
  ////attachInterrupt(digitalPinToInterrupt(ANIM_INC_INT), debounce_anim_inc, RISING);

  //pinMode(ANIM_DEC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ANIM_DEC_INT), debounce_anim_dec, RISING);

  //// Interrupt to turn on and off animation autopilot mode. Use change since button
  //// will be latching.
  //pinMode(ANIM_AUTO_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ANIM_AUTO_INT), debounce_anim_auto, CHANGE);

  //// Interrupt for strobe DJ animation. Use change so animation starts on button press
  //// and ends on button release
  //pinMode(STROBE_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(STROBE_INT), debounce_strobe, CHANGE);

  //// Interrupt for blackout DJ animation. Look for change so we can hold and release
  //pinMode(BLKOUT_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BLKOUT_INT), debounce_blackout, CHANGE);

  //// Interrupt for whiteout DJ animation. Look for change so we can hold and release
  //pinMode(WHTOUT_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(WHTOUT_INT), debounce_whiteout, CHANGE);

<<<<<<< HEAD
  // Interrupt for reversing animation direction. Look for change since this will
  // be a latching button
=======
  //// Interrupt for reversing animation direction. Look for change since this will
  //// be a latching button
>>>>>>> refs/remotes/origin/master
  //pinMode(REVERS_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(REVERS_INT), debounce_anim_reverse, CHANGE);
}

void loop() {
  // Read in global brightness value
  //gBrightness = map(analogRead(VAL_POT), 0, 1253, 0, maxBrightness);
  
  // Read color from potentiometer input
  gRGB = CHSV(map(analogRead(HUE_POT), 0, 1253, 0, 255), 255, gBrightness);

  // Increment global hue index for extra groovy rainbows
  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }

  // Check the palette counter and switch acordingly or go into autopilot mode
  if (palette_autopilot) {
    updateGPaletteTimer();
  }
  else {
    updateGPalette();
  }

<<<<<<< HEAD
  // Check if we want to autopilot the animations
  if (anim_autopilot) {
    EVERY_N_SECONDS(30) {
      gAnimCounter = (gAnimCounter)%numAnimation;
    }
  }

  // Select animation to run based on global counter
  /*if (!dj_control) {
    switch( gAnimCounter ) {
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
    }
  }*/

  theater_chase_bounce();
=======
  // Select animation to run, or go in autopilot
  //if (!dj_control) {
  //  switch( gAnimCounter ) {
  //    case 0:
  //      palette_mod();
  //      break;
  //    case 1:
  //      fill_mod_smooth();
  //      break;
  //    case 2:
  //      palette_eq();
  //      break;
  //    case 3:
  //      theater_chase();
  //      break;
  //    case 4:
  //      theater_chase_bounce();
  //      break;
  //    case 5:
  //      theater_chase_mod();
  //      break;
  //  }
  //}

  theater_chase_strobe();
>>>>>>> refs/remotes/origin/master

  // The following are all checks for DJ animations that
  // interrupt the normal animations for some added IN YO FACE
  if (dj_control) {
    if (run_strobe) {
      strobes();
    }
    if (run_blackout) {
      blackout();
    }
    if (run_whiteout) {
      whiteout();
    }
  }

  // For a framerate (probably gonna scrap this)
  //FastLED.delay(1000./ frameRate);
}

////////////////////////////////////////////////////////////
// DJ animations. These are meant to interrupt the normal //
// animations and take over.                              //
////////////////////////////////////////////////////////////

// Can't be named strobe for some reason... IDK man
void strobes() {
  EVERY_N_MILLISECONDS_I(thisTimer, 200) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 100, 300));
    fill_solid(leds, numLED, CRGB::White);
  }

  FastLED.show();
  fadeToBlackBy(leds, numLED, 10);
  //fill_solid(leds, numLED, CRGB::Black);
}

// Does what it says, sets every LED to black
void blackout() {
  fill_solid(leds, numLED, CRGB::Black);
  FastLED.show();
}

// Same as blackout, but with white.
void whiteout() {
  fill_solid(leds, numLED, CRGB::White);
  FastLED.show();
}

///////////////////////////////////////
// Some simple modulating animations //
///////////////////////////////////////

// Simple rainbow modulation shit, but its just so sleek with the FastLED ENM.
void palette_mod() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 200));
    pal_index++;
    gHue++;
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
  //brightness = beatsin8(map(analogRead(RATE_POT), 0, 1253, 120, 10), 0, maxBrightness);
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
  //lead = beatsin8(map(analogRead(RATE_POT), 0, 1253, 120, 10), 0, lead_max);
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

// Simple theater chase where packets move continuously
void theater_chase() {
  // Indices
  static uint8_t pal_index = 0;

  // Update lead LED position at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 200));
    pal_index++;
  }
  fill_palette(leds, numLED, pal_index, 6, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

// A theater chase where packets switch directions every once in a while
void theater_chase_bounce() {
  static uint8_t pal_index = 0;  
  static uint8_t bpm = map(analogRead(RATE_POT), 0 , 1253, 60, 20);

  // Some variables to help us smoothly transition between rates
  static uint8_t last_index = 0;
  static uint8_t last_second = 99;
  uint8_t second = (millis()/1000)%60;

  // Update lead LED position at an input dependent rate
  last_index = pal_index;
  pal_index = beatsin8(bpm, 0, 255);

  if( pal_index == 127 && (last_index - pal_index) < 0 && second != last_second) {
    last_second = second;
    bpm = map(analogRead(RATE_POT), 0 , 1253, 60, 20);
  }
  
  fill_palette(leds, numLED, pal_index, 4, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

// A theater chase where packets switch direction every once in a while. Lets see how
// this looks with a saw wave
void theater_chase_saw() {
  static uint8_t pal_index = 0;

  // Increment palette index at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 500));
    pal_index++;
  }

  fill_palette(leds, numLED, triwave8(pal_index), 4, gPalette, maxBrightness, gBlending);
  FastLED.show();
}

void theater_chase_mod() {
  // Index shit
  static uint8_t pal_index = 0;
  static uint8_t col_inc = 0;

  // Move the palette index every 10 ms
  EVERY_N_MILLISECONDS(10) {
    pal_index++;
  }
  // Update the fill_palette spacing at an input dependent rate
  EVERY_N_MILLISECONDS_I(thisTimer, 5) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 1, 4000));
    col_inc++;
  }
  if (col_inc < 256 / 2) {
    fill_palette(leds, numLED, pal_index, col_inc, gPalette, maxBrightness, gBlending);
  }
  else {
    fill_palette(leds, numLED, pal_index, 256 - col_inc, gPalette, maxBrightness, gBlending);
  }
  FastLED.show();
}

// Strobe on and off the palette as we animate it through it
void theater_chase_strobe() {
  // Indices for palette animation
  static uint8_t pal_index = 0;

  // Move through the palette starting point
  EVERY_N_MILLISECONDS(50) {
    pal_index++;
  }
  fill_palette(leds, numLED, pal_index, 6, gPalette, maxBrightness, gBlending);

  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    fadeToBlackBy(leds, numLED, 30);
  }

  FastLED.show();
}

// Random blocks of colors with a random width
void theater_chase_random() {
  // Vector to hold the widths, since they're randomly
  // determined, we need a vector to deal with varying sizes
  static std::vector<uint8_t> packet_widths;  
  static std::vector<CRGB> packet_colors;
  
  // Fill the vector the first time it's run. Use a static variable
  // so we only have to run it once
  static boolean first_run = false;
  if (first_run) {
    uint8_t rnd_width;
    uint8_t num_filled = 0; // Keep track of how many LEDs we've filled
    uint8_t pal_index=0;
    while (num_filled < numLED) {
      // Generate new packet length
      rnd_width = random8(2, 5);
      packet_widths.push_back(rnd_width);
      num_fill += rnd_width;

      // Assign packet a color from the current palette
      packet_colors.push_back(gPalette[ColorFromPalette(gPalette, pal_index, gBrightness, gBlending)]);
      pal_index += 16;
    }

    // Now loop through the vectors and fill the strand
    for (uint8_t vec_index = 0; vec_index < packet_widths.size(); vec_index++) {
      
    }
    
  }
  
}
