#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Arduino Variables
#define PIN 6
#define BUTTON 3
#define POT 5

// Important Light Variables
const uint8_t numLED = 51;
boolean sState = true; // Initialize board to off
boolean *ledState = new boolean[numLED]; // Second array for holding future states (first is built in neopixel class)
uint8_t *neigh = new uint8_t[6]; // Store nearest neighbors

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
  //strip.setBrightness(100);
  for (uint8_t led = 0; led < numLED; led++ ) {
    strip.setPixelColor(led, 0);
  }
  strip.show(); // Initialize all pixels to 'off'
}

// Just for testing functions ATM
void loop() {

  // Check if board is supposed to be on
  //  if (sState) {
  //random_walk(50, Wheel(random(256)), true, false);
  //game_of_life(100, 0.33, false);

  //  }
  //ripple_smooth(0);
  //ripple_split(1);
  //starry(10, 10, true);
  //ripple_single(0);
  Serial.println(analogRead(POT));
  delay(500);
}

/////////////////
// ANIMATIONS! //
/////////////////

// Random Walk that turns off in an opposite order when it cannot walk any further.
// Also can be considered the spidery fill.
void random_walk (uint32_t color, boolean del, boolean continuous) {
  // Some initial setup
  uint8_t numNeigh, nextNeigh, attempts;
  boolean progress = true;
  uint8_t led = random(numLED);
  strip.setPixelColor(led, color);
  strip.show();
  delay(analogRead(POT));
  uint8_t *order = new uint8_t[numLED];
  for (uint8_t led = 0; led < numLED; led++ ) {
    order[led] = 255;
  }

  // Now chose random neighbor, turn it on, and repeat until we can't
  // progress anymore. If board is continuous, the only blocking we have is
  // from the walk itself.
  uint8_t numItr = 0;
  while (true) {

    numNeigh = nearestNeighbor(led, neigh, false, false);
    // Attempt to grab a neighbor that is off. If we have
    // check all the neighbors and none are off, then exit the walk.
    nextNeigh = random(numNeigh);
    if (del) {
      while (strip.getPixelColor(neigh[nextNeigh]) != 0 || neigh[nextNeigh] == 50) {
        // Set attempted pixel to 255 and resort array
        neigh[nextNeigh] = 255;
        numNeigh--;
        uint8_t _tmp, i;
        int8_t j;
        for (i = 1; i < 6; i++) {
          _tmp = neigh[i];
          j = i - 1;
          while (j >= 0 && neigh[j] > _tmp) {
            neigh[j + 1] = neigh[j];
            j--;
          }
          neigh[j + 1] = _tmp;

        }
        // If there are no more neighbors, we have run into a dead end
        if (numNeigh == 0) {
          progress = false;
          break;
        }
        nextNeigh = random(numNeigh);
      }
    }
    else {
      while (strip.getPixelColor(neigh[nextNeigh]) == color || neigh[nextNeigh] == 50) {
        // Set attempted pixel to 255 and resort array
        neigh[nextNeigh] = 255;
        numNeigh--;
        uint8_t _tmp, i;
        int8_t j;
        for (i = 1; i < 6; i++) {
          _tmp = neigh[i];
          j = i - 1;
          while (j >= 0 && neigh[j] > _tmp) {
            neigh[j + 1] = neigh[j];
            j--;
          }
          neigh[j + 1] = _tmp;

        }
        // If there are no more neighbors, we have run into a dead end
        if (numNeigh == 0) {
          progress = false;
          break;
        }
        nextNeigh = random(numNeigh);
      }
    }
    order[numItr] = led;
    numItr++;
    if (!progress) {
      break;
    }
    // Turn next pixel on and update strip once we've passed the checks..
    strip.setPixelColor(neigh[nextNeigh], color);
    led = neigh[nextNeigh];
    strip.show();
    delay(analogRead(POT));
  }

  // Now that we are out, go through the order array and turn off led's. This gives
  // us the spidery effect we desire
  if (del) {
    for (int8_t led = 0; led < numLED; led++ ) {
      if (order[led] == 255) {
        break;
      }
      strip.setPixelColor(order[led], 0);
      strip.show();
      delay(analogRead(POT));

    }
  }
  // Cleanup
  delete order;

}

