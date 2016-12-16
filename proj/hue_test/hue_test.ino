#include <FastLED.h>

CRGB cols[10];

void setup() {
  Serial.begin(9600);
}

void loop() {
  static uint8_t hue  = 90;
  fill_rainbow(cols, 10, 0);
  
  for(int i = 0; i < 10; i++) {
    Serial.println(rgb2hsv_approximate(cols[i]).hue);
  }
  

}
