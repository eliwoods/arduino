#include<FastLED.h>

#define MAX_NUM_LED 180
#define NUM_LED 90
// Note: this can be 12 if you're using a teensy 3 and don't mind soldering the pads on the back
#define NUM_STRIPS 8

CRGB leds[NUM_STRIPS * MAX_NUM_LED];

// Pin layouts on the teensy 3/3.1:
// WS2811_PORTD: 2,14,7,8,6,20,21,5
// WS2811_PORTC: 15,22,23,9,10,13,11,12,28,27,29,30 (these last 4 are pads on the bottom of the teensy)
// WS2811_PORTDC: 2,14,7,8,6,20,21,5,15,22,23,9,10,13,11,12 - 16 way parallel
//
// Pin layouts on the due
// WS2811_PORTA: 69,68,61,60,59,100,58,31 (note: pin 100 only available on the digix)
// WS2811_PORTB: 90,91,92,93,94,95,96,97 (note: only available on the digix)
// WS2811_PORTD: 25,26,27,28,14,15,29,11
//

const CRGBPalette16 test = {CRGB::Red, CRGB::Blue, CRGB::Orange, CRGB::Purple, CRGB::Yellow,
                            CRGB::Green, CRGB::White, CRGB::Pink, CRGB::Black, CRGB::Black, 
                            CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black };

void setup() {
  // LEDS.addLeds<WS2811_PORTA,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTB,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  LEDS.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, MAX_NUM_LED);
  LEDS.setBrightness(32);
}

void loop() {

  for(uint8_t ss = 0; ss < NUM_STRIPS; ss++) {
    if(ss == 0) {
      fill_solid(leds+ss*MAX_NUM_LED, MAX_NUM_LED, test[ss]);
    }
    else {
      fill_solid(leds+ss*MAX_NUM_LED, NUM_LED, test[ss];
    }
  }
  

  LEDS.show();
  LEDS.delay(10);
}
