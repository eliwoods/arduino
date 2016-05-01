#include "FastLED.h"

void setup() {
  Serial.begin(9000);

}

void loop() {
  uint8_t blah = beatsin8(20, 0, 255);

  if(blah == 254) {
    Serial.println("BLAH");
  }

  //delay(10);

}
