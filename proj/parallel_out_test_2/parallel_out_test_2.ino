#include<FastLED.h>


#define NUM_LED 90
#define NUM_STRIPS 8


CRGB leds[NUM_STRIPS * NUM_LED];


// READ ME!!! //
// These are the pins on the arduion we're using. Attach a 5ft strip to each of the pins, then just tell me
// what colors go to which pin number. Ezpz lemon squeezy.
// WS2811_PORTD: 25,26,27,28,14,15,29,11
//

const TProgmemPalette16 WhiteBlack_p PROGMEM = {
  CRGB::Red,
  CRGB::Blue,
  CRGB::Orange,
  CRGB::Purple,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::White,
  CRGB::Pink,
  CRGB::White,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
};

void setup() {
  // LEDS.addLeds<WS2811_PORTA,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTB,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  LEDS.addLeds<WS2811_PORTD, NUM_STRIPS>(leds, NUM_LED);
  LEDS.setBrightness(32);

  LEDS.setMaxPowerInMilliWatts(24000);
}

void loop() {


//  for (uint8_t ss = 0; ss < NUM_STRIPS; ss++) {
//    fill_solid(&(leds[ss*NUM_LED]), NUM_LED, CHSV(2*ss*16, 192, 255)); 
//  }

  static uint8_t hue = 0;
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LED; j++) {
      leds[(i * NUM_LED) + j] = CHSV((32 * i) + hue + j, 192, 255);
    }
  }

  // Set the first n leds on each strip to show which strip it is
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j <= i; j++) {
      leds[(i * NUM_LED) + j] = CRGB::Red;
    }
  }

  hue++;

  //LEDS.setBrightness(32);
  LEDS.show();
  LEDS.delay(10);
}
