#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Arduino Variables
#define PIN 6
#define BUTTON 3

// Important Light Variables
const byte numCrcl = 5;
bool sState = false; // Initialize board to off

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(50, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  // For turning this bright shit off
  pinMode(BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), pin_ISR, CHANGE);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// Just for testing functions ATM
void loop() {

  // Check if board is supposed to be on
  if (sState) {
    for (uint8_t i = 0; i < 10; i++) {
      ripple_smooth(10);
    }
  }

}

// Ripple Smooth Color Transition
void ripple_smooth(uint8_t hold) {
  for (uint16_t j = 0; j < 256; j += 5) {
    for (uint16_t i = 0; i < numCrcl; i++) {
      setRingColor(i,  Wheel((( i * 256 / strip.numPixels()) + j) & 255));
      strip.show();
      delay(hold);
    }
  }
}

void ripple_split(uint8_t hold) {
  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint16_t k = 0; k < 2; k++) {
      for (uint16_t i = 0; i < numCrcl; i = i + 2) {
        setRingColor(i + k, Wheel(( i + j * 10) & 255));
      }
      strip.show();
      delay(hold);

      for (int i = 0; i < 5; i = i + 2) {
        setRingColor(i + k, 0);
      }

    }
  }
}

void ripple_single_rev(uint8_t hold) {
  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint16_t i = 0; i < numCrcl * 2; i++) {
      if (i < 5) {
        setRingColor(i, Wheel(( i + j * 10) & 255));
        strip.show();
        delay(hold);
        setRingColor(i, 0);
      }
      else {
        setRingColor(10 - i, Wheel(( (10 - i) + j * 10) & 255));
        strip.show();
        delay(hold);
        setRingColor(10 - i, 0);
      }
    }
  }
}

void ripple_single(uint8_t hold) {
  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint16_t i = 0; i < numCrcl; i++) {
      setRingColor(i, Wheel(( i + j * 10) & 255));
      strip.show();
      delay(hold);
      setRingColor(i, 0);
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Handle all of the messy work for converting ring number to individual pixels
void setRingColor(uint8_t ring, uint32_t color) {
  if (ring == 0 ) {
    strip.setPixelColor(25, color);
  }
  else if (ring == 1) {
    strip.setPixelColor(9, color);
    strip.setPixelColor(17, color);
    strip.setPixelColor(26, color);
    strip.setPixelColor(34, color);
    strip.setPixelColor(41, color);
    strip.setPixelColor(33, color);
    strip.setPixelColor(24, color);
    strip.setPixelColor(16, color);

  }
  else if (ring == 2) {
    strip.setPixelColor(3, color);
    strip.setPixelColor(10, color);
    strip.setPixelColor(18, color);
    strip.setPixelColor(27, color);
    strip.setPixelColor(35, color);
    strip.setPixelColor(42, color);
    strip.setPixelColor(48, color);
    strip.setPixelColor(47, color);
    strip.setPixelColor(40, color);
    strip.setPixelColor(32, color);
    strip.setPixelColor(23, color);
    strip.setPixelColor(15, color);
    strip.setPixelColor(8, color);
    strip.setPixelColor(2, color);

  }
  else if (ring == 3) {
    strip.setPixelColor(1, color);
    strip.setPixelColor(4, color);
    strip.setPixelColor(11, color);
    strip.setPixelColor(19, color);
    strip.setPixelColor(28, color);
    strip.setPixelColor(36, color);
    strip.setPixelColor(43, color);
    strip.setPixelColor(49, color);
    strip.setPixelColor(46, color);
    strip.setPixelColor(39, color);
    strip.setPixelColor(31, color);
    strip.setPixelColor(22, color);
    strip.setPixelColor(14, color);
    strip.setPixelColor(7, color);
  }
  else if (ring == 4) {
    strip.setPixelColor(5, color);
    strip.setPixelColor(12, color);
    strip.setPixelColor(20, color);
    strip.setPixelColor(29, color);
    strip.setPixelColor(37, color);
    strip.setPixelColor(44, color);
    strip.setPixelColor(45, color);
    strip.setPixelColor(38, color);
    strip.setPixelColor(30, color);
    strip.setPixelColor(21, color);
    strip.setPixelColor(13, color);
    strip.setPixelColor(6, color);
    strip.setPixelColor(0, color);
    // OFF BY ONE A:LLJKSDFHLKJSDF
    //strip.setPixelColor(51, color);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
