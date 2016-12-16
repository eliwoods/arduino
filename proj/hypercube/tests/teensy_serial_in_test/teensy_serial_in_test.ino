#include <FastLED.h>

#define NUM_LED 30
#define LED_PIN 5

CRGB leds[NUM_LED];

uint8_t gHue;
boolean got_event = false;

void setup() {
  // Setup Serial
  Serial1.begin(9600);
  Serial.begin(9600);

  // Setup FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setBrightness(100);
  FastLED.show();

}

void loop() {

  if ( Serial1.available() > 0) {
    gHue = Serial1.read();
    fill_solid(leds, NUM_LED, CHSV(gHue, 255, 100));
    FastLED.show();
    FastLED.delay(10); // idk why I'm doing this..
  }


  //    if (got_event) {
  //      fill_solid(leds, NUM_LED, CHSV(gHue, 255, 100));
  //      FastLED.show();
  //      FastLED.delay(10); // idk why I'm doing this..
  //      got_event = false;
  //    }

  //  fill_solid(leds, NUM_LED, CHSV(gHue, 255, 100));
  //  FastLED.show();
  //  FastLED.delay(10); // idk why I'm doing this..
  //  gHue++;

}

void serialEvent() {
  while (Serial1.available()) {
    gHue = Serial1.read();
    //    if ((char)Serial.read() == '\n') {
    //      got_event = true;
    //    }
    got_event = true;
  }
}

