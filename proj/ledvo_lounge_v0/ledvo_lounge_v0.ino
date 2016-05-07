// LED library
#include <FastLED.h>

// C++ Libraries (yay!)
#include <StandardCplusplus.h>
#include <vector>

// Arduino Libraries
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// Analog Pins
#define RATE_POT 0 // Potentiometer for animation rate
#define HUE_POT 0 // Potentiometer for global hue
#define VAL_POT 0 // Potentiometer for global brightness
#define DJ_POT 0 // Potentiometer for DJ animation rates

// Digital Pins
#define LED_IN 6
#define PAL_INC_INT 2 // Palette incrementing interrupt
#define PAL_DEC_INT 2 // Palette decrementing interrupt
#define PAL_AUTO_INT 3 // Autopilot palette mode interrupt
#define ANIM_INC_INT 3 // Animation incrementing interrupt
#define ANIM_DEC_INT 3 // Animation decrementing interrupt
#define ANIM_AUTO_INT 3 // Animation autopilot interrupt
#define STROBE_INT 3 // Strobe animation interrupt
#define BLKOUT_INT 3 // Blackout animation interrupt
#define WHTOUT_INT 3 // Whiteout animation interrupt
#define BLKSTROBE_INT 3 // Black strobe interrupt
#define REVERS_INT 3 // Reversing animations interrupt


// Variables for the LED strand
const uint8_t frameRate = 100; // FPS
const uint8_t maxBrightness = 200;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER
const uint16_t numLED = 60;
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
const uint8_t numAnimation = 10;

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

  // Interrupts to switch palette choice
  pinMode(PAL_INC_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PAL_INC_INT), debounce_palette_inc, RISING);

  //pinMode(PAL_DEC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_DEC_INT), debounce_palette_dec, RISING);

  // Interrupt to turn on and off palette autopilot mode. Use change since the button
  // will be latching
  //pinMode(PAL_AUTO_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PAL_AUTO_INT), debounce_palette_auto, CHANGE);

  // Interrupts to switch animation
  pinMode(ANIM_INC_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(ANIM_INC_INT), debounce_anim_inc, RISING);

  //pinMode(ANIM_DEC_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ANIM_DEC_INT), debounce_anim_dec, RISING);

  // Interrupt to turn on and off animation autopilot mode. Use change since button
  // will be latching.
  //pinMode(ANIM_AUTO_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ANIM_AUTO_INT), debounce_anim_auto, CHANGE);

  // Interrupt for strobe DJ animation. Use change so animation starts on button press
  // and ends on button release
  //pinMode(STROBE_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(STROBE_INT), debounce_strobe, CHANGE);

  // Interrupt for blackout DJ animation. Look for change so we can hold and release
  //pinMode(BLKOUT_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BLKOUT_INT), debounce_blackout, CHANGE);

  // Interrupt for whiteout DJ animation. Look for change so we can hold and release
  //pinMode(WHTOUT_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(WHTOUT_INT), debounce_whiteout, CHANGE);

  // Interrupt for strobing black over current animation.
  //pinMode(BLKSTROBE_INT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BLKSTROBE_INT), debounce_blk_strobe, RISING);

  // Interrupt for reversing animation direction. Look for change since this will
  // be a latching button
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
    EVERY_N_SECONDS(30) {
      gPaletteCounter = (gPaletteCounter + 1) % numPalettes;
    }
    updateGPalette();
  }
  else {
    //updateGPalette();
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
  /*if (!dj_control) {
    switch( gAnimCounter ) {
      case 0:
        theater_chase();
        break;
      case 1:
        theater_chase_bounce();
        break;
      case 2:
        theater_chase_saw();
        break;
      case 3:
        theater_chase_mod();
        break;
      case 4:
        fill_smooth();
        break;
      case 5:
        palette_eq();
        break;
      case 6:
        fill_ramp_up();
        break;
      case 7:
        fill_ramp_down();
        break;
      case 8:
        palette_eq_tri();
        break;
      case 9:
        fill_to_empty();
        break;
    }
    }*/

  //fill_to_empty();

  theater_chase();

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
  //  EVERY_N_MILLISECONDS_I(thisTimer, 200) {
  //    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 100, 300));
  //    fill_solid(leds, numLED, CRGB::White);
  //  }

  EVERY_N_MILLISECONDS(200) {
    fill_solid(leds, numLED, CRGB::White);
  }

  FastLED.show();
  fadeToBlackBy(leds, numLED, 10);
  //fill_solid(leds, numLED, CRGB::Black);
}

