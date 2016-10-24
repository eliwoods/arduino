#include <Adafruit_NeoPixel.h>

const uint16_t max_led = 10000;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(max_led, 6, NEO_GRB + NEO_KHZ800);

void setup() {
  
  strip.begin();
  for (uint8_t led = 0; led < max_led; led++) {
    strip.setPixelColor(led, 0);
  }
  strip.show();

}

void loop() {
  // put your main code here, to run repeatedly:

}
