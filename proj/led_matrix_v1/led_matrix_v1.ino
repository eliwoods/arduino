// C Includes
#include <math.h>

// Arduino Includes
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#include <avr/interrupt.h>
#endif

// Variables for pins on the arduino board
#define LED 6
#define REV_BUTTON 3
#define ANIM_BUTTON 2
#define POT 0

// Variables for pin interrupts
volatile boolean sState = false; // Used for kill switch
volatile boolean _rev = false; // For direction of certain animations
volatile uint8_t anim = 0; // For choosing index of animation
volatile boolean anim_switch = false;

// Important Light Variables
const uint8_t numLED = 97;
const uint8_t numCrcl = 7;
boolean *ledState = new boolean[numLED]; // Second array for holding future states (first is built in neopixel class)
uint32_t *cols = new uint32_t[numCrcl]; // Global random color array
uint8_t neigh[6]; // Store nearest neighbors

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLED, LED, NEO_GRB + NEO_KHZ800);

void setup() {

  // Utility setup
  Serial.begin(9600);
  randomSeed(analogRead(5));

  // Initialize pixelStates to off
  for (int8_t i = 0; i < numLED; i++) {
    ledState[i] = false;
  }

  // For reversing certain animations
  pinMode(REV_BUTTON, INPUT);
  Serial.print("Using pin ");
  Serial.print(REV_BUTTON);
  Serial.println(" as interrupt for reverse.");
  attachInterrupt(digitalPinToInterrupt(REV_BUTTON), rev_ISR, CHANGE);

  // For chosing animation with button
  pinMode(ANIM_BUTTON, INPUT);
  Serial.print("Using pin ");
  Serial.print(ANIM_BUTTON);
  Serial.println(" as interrupt for animation choice.");
  attachInterrupt(digitalPinToInterrupt(ANIM_BUTTON), anim_ISR, CHANGE);

  // Initialize array of random colors to be cycled.
  for (uint8_t col = 0; col < numCrcl; col++) {
    cols[col] = wheel(random(256));
  }

  // Strip setup
  strip.begin();
  strip.setBrightness(100);
  for (uint8_t led = 0; led < numLED; led++ ) {
    strip.setPixelColor(led, 0);
  }
  strip.show(); // Initialize all pixels to 'off'
}

// Just for testing functions ATM
void loop() {

  // spiral(wheel(random(256)), false, true);
  // ripple_smooth(false);
  // ripple_single_rev();
  // random_walk(wheel(random(256)), false, false);
  // game_of_life(.3, false);
  
}

/////////////////
// ANIMATIONS! //
/////////////////

void spiral (uint32_t color, boolean rev, boolean del) {
  if (!rev) {
    for (uint8_t rng = 0; rng < numCrcl; rng++) {
      fillRing(rng, color, false);
    }
    if (del) {
      for (uint8_t rng = 0; rng < numCrcl; rng++) {
        fillRing(rng, 0, false);
      }
    }
  }
  else {
    for (int8_t rng = numCrcl - 1; rng >= 0; rng--) {
      fillRing(rng, color, true);
    }
    if (del) {
      for (int8_t rng = numCrcl - 1; rng >= 0; rng--) {
        fillRing(rng, 0, true);
      }
    }
  }
}

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

    //
    numNeigh = nearestNeighbor(led, neigh, false, false);
    nextNeigh = random(numNeigh);

    // Attempt to grab a neighbor that is off. If we have
    // check all the neighbors and none are off, then exit the walk.
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
    // Turn next pixel on, and set the current pixel to its index. Then update and repeat.
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
  rand_init(starting_chance, wheel(50), false);
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
        strip.setPixelColor(led, wheel(50));
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
    strip.setPixelColor(led, wheel((uint8_t)random(255)));
    strip.show();
    delay(analogRead(POT));
  }
}

// Randomly assign each pixel a color
void chaos() {
  for (uint8_t led = 0; led < numLED; led++) {
    strip.setPixelColor(led, wheel((uint8_t)random(255)));
  }
  strip.show();
  delay(analogRead(POT));
  return;
}


// Ripple Smooth Color Transition
void ripple_smooth( boolean rndm) {
  if (rndm) {
    uint32_t color = wheel(random(256));
    for (uint8_t i = 0; i < numCrcl; i++) {
      setRingColor(i, color);
      strip.show();
      delay(analogRead(POT));
    }
  }
  else {
    for (uint16_t j = 0; j < 256; j += 10) {
      for (uint8_t i = 0; i < numCrcl; i++) {
        setRingColor(i,  wheel(((256 / strip.numPixels()) + j) & 255));
        strip.show();
        delay(analogRead(POT));
      }
    }
  }
  return;
}

// Ripple every other ring.
void ripple_split() {
  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t k = 0; k < 2; k++) {
      for (uint8_t i = 0; i < numCrcl; i = i + 2) {
        setRingColor(i + k, wheel(( j + i * 256 / strip.numPixels()) & 255));
      }
      strip.show();
      delay(analogRead(POT) * 0.4888);
      //Serial.println(analogRead(POT)*0.4888);

      for (uint8_t i = 0; i < 5; i = i + 2) {
        setRingColor(i + k, 0);
      }

    }
  }
  return;
}

// Ripple one circle out then back in.
void ripple_single_rev() {
  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t i = 0; i < numCrcl * 2; i++) {
      if (i < 5) {
        setRingColor(i, wheel(( i * 256 / strip.numPixels() + j) & 255));
        strip.show();
        delay(analogRead(POT));
        setRingColor(i, 0);
      }
      else {
        setRingColor(2 * numCrcl - i, wheel(( (10 - i) * 256 / strip.numPixels() + j) & 255));
        strip.show();
        delay(analogRead(POT));
        setRingColor(2 * numCrcl - i, 0);
      }
    }
  }
}

void ripple_single(boolean rev) {
  if (!rev) {
    for (uint16_t j = 0; j < 256; j = j + 10) {
      for (uint8_t i = 0; i < numCrcl; i++) {
        setRingColor(i, wheel(( i * 256 / strip.numPixels() + j) & 255));
        strip.show();
        delay(analogRead(POT));
        setRingColor(i, 0);
      }
    }
  }
  else {
    for (uint16_t j = 0; j < 256; j = j + 10) {
      for (int8_t i = numCrcl - 1; i >= 0; i--) {
        setRingColor(i, wheel(( i * 256 / strip.numPixels() + j) & 255));
        strip.show();
        delay(analogRead(POT));
        setRingColor(i, 0);
      }
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
      strip.setPixelColor(i, wheel((i + j) & 255));
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
      strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
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
        strip.setPixelColor(i + q, wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(analogRead(POT));

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}




