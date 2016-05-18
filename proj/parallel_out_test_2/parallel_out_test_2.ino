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

CRGBPalette16 gPalette;
uint8_t gHue;

void setup() {
  // LEDS.addLeds<WS2811_PORTA,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTB,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
  // LEDS.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  LEDS.addLeds<WS2811_PORTD, NUM_STRIPS>(leds, NUM_LED);
  LEDS.setBrightness(32);

  //LEDS.setMaxPowerInMilliWatts(24000);
}

void loop() {


  //  for (uint8_t ss = 0; ss < NUM_STRIPS; ss++) {
  //    fill_solid(&(leds[ss*NUM_LED]), NUM_LED, CHSV(2*ss*16, 192, 255));
  //  }

  EVERY_N_MILLISECONDS(10) {
    gHue++;
  }

  update_RainbowBlack_p();
  theater_chase();


  //LEDS.setBrightness(32);
  LEDS.delay(100);
}

void theater_chase() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS(100) {
    pal_index++;
  }

  for (uint8_t ss = 0; ss < NUM_STRIPS; ss ++) {
    fill_palette(leds + (ss * NUM_LED), NUM_LED, pal_index, 6, gPalette, 60, LINEARBLEND);
  }
  LEDS.show();
}

// RB|B|B|B repeating
void update_RainbowBlack_p() {
  // Instead of blacking out, fill backgroud with rainbow, then
  // fill every thing with black except
  fill_rainbow(gPalette, 16, gHue, 10);
  for (uint8_t i = 0 ; i < 16; i++) {
    if ( i % 4 == 0) {
      continue;
    }
    gPalette[i] = CRGB::Black;
  }
}