// Game of Life. Using non-local neighbor rules, which are meant for 8 nearest neighbors
// but should stil work for our purposes.
// * If a cell has exactly 3 neighbors, it is "born"
// * If a living cell has < 3 neighbors, it dies of lonliness
// * If a living cell has 3 or 4 neighbors, it stays alive
// * If a living cell has > 4 neighbors, it dies of overcrowding
void game_of_life(float starting_chance, boolean continuous) {
  // First, intialize the board with random state and setup some useful variables
  uint8_t numNeigh, livNeigh, numLive;
  rand_init(starting_chance, Wheel(50), false);
  numLive = 0;
  for (uint8_t led = 0; led < numLED; led++) {
    if (strip.getPixelColor(led) != 0) {
      numLive++;
    }
  }
  // Loop through every light, get nearest neighbors and check the rules. Store
  // future generation in in ledState array.
  delay(analogRead(POT));
  while (numLive != 0) {
    for (uint8_t led = 0; led < numLED; led++) {
      numNeigh = nearestNeighbor(led, neigh, false, false);
      livNeigh = 0;
      for (uint8_t nn = 0; nn < numNeigh; nn++) { // Loop over neighbors
        if (strip.getPixelColor(neigh[nn]) != 0) {
          livNeigh++;
        }
      }
      // Check Rules
      if (strip.getPixelColor(led) == 0) { // If cell off
        if (livNeigh == 3) {
          ledState[led] = true;
        }
      }
      else { // If cell on
        if (livNeigh < 2 || livNeigh > 3) {
          ledState[led] = false;
        }
        else if (livNeigh == 3 || livNeigh == 4) {
          ledState[led] = true;
        }
      }
    } // End loop over board

    // Now set new board state
    numLive = 0;
    for (uint8_t led = 0; led < numLED; led++) {
      if (ledState[led]) {
        strip.setPixelColor(led, Wheel(50));
        numLive++;
      }
      if (!ledState[led]) {
        strip.setPixelColor(led, 0);
      }
    }
    strip.show();
    delay(analogRead(POT));
  }
}

// Starry night like animation
void starry(uint8_t r_time, boolean white) {
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
    delay(analogRead(POT));
  }
  else {
    strip.setPixelColor(led, Wheel((uint8_t)random(255)));
    strip.show();
    delay(analogRead(POT));
  }
}

// Randomly assign each pixel a color
void chaos() {
  for (uint8_t led = 0; led < numLED; led++) {
    strip.setPixelColor(led, Wheel((uint8_t)random(255)));
  }
  strip.show();
  delay(analogRead(POT));
  return;
}

// Ripple Smooth Color Transition
void ripple_smooth(uint8_t ringStyle) {
  uint8_t numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (uint16_t j = 0; j < 256; j += 30) {
    for (uint8_t i = 0; i < numCrcl; i++) {
      setRingColor(i,  Wheel(((256 / strip.numPixels()) + j) & 255), ringStyle);
      strip.show();
      delay(analogRead(POT));
    }
  }
  return;
}

// Ripple every other ring.
void ripple_split(uint8_t ringStyle) {
  uint8_t numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t k = 0; k < 2; k++) {
      for (uint8_t i = 0; i < numCrcl; i = i + 2) {
        setRingColor(i + k, Wheel(( j + i * 256 / strip.numPixels()) & 255), ringStyle);
      }
      strip.show();
      delay(analogRead(POT));

      for (uint8_t i = 0; i < 5; i = i + 2) {
        setRingColor(i + k, 0, ringStyle);
      }

    }
  }
  return;
}

// Ripple one circle out then back in.
void ripple_single_rev(uint8_t ringStyle) {
  uint8_t numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t i = 0; i < numCrcl * 2; i++) {
      if (i < 5) {
        setRingColor(i, Wheel(( i * 256 / strip.numPixels() + j) & 255), ringStyle);
        strip.show();
        delay(analogRead(POT));
        setRingColor(i, 0, ringStyle);
      }
      else {
        setRingColor(2 * numCrcl - i, Wheel(( (10 - i) * 256 / strip.numPixels() + j) & 255), ringStyle);
        strip.show();
        delay(analogRead(POT));
        setRingColor(2 * numCrcl - i, 0, ringStyle);
      }
    }
  }
}

void ripple_single(uint8_t ringStyle) {
  uint8_t numCrcl;
  if (ringStyle == 0) {
    numCrcl = 5;
  }
  if (ringStyle == 1) {
    numCrcl = 4;
  }

  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t i = 0; i < numCrcl; i++) {
      setRingColor(i, Wheel(( i * 256 / strip.numPixels() + j) & 255), ringStyle);
      strip.show();
      delay(analogRead(POT));
      setRingColor(i, 0, ringStyle);
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(analogRead(POT));
  }
}

void rainbow() {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(analogRead(POT));
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(analogRead(POT));
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(analogRead(POT));

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow() {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(analogRead(POT));

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}




