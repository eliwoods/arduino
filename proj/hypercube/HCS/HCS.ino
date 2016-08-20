// LED Includes
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

// Arduino Includes
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

// BNO055 Includes
#include <i2c_t3.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055_t3.h>
#include <utility/imumaths.h>

// Digital Pins for Interrupts (may be subject to change)
#define LASER0 0
#define LASER1 1
#define LASER2 17
#define LASER3 22
#define PIEZO0 18
#define PIEZO1 19
#define DONOTPRS 23
// NEED TO DECIDE ON A PIN TO USE FOR LASER BREAK RESET SWITCH

#define UNUSED 13

// For describing the shells easier in code
#define INNER 0
#define OUTER 1

// Object for our accelerometer and other variables
Adafruit_BNO055 bno = Adafruit_BNO055(WIRE1_BUS, -1, BNO055_ADDRESS_A, I2C_MASTER, I2C_PINS_29_30, I2C_PULLUP_INT, I2C_RATE_100, I2C_OP_MODE_ISR);
boolean bno_running = false;
double acc_thresh = 10; // TOTALLY ARBITRARY VALUE RN
uint32_t curr_second, last_second;
uint8_t cumm_time;
const uint8_t bno_off_time = 5; // Turn off after 10 seconds of being idle


// Constants describing numbers of LEDs and strips
const uint16_t strip_len = 90; // Number of LED per strip
const uint8_t num_per_group = 6; // Number of strips per OctoWS2811 group

const uint8_t in_strips = 3 * num_per_group; // Number of strips on inner shell
const uint8_t out_strips = 5 * num_per_group; // Number of strips on outer shell

const uint16_t in_LED_tot = in_strips * strip_len;
const uint16_t out_LED_tot = out_strips * strip_len;
const uint16_t led_tot = in_LED_tot + out_LED_tot;

// Some templates that we can copy paste to the main LED array
CRGBArray<strip_len> led_tmplt;
CRGBArray<in_LED_tot> in_leds;
CRGBArray<out_LED_tot> out_leds;

// The array that ultimately gets pushed to the Octo Controller
CRGBArray<led_tot> leds;

// For controlling the brightness, again might not need this variability.
const uint8_t maxBrightness = 75;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette, nPalette; // These point towards the background palette that we fill the templates with
CRGBPalette16 oPalette, iPalette; // Lets try using two separate palettes for the shells
CRGBPalette16 tempPalette; // Use this to store the current palette while we overwrite it and switch
TBlendType gBlending;
uint8_t iPaletteCounter, oPaletteCounter, gPaletteCounter, tmp_iPaletteCounter; // Global Palette
uint16_t gRate; // Global Palette Index
extern const uint8_t numPalettes, numPalStyles;
boolean use_white = false;

// To control hue globally through accelerometer input
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t iNumAnimation = 8; 
const uint8_t oNumAnimation = 8; 
uint8_t iAnimCounter, oAnimCounter;
boolean iAnimSwitch, oAnimSwitch; // Use this flag so that we fade the color palette into each animation
uint8_t iAnimSwitchCount, oAnimSwitchCount; // Use this to count how many times we've faded to the 

// These are the flags that will get flipped during the ISR
volatile boolean do_not_pressed = false;
volatile boolean laser0_on = false;
volatile boolean laser1_on = false;
volatile boolean laser2_on = false;
volatile boolean laser3_on = false;
volatile boolean piezo0_flicked = false;
volatile boolean piezo1_flicked = false;
volatile uint32_t last_millis;
const uint32_t debounce_time = 15; // In seconds

