// LED Includes
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

// Arduino Includes
#ifdef __AVR__
#include <avr/interrupt.h>
#endif

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
const uint8_t maxBrightness = 50;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER

// Setup and global variable delcaration for palettes
CRGBPalette16 gPalette;
TBlendType gBlending;
uint8_t gIndex; // Global Palette Index
extern const uint8_t numPalettes;

// To control hue globally through a potentiometer input
uint8_t gHue;

// For animation switching, this number needs to be hard coded unforunately
const uint8_t numAnimation = 12; //CHANGE THIS, MIGHT NOT EVEN BE NECESSARY
uint8_t chaser_opt = 0; // For choosing the sub patterns on the animations that use chasers


void setup() {
  // Initialize the leds, specifially to use OctoWS2811 controller
  LEDS.addLeds<OCTOWS2811>(leds, strip_len * num_per_group); // No need to declare pin numbers since they are preset with parallel output
  LEDS.setBrightness(maxBrightness); // LETS SEE HOW FAR WE CAN PUSH IT
  LEDS.show();

  // Initialize global color variables
  gBrightness = maxBrightness;
  gBlending = NOBLEND;
  gPalette = RainbowColors_p; // SHOULD CHANGE THIS
  gIndex = 0;
  gHue = 0;

}

void loop() {
  static uint8_t gAnimCounter = 0;
  EVERY_N_SECONDS(30) {
    gAnimCounter = (gAnimCounter + 1) % 2;
  }

  // Since it costs basically nothing, lets change the global index on each looop
  EVERY_N_MILLISECONDS(1) {
    gIndex++;
  }

  switch (gAnimCounter) {
    // Now lets see if we can send chasers all in one direction along each shell
    case 0:
      // Fill the template array first
      chaser(led_tmplt, strip_len, gIndex);

      // Now fill both the inner template
      for (uint8_t s = 0; s < in_strips; s++) {
        if (s % 2 == 0) {
          in_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt;
        }
        else {
          in_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt;
        }
      }

      // Fill outer template
      for (uint8_t s = 0; s < out_strips; s++) {
        if (s % 2 == 0) {
          out_leds(strip_len * (s + 1) - 1, strip_len * s) = led_tmplt;
        }
        else {
          out_leds(strip_len * s, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }

      // Now send to the LEDs
      copy_pasta_dump();
      break;
  
    case 1:
      static uint8_t in_offset = 12;
      static uint8_t out_offset = 4;
      // First do the inner cylider since it's smaller
      for (uint8_t s = 0; s < in_strips; s++) {
        chaser(led_tmplt, strip_len, gIndex + in_offset * s);
        if (s % 2 == 0) {
          in_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt;
        }
        else {
          in_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt;
        }
      }

      // Next do the outer cylinder
      for (uint8_t s = 0; s < out_strips; s++) {
        chaser(led_tmplt, strip_len, gIndex + out_offset * s);
        if (s % 2 == 0) {
          out_leds(strip_len * (s + 1) - 1, s * strip_len) = led_tmplt;
        }
        else {
          out_leds(s * strip_len, strip_len * (s + 1) - 1) = led_tmplt;
        }
      }

      // Now we copy-pasta and dump the info
      copy_pasta_dump();
      break;
  }

  // We'll have to use a separate counter for these than the global index
//      static uint8_t in_pos = 0;
//      static uint8_t out_pos = 0;
//
//      // Update the positions of both the strips and reset the array
//      EVERY_N_MILLISECONDS(200) {
//        in_pos = (in_pos + 1) % in_strips;
//        out_pos = (out_pos + 1) % out_strips;
//        clear_all();
//      }
//
//      // Filling the inner perimeter and outer. We offset the outer to deal
//      // with them being in the same array
//      fill_solid(leds(strip_len * in_pos, strip_len * (in_pos + 1) - 1), strip_len, CHSV(10, 255, maxBrightness));
//      fill_solid(leds(in_LED_tot + strip_len * out_pos, in_LED_tot + strip_len * (out_pos + 1) - 1), strip_len, CHSV(50, 255, maxBrightness));
//      LEDS.show();
//      break;


  // Try spiraling the chasers in opposite direction on the inner and the outer
  // cylinders. Do this by offsetting the chasers by some amount to be tested later.
  //  if ( gAnimCounter == 3) {
  //    static uint8_t chaser_offset = 4;
  //    // First do the inner cylider since it's smaller
  //    for (uint8_t s = 0; s < in_strips; s++) {
  //      if (s % 2 == 0) {
  //        chaser(in_leds(s * strip_len, strip_len * (s + 1) - 1), strip_len, gIndex + chaser_offset * s);
  //      }
  //      else {
  //        chaser(in_leds(strip_len * (s + 1) - 1, s * strip_len), strip_len, gIndex + chaser_offset * s);
  //      }
  //    }
  //
  //    // Next do the outer cylinder
  //    for (uint8_t s = 0; s < out_strips; s++) {
  //      if (s % 2 == 0) {
  //        chaser(out_leds(s * strip_len, strip_len * (s + 1) - 1), strip_len, gIndex + chaser_offset * s);
  //      }
  //      else {
  //        chaser(out_leds(strip_len * (s + 1) - 1, s * strip_len), strip_len, gIndex + chaser_offset * s);
  //      }
  //    }
  //
  //    // Now we copy-pasta and dump the info
  //    copy_pasta_dump();
  //  }
  //
  //  // Now lets try filling the inner and outer cylinders in opposite direction.
  //  // Lets also offset them for shits and giggles AKA because it'll look badass
  //  if (gAnimCounter == 4) {
  //    static uint8_t chase_offset = 4;
  //    // First do the inner cylinder
  //    for (uint8_t s = 0; s < in_strips; s++) {
  //      if (s % 2 == 0) {
  //        fill_to_empty(in_leds(s * strip_len, strip_len * (s + 1) - 1), strip_len, gIndex + s * chase_offset);
  //      }
  //      else {
  //        fill_to_empty(in_leds(strip_len * (s + 1) - 1, strip_len * s), strip_len, gIndex + s * chase_offset);
  //      }
  //    }
  //
  //    // Now do the outer cylinder
  //    for (uint8_t s = 0; s < out_strips; s++) {
  //      if (s % 2 == 0) {
  //        fill_to_empty(out_leds(s * strip_len, strip_len * (s + 1) - 1), strip_len, gIndex + s * chase_offset);
  //      }
  //      else {
  //        fill_to_empty(out_leds(strip_len * (s + 1) - 1, strip_len * s), strip_len, gIndex + s * chase_offset);
  //      }
  //    }
  //
  //    // Now copy-pasta to the main array and dump to the lights
  //    copy_pasta_dump();
  //  }
  //
  //  // Illuminate one strip at a time and send it around the perimeter. Do the same for both
  //  // inner and outer, but obviously in different directions
  //  if (gAnimCounter == 5) {
  //    // We'll have to use a separate counter for these than the global index
  //    static uint8_t in_pos = 0;
  //    static uint8_t out_pos = 0;
  //
  //    // Update the positions of both the strips and reset the array
  //    EVERY_N_MILLISECONDS(200) {
  //      in_pos = (in_pos + 1) % in_strips;
  //      out_pos = (out_pos + 1) % out_strips;
  //      clear_all();
  //    }
  //
  //    // Filling the inner perimeter and outer. We offset the outer to deal
  //    // with them being in the same array
  //    fill_solid(leds(strip_len * in_pos, strip_len * (in_pos + 1) - 1), strip_len, CRGB::Red);
  //    fill_solid(leds(in_LED_tot + strip_len * out_pos, in_LED_tot + strip_len * (out_pos + 1) - 1), strip_len, CRGB::Red);
  //    LEDS.show();
  //
  //  }

  // Draw a circle on the inner and outer perimeters and move it up and down in...
  // you guessed it, opposite directions!


}
