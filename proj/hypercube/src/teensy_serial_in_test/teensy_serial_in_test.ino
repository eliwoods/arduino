#include <FastLED.h>

#define NUM_LED 30
#define LED_PIN 1

CRGB leds[NUM_LED];

uint8_t gHue;
boolean got_event = false;

void setup() {
  // Setup Serial
  Serial2.begin(9600);

  pinMode(0, OUTPUT);

  // Setup FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setBrightness(100);
  FastLED.show();

}

void loop() {

  fill_solid(leds, NUM_LED, CHSV(gHue, 255, 100));
  FastLED.show();
  FastLED.delay(10); // idk why I'm doing this..
    delay(1000);


}

void serialEvent() {
  while (Serial2.available()) {
    gHue = Serial2.read();
    if ((char)Serial2.read() == '\n') {
      got_event = true;
    }
  }
}