void setup() {
  // Initialize the leds, specifially to use OctoWS2811 controller
  LEDS.addLeds<OCTOWS2811>(leds, strip_len * num_per_group); // No need to declare pin numbers since they are preset with parallel output
  LEDS.setBrightness(maxBrightness); // LETS SEE HOW FAR WE CAN PUSH IT
  LEDS.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = LINEARBLEND;
  iPaletteCounter = 0;
  oPaletteCounter = iPaletteCounter+3;

  iAnimCounter = 0;
  oAnimCounter = 0;
  iAnimSwitch = false;
  oAnimSwitch = false;
  iAnimSwitchCount = 0;
  oAnimSwitchCount = 0;
  gRate = 0;
  gHue = 0;

  // Random number generation for the noise overlap
  random16_set_seed(4832);
  random16_add_entropy(analogRead(UNUSED));

  // Setup for the interrupts, names hould be pretty self explanatory
  pinMode(LASER0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LASER0), laser0_ISR, RISING);

  pinMode(LASER1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LASER1), laser1_ISR, RISING);

  pinMode(LASER2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LASER2), laser2_ISR, RISING);

  pinMode(LASER3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LASER3), laser3_ISR, RISING);

  pinMode(PIEZO0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIEZO0), piezo0_ISR, CHANGE);

  pinMode(PIEZO1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIEZO1), piezo1_ISR, CHANGE);

  pinMode(DONOTPRS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DONOTPRS), debounce_do_not_press, RISING);

  // Initialize the gyroscope. If it's not on, turn the whole display red
  // as an easy indication
  if (!bno.begin()) {
    fill_solid(leds, led_tot, CHSV(0, 255, gBrightness));
    LEDS.show();
    while(1);
  }

  bno.setExtCrystalUse(true);

}

