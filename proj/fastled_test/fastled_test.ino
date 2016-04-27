#include <FastLED.h>

// Pin Variables
#define LED_IN 6

// Variables for the LED strand
const uint8_t frameRate = 30; // FPS
const uint8_t maxBrightness = 100;
uint8_t gBrightness = maxBrightness; // CHANGE THIS ONCE YOU HAVE ANOTHER POTENTIOMETER
const uint8_t numLED = 15;
CRGB *leds = new CRGB[numLED];

CRGBPalette16 gPalette;
uint8_t gHue;

void setup() {
  delay(3000);

  FastLED.addLeds<NEOPIXEL, LED_IN>(leds, numLED);
  FastLED.addLeds<NEOPIXEL, LED_IN>(leds+numLED, numLED);
  FastLED.setBrightness(maxBrightness);

  gPalette = RainbowColors_p;
  gHue = 0;
}

void loop() {

  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }

  fill_palette(leds, numLED, gHue, 4, gPalette, maxBrightness, LINEARBLEND);
  fill_palette(leds+15, numLED, gHue, 4, gPalette, maxBrightness, LINEARBLEND);

  FastLED.show();
  
}

