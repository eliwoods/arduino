// Sensor Includes
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// FastLED Includes and Variables
#include <FastLED.h>

#define LED_OUT 7
#define NUMLED 90
CRGB leds[NUMLED];

// Some Required Sensor shit
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

/*
   Connections (BNO)
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground
 */

void setup() {
  // Safety First Boys
  delay(3000);

  // FastLED Setup
  FastLED.addLeds<WS2812B, LED_OUT, GRB>(leds, NUMLED);
  FastLED.setBrightness(100);
  FastLED.show();

  // Sensor Setup
  if(!bno.begin()) {
    Serial.print("No sensor detected ya goof!!!");
    while(1);
  }

  bno.setExtCrystalUse(true);

}

void loop() {
  // put your main code here, to run repeatedly:

}