void loop() {
  // Read in events from the gyroscope/accelerometer
  static imu::Vector<3> accel;
  static sensors_event_t event;
  EVERY_N_MILLISECONDS(20) {
    bno.getEvent(&event);
    accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  }

  // Check if the BNO has stopped moving below some threshold
  if(accel.x() < acc_thresh && accel.y() < acc_thresh) {
    // Only do these calculations if the accelerometer is currently being read from
    if(bno_running) {
      curr_second = millis()*1000;
      // Add the difference to the cummulative time if we're not on the same second
      if(curr_second > last_second) {
        cumm_time += curr_second - last_second;
      }
      // Turn BNO off if it's sad idle for too long
      if(cumm_time >= bno_off_time) {
        bno_running = false;
      }
    }
  }
  // If the BNO has moved, use it's orientation to control animation rate and colors
  else if(accel.x() > acc_thresh || accel.y() > acc_thresh) {
    bno_running = true;
    cumm_time = 0;

    // Convert the heading to the color index
    gHue = map(event.orientation.x, 0, 359, 0, 255);

    // Convert the pitch to that rate at which the animation index is changed
    gRate = map(event.orientation.y, -180, 180, 5, 150);
  }
  // If the the bno is sitting still, run the animations and hue change at a constant rate
  if(!bno_running) {
    gRate = 10;
    EVERY_N_MILLISECONDS(20) {
      gHue++;
    }
  }

  // Handle switching between outer shell animations. Don't do this if we're
  // already switching the inner shell though, don't want to figure that out
  if(!iAnimSwitch) {
    EVERY_N_SECONDS(60) {
      oAnimCounter = (oAnimCounter + 1) % oNumAnimation;
      oAnimSwitch = true;
      // Store the current palette and overwite it with all black
      tempPalette = oPalette;
      oPalette = CRGBPalette16(CRGB::Black);
    }
  }

  // Same deal for the inner shell
  if(!oAnimSwitch) {
    EVERY_N_SECONDS(40) {
      iAnimCounter = (iAnimCounter + 1) % iNumAnimation;
      iAnimSwitch = true;
      // Store the current palette and overwrite with all black
      tempPalette = iPalette;
      iPalette = CRGBPalette16(CRGB::Black);
    }
  }

  // Update the global color palette. This is just the color scheme
  // that we will then repackage to have different spacial distributions
  if(!iAnimSwitch && !oAnimSwitch) {
    EVERY_N_SECONDS(100) {
      gPaletteCounter = (gPaletteCounter+1)%numPalettes;
    }
    updatePaletteScheme();
  }

  // Update the palette style for the inner and outer shells. This
  // is basically how we package the given color palette.
  if(!oAnimSwitch) {
    EVERY_N_SECONDS(45) {
      oPaletteCounter = (oPaletteCounter +1) % numPalStyles;
    }
  }
  // For the inner palette, switch it up so that every other time it uses
  // a white palette.
  if(!iAnimSwitch) {
    EVERY_N_SECONDS(135) {
      if(use_white) {
        tmp_iPaletteCounter = iPaletteCounter;
        iPaletteCounter = 5;
        use_white = !use_white;
      }
      //  The white black is no longer part of the usual rotation
      // though since it alternates every time the timer is up.
      else {
        iPaletteCounter = (iPaletteCounter + 1) % (numPalStyles - 1);
      }
    }
  }

  // Do some checks to make sure we don't have some shitty combination of animations and palettes
  if(!oAnimSwitch && !iAnimSwitch) {
    if(oAnimCounter%2 == 0 && (oPaletteCounter == 0 || oPaletteCounter == 4)) {
      // Doign this in two if statements so I don't have a ridiculously long line. This whole check
      // just makes sure that we don't have both of the shells solid colors at the same time
      if( iAnimCounter%2 == 0 && iPaletteCounter == 0) {
        oPaletteCounter = random8(numPalStyles);
        while (oPaletteCounter == 0) {
          oPaletteCounter = random8(numPalStyles);
        }
      }
    }
    // This check makes sure we don't have the rainbow palette going when doing the crazier animations.
    // It just looks way too hectic when that happens
    if(oAnimCounter == 4 || oAnimCounter == 6) {
      oPaletteCounter = random8(numPalStyles);
      while (oPaletteCounter == 5) {
        oPaletteCounter = random8(numPalStyles);
      }
    }
    if(iAnimCounter == 4 || iAnimCounter == 6) {
      iPaletteCounter = random8(numPalStyles-1);
      while(iPaletteCounter == 4) {
        iPaletteCounter = random8(numPalStyles-1);
      }
    }
  }
  updateGPalette();

  // These are the main animations. First we check if we weant to stobe the whole thing though. If we don't
  // do this we end up with white over the animations and not a total strobe takeover.
  if(!piezo0_flicked) { 
    // Check if the antistrobe spring has been flicked
    if(piezo1_flicked) {
      EVERY_N_MILLISECONDS(20) {
        clear_all();
        LEDS.show();
        LEDS.delay(10);
      }
    }

    switch (iAnimCounter) {
      case 0:
        chase_straight(INNER, false);
        break;
      case 1:
        shell_wrap(INNER, 1, false);
        break;
      case 2:
        chase_spiral(INNER, 4, false);
        break;
      case 3:
        shell_wrap(INNER, 0, false);
        break;
      case 4:
        static uint8_t oOffset = 0;
        EVERY_N_MILLISECONDS(200) {
          oOffset++;
        }
        chase_spiral(INNER, oOffset, false);
        break;
      case 5:
        shell_wrap(INNER, 2, false);
        break;
      case 6:
        chase_helix(INNER, 4, false);
        break;
      case 7:
        shell_wrap(INNER, 3, false);
        break;
    }
    switch (oAnimCounter) {
      case 0:
        chase_straight(OUTER, true);
        break;
      case 1:
        shell_wrap(OUTER, 1, true);
        break;
      case 2:
        chase_spiral(OUTER, 24, true);
        break;
      case 3:
        shell_wrap(OUTER, 0, true);
        break;
      case 4:
        static uint8_t oOffset = 0;
        EVERY_N_MILLISECONDS(100) {
          oOffset++;
        }
        chase_spiral(OUTER, oOffset, true);
        break;
      case 5:
        shell_wrap(OUTER, 2, true);
        break;
      case 6:
        chase_helix(OUTER, 24, true);
        break;
      case 7:
        shell_wrap(OUTER, 3, true);
        break;
    }

    // Merge each shell to the whole LED array and push to the lights
    merge_animations();

    // Try the animation fading here. This way the animations will have time to 
    // render once before we start the blending process
    if(oAnimSwitch) {
      nblendPaletteTowardPalette(oPalette, tempPalette, 100);
      if(oPalette == tempPalette) {
        oAnimSwitch = false;
      }
    }
    if(iAnimSwitch) {
      nblendPaletteTowardPalette(iPalette, tempPalette, 100);
      if(iPalette == tempPalette) {
        iAnimSwitch = false;
      }
    }

    // Now turn on the overlay animations if they're meant to be
    if (laser0_on) {
      ring_bounce_opp(10);
    }
    if (laser1_on) {
      helix_spiral_overlay(5, 28);
    }
    if (laser2_on) {
      bar_wrap_overlay(5);
    }
    if (laser3_on) {
      overlay_snow(100, 0.10);
    }
    if (laser0_on || laser1_on || laser2_on || laser3_on) {
      LEDS.show();
    }
  }

  // Strobe the lights if the springs are moving above a threshold determined
  // by the other arduino
  if(piezo0_flicked) {
    EVERY_N_MILLISECONDS(20) {
      fill_solid(leds, led_tot, CHSV(255, 0, maxBrightness));
      LEDS.show();
      clear_all();
    }
  }

  // Check if the big red button has been pressed. Do this up here
  if(do_not_pressed) {
    do_not_press_response();
  }

}
