#include <FastLED.h>

#define NUM_LED 30
#define LED_PIN 1

CRGB leds[NUM_LED];

void setup() {
  // Setup Serial
  Serial.begin(11500);

  // Setup FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setBrightness(100);
  FastLED.show();

}

void loop() {
  static uint8_t gHue = 0;

  // Read in serial data
  if (Serial.available() > 0) {
    gHue = Serial.read();
  }

  fill_solid(leds, NUM_LED, CHSV(gHue, 255, 100));
  FastLED.show();
  FastLED.delay(10); // idk why I'm doing this..


}
