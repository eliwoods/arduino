#include<FastLED.h>

#define MAX_NUM_LED 180
#define NUM_LED 90
#define NUM_STRIPS 8


CRGB leds[NUM_STRIPS * MAX_NUM_LED];


// READ ME!!! //
// These are the pins on the arduion we're using. Attach a 5ft strip to each of the pins, then just tell me
// what colors go to which pin number. Ezpz lemon squeezy.
// WS2811_PORTD: 25,26,27,28,14,15,29,11
//

const CRGBPalette16 test(CRGB::Red, CRGB::Blue, CRGB::Orange, CRGB::Purple, CRGB::Yellow,
                            CRGB::Green, CRGB::White, CRGB::Pink, CRGB::Black, CRGB::Black,
                            CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black
                           );

void setup() {
  // LEDS.addLeds<WS2811_PORTA,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTB,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  LEDS.addLeds<WS2811_PORTD, NUM_STRIPS>(leds, MAX_NUM_LED);
  LEDS.setBrightness(32);
}

void loop() {

  for (uint8_t ss = 0; ss < NUM_STRIPS; ss++) {
    if(ss == 0) {
      fill_solid(leds, MAX_NUM_LED, test[ss]);
    }
    fill_solid(leds + ss * MAX_NUM_LED, NUM_LED, test[ss]);
  }


  LEDS.show();
  LEDS.delay(10);
}
