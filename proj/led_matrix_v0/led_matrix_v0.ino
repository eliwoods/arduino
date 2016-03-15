#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Arduino Variables
#define PIN 6
#define BUTTON 3

// Important Light Variables
const byte numLED = 51;
bool sState = true; // Initialize board to off
bool *ledState = new bool[numLED];
byte *neigh = new byte[6];

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLED, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  // Utility setup
  randomSeed(analogRead(0));

  // Initialize pixelStates to off
  for (int8_t i = 0; i < numLED; i++) {
    ledState[i] = false;
  }

  // For turning this bright shit off
  pinMode(BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), pin_ISR, CHANGE);

  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// Just for testing functions ATM
void loop() {

  // Check if board is supposed to be on
  if (sState) {
    ripple_smooth(10, 0);
    //ripple_split(100, 0);
    
    byte numNeigh;

    for (int i = 0; i < numLED; i++ ) {
      numNeigh = nearestNeighbor(i, neigh, false, false);
      for (byte j = 0; j < numNeigh; j++) {
        if (neigh[j] == 255) {
          continue;
        }
        strip.setPixelColor(neigh[j], Wheel(100));
      }
      strip.show();
      delay(150);
      for (byte j = 0; j < numNeigh; j++) {
        if (neigh[j] == 255) {
          continue;
        }
        strip.setPixelColor(neigh[j], 0);
      }
    }
    
  }

}

/////////////////
// ANIMATIONS! //
/////////////////

// Starry night like animation
void starry(uint8_t r_time, uint8_t hold, boolean white) {
  // r_time : ramp time

  int8_t led = random(strip.numPixels());

  // Turn led on if off
  if (white) {
    if (strip.getPixelColor(led) == 0) {
      for (uint8_t i = 0; i < 8; i++) {
        strip.setPixelColor(led, strip.Color(i, i, i));
        strip.show();
        delay(r_time);
      }
      ledState[led] = true;
    }
    // Turn led off if on
    else {
      for (uint8_t i = 0; i < 8; i++) {
        strip.setPixelColor(led, strip.Color(7 - i, 7 - i, 7 - i));
        strip.show();
        delay(r_time);
      }
      ledState[led] = false;
    }
    delay(hold);
  }
  else {
    strip.setPixelColor(led, Wheel((byte)random(255)));
    strip.show();
    delay(hold);
  }
}

// Randomly assign each pixel a color
void chaos(uint8_t hold) {
  for (uint8_t led = 0; led < numLED; led++) {
    strip.setPixelColor(led, Wheel((byte)random(255)));
  }
  strip.show();
  delay(hold);
}

// Ripple Smooth Color Transition
void ripple_smooth(uint8_t hold, byte ringStyle) {
  byte numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (uint8_t j = 0; j < 256; j += 5) {
    for (byte i = 0; i < numCrcl; i++) {
      setRingColor(i,  Wheel((( i * 256 / strip.numPixels()) + j) & 255), ringStyle);
      strip.show();
      delay(hold);
    }
  }
}

// Ripple every other ring.
void ripple_split(uint8_t hold, byte ringStyle) {
  byte numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (uint8_t j = 0; j < 256; j = j + 10) {
    for (byte k = 0; k < 2; k++) {
      for (byte i = 0; i < numCrcl; i = i + 2) {
        setRingColor(i + k, Wheel(( i + j * 10) & 255), ringStyle);
      }
      strip.show();
      delay(hold);

      for (int i = 0; i < 5; i = i + 2) {
        setRingColor(i + k, 0, ringStyle);
      }

    }
  }
}

// Ripple one circle out then back in.
void ripple_single_rev(uint8_t hold, byte ringStyle) {
  byte numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (byte j = 0; j < 256; j = j + 10) {
    for (byte i = 0; i < numCrcl * 2; i++) {
      if (i < 5) {
        setRingColor(i, Wheel(( i + j * 10) & 255), ringStyle);
        strip.show();
        delay(hold);
        setRingColor(i, 0, ringStyle);
      }
      else {
        setRingColor(2 * numCrcl - i, Wheel(( (10 - i) + j * 10) & 255), ringStyle);
        strip.show();
        delay(hold);
        setRingColor(2 * numCrcl - i, 0, ringStyle);
      }
    }
  }
}

void ripple_single(uint8_t hold, byte ringStyle) {
  byte numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (byte j = 0; j < 256; j = j + 10) {
    for (byte i = 0; i < numCrcl; i++) {
      setRingColor(i, Wheel(( i + j * 10) & 255), ringStyle);
      strip.show();
      delay(hold);
      setRingColor(i, 0, ringStyle);
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




