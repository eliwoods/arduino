#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    16
#define BRIGHTNESS  255
#define RATIO 0.5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 16       

const CRGBPalette16 palette = {
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black
};

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
}


void loop() {
    static uint8_t index = 0;   
    EVERY_N_MILLISECONDS(1000 / UPDATES_PER_SECOND) {
      index++; 
    }
     
    fill_palette(leds, 16, index, 16, palette, BRIGHTNESS, LINEARBLEND);
    FastLED.show();
}