// Strobes black over animations, or at least a crude attempt to do so
void strobe_black() {
  EVERY_N_MILLISECONDS_I(thisTimer, 175) {
    fill_solid(leds, numLED, CRGB::Black);
    FastLED.show();
    FastLED.delay(50);
  }
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

// Fill the whole bar and fade to black in a smooth oscillating fashion. Also, slowly
// work our way through the global template
void fill_smooth() {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;

  // Some variables to help us smoothly transition between rates and
  // keep track of where we are on the sinwave
  static uint8_t bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
  static uint8_t prev_brightness = 0;
  static uint8_t last_second = 99;
  uint8_t second = (millis() / 1000) % 60;

  // Modulate brigthness at input dependent rate
  prev_brightness = brightness;
  brightness = beatsin8(bpm, 0, maxBrightness);

  // If we are about to fade to black, grab the next color in the palette
  if (brightness == 1 && (brightness - prev_brightness) < 0) {
    pal_index += 16;
  }

  // Check for when we should switch bpm here, so we don't interrupt color assignment
  if (brightness == 1 && (brightness - prev_brightness) < 0 && last_second != second) {
    last_second = second;
    bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
  }

  // Fill the whole strip
  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();
}

// A single equalizer type bar
void palette_eq() {
  static uint8_t pal_index = 0;
  static uint16_t lead_max = numLED / 2;
  static uint16_t lead = 0;

  // Some variables to help us smoothly transition between rates and
  // keep track of where we are on the sinwave
  static uint8_t bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
  static uint8_t prev = 0;
  static uint8_t last_second = 99;
  uint8_t second = (millis() / 1000) % 60;

  // Fill bar at input dependent rate. First record the previous lead value
  // so that we can check which diretion we are heading on the sin wave later on
  prev = lead;
  lead = beatsin8(bpm, 0, lead_max);

  // If the eq bar is about to be empty, generate a new max length to fill.
  // Also increment the index of the palette, for a little more modulation cuz
  // why not.
  if (lead == 0 && (lead - prev) < 0) {
    pal_index += 16;
    lead_max = random8(numLED / 3, numLED);
  }

  // Check for rate change here so we don't interrupt the color and length
  // assignment
  if (lead == 0 && (lead - prev) < 0 && last_second != second) {
    last_second = second;
    bpm = map(analogRead(RATE_POT), 0, 1253, 120, 10);
    lead = 0;
  }

  //fill_palette(leds, lead, pal_index, 16, gPalette, maxBrightness, gBlending);
  fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
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
  uint8_t second = (millis() / 1000) % 60;

  // Update lead LED position at an input dependent rate
  last_index = pal_index;
  pal_index = beatsin8(bpm, 0, 155);

  if ((pal_index == 127 && (last_index - pal_index) < 0) && second != last_second) {
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

// THESE FUNCTIONS ARE UNDER SERIOUS CONSTRUCTION!!!!!!!!! //

// Ramp the brightness up, then cut off quickly like a saw wave
void fill_ramp_up() {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;

  // Ramp the brightness up at a input controlled rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    brightness += 16;
  }

  // Jump to next color in the gradient if we are at the bottom of the ramp
  if (brightness == 0) {
    pal_index += 16;
  }

  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();

}

// Jump to a fully illuminated strand, then ramp down. Also like a saw wave, but
// the opposite direction.
void fill_ramp_down() {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;

  // Ramp the brightness up at a input controlled rate
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    brightness -= 16;
  }

  // Jump to next color in the gradient if we are at the bottom of the ramp
  if (brightness == 0) {
    pal_index -= 16;
  }

  fill_solid(leds, numLED, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
  FastLED.show();
}


// Like palette_eq, but uses a triangular wave. This lets us have more control over the speed
// and transition smoother between animation rates.
void palette_eq_tri() {
  static uint8_t pal_index = 0;
  static uint16_t lead_max = numLED / 2;
  static uint16_t lead = 0;

  // Fill bar at input dependent rate. First record the previous lead value
  // so that we can check which diretion we are heading on the sin wave later on
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    lead = (lead + 1) % (2 * lead_max);
  }

  // If the eq bar is about to be empty, generate a new max length to fill.
  // Also increment the index of the palette, for a little more modulation cuz
  // why not.
  if (lead == 0) {
    pal_index += 16;
    lead_max = random8(numLED / 3, numLED);
  }

  // Fill the the bar until we get to lead_max, then work our way back down. This doesn't use
  // triwave8 explicitly since it has a set range and we need to change the amplitude of the triangluar
  // each time we make a "round trip"
  if (lead < lead_max) {
    fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else {
    fill_solid(leds, 2 * lead_max - lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  FastLED.show();
  // Need this so that the decreasing direction animates properly
  fadeToBlackBy(leds, numLED, 20);
}

// Fill the whole strip from left to right, then empty from left to right.
void fill_to_empty() {
  // Index for grabbing colors from the global palette
  static uint8_t pal_index = 0;

  // Some variables for the filling animation
  static boolean _fill = true; // Flag to know which way we are filling
  static uint16_t lead = 0;

  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    lead = (lead + 1) % numLED;
  }



  if (_fill) {
    //fill_solid(leds, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    fill_solid(leds, lead, CRGB::Red);
  }
  else {
    fill_solid(leds, lead, CRGB::Black);
  }
  FastLED.show();

  // Switch fill flag if we are done filling. If we are done emptying, switch
  // the flag and also grab the next color in the palette. Try this in here
  // to see if it fixes the issue of missing the switch at 0
  if (lead == numLED - 1) {
    if (_fill) {
      _fill = false;
    }
    else {
      _fill = true;
      pal_index += 16;
    }
  }
}


// Pretty self explanatory. Grab a random LED and turn it on if it's off, or turn it off if it's on.
// Rinse and repeat.
void starry_night_flicker() {
  // Grab a random LED
  static uint16_t led = random16(numLED);

  // Now ramp up and down the pixel at an input dependent rate
  static uint8_t brightness_counter = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 100));
    brightness_counter += 4; // Make this ramp up a little bit faster
  }

  // Ramp all the channels up at the same rate achieve a single
  // pixel white glowing effect
  uint8_t brightness = triwave8(brightness_counter);
  leds[led] = CRGB(brightness, brightness, brightness);
  FastLED.show();

  // If we are back to zero brightness, grab a new led to do this with
  if (brightness_counter == 0) {
    led = random16(numLED);
  }

}

