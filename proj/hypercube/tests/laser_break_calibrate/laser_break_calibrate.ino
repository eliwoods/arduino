#include <FastLED.h>

// Digital Pins (I/O)
#define LASER_POWER 3

void setup() {
  Serial.begin(9000);
  
  pinMode(LASER_POWER, OUTPUT);
  
}

void loop() {
  digitalWrite(LASER_POWER, HIGH);

  Serial.print("Ch 0:\t");
  Serial.println(analogRead(0));
  Serial.print("CH 1:\t");
  Serial.println(analogRead(1));
  Serial.print("Ch 2:\t");
  Serial.println(analogRead(2));
  Serial.print("Ch 3:\t");
  Serial.println(analogRead(3));
  
}