void starry_night_vector() {
  // Vector to store the randomly chosen leds
  static std::vector<uint16_t> stars;

  // If the vector is empty, we are either running for the first time
  // or we are on the next cycle. Either way we want to set it up
  if (stars.size() == 0) {
    uint8_t rnd;
    for (uint16_t ll = 0; ll < numLED; ll++) {
      rnd = random8();
      // Only set ~1/3 of the leds as a star
      if (rnd < (255 / 3)) {
        stars.push_back(ll);
      }
    }
  }

  // Increase brightness at an input controlled rate
  static uint8_t brightness_counter = 0;
  EVERY_N_MILLISECONDS_I(thisTimer, 100) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 150));
    brightness_counter += 4;
  }

  // Now loop over stars and ramp their brightness up and down
  uint8_t brightness = triwave8(brightness_counter);
  for (uint16_t ll = 0; ll < stars.size(); ll++ ) {
    leds[stars[ll]] = CRGB(brightness, brightness, brightness);
  }

  FastLED.show();

  // Check if we are at the valley of our triwave, and clear the
  // vector for the next set of stars if we are.
  if (brightness_counter == 0) {
    stars.clear();
  }

}


// Random blocks of colors with a random width.
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
    uint8_t pal_index = 0;
    while (num_filled < numLED) {
      // Generate new packet length
      rnd_width = random8(2, 5);
      packet_widths.push_back(rnd_width);
      num_filled += rnd_width;

      // Assign packet a color from the current palette
      packet_colors.push_back(ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
      pal_index += 16;
    }

    // Now loop through the vectors and fill the strand
    for (uint8_t vec_index = 0; vec_index < packet_widths.size(); vec_index++) {

    }

  }

}
