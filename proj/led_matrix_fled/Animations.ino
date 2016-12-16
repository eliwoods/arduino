/////////////////
// ANIMATIONS! //
/////////////////

/*void spiral (uint32_t color, uint16_t hold, uint8_t orientation, boolean rev, boolean del) {
  // Orientation == 0 : Hexagon shaped rings
  // Orientation == 1 : Heigh based rings
  uint8_t numCrcl;
  if (orientation == 0) {
    numCrcl = 7;
  }
  else if (orientation == 1) {
    numCrcl = 10;
  }
  else {
    orientation = 0;
    numCrcl = 7;
  }

  // If spiral travels out from center
  if (!rev) {
    for (uint8_t rng = 0; rng < numCrcl; rng++) {
      fillRing(rng, color, hold, orientation, false);
    }
    if (del) {
      for (uint8_t rng = 0; rng < numCrcl; rng++) {
        fillRing(rng, 0, hold, orientation, false);
      }
    }
  }
  // If spiral travels in from outter ring
  else {
    for (int8_t rng = numCrcl - 1; rng >= 0; rng--) {
      fillRing(rng, color, hold, orientation, true);
    }
    if (del) {
      for (int8_t rng = numCrcl - 1; rng >= 0; rng--) {
        fillRing(rng, 0, hold, orientation, true);
      }
    }
  }
}
*/

// Need to find a way to no have this function hold up arduino while it runs, like how
// I've done everything else in fastLED
void random_walk (uint8_t _hue, uint16_t hold, boolean del, boolean continuous) {
  // Some initial setup
  uint8_t numNeigh, nextNeigh, attempts;
  boolean progress = true;
  uint8_t led = random(numLED);
  leds[led] = col;
  FastLED.show();
  FastLED.delay(hold);
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
    nextNeigh = random(numNeigh);

    // Attempt to grab a neighbor that is off. If we have
    // check all the neighbors and none are off, then exit the walk.
    if (del) {
      while (leds[neigh[nextNeigh]] == CRGB::Black) {
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
      // Check if the hue we're assigning is the same as the hue from a potential
      // nearest neighbor
      while (_hue == rgb2hsv_approximate(leds[neigh[nextNeigh]]).hue) { 
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
    leds[neight[nextNeigh]] = CHSV(_hue, 255, gBrightness);
    led = neigh[nextNeigh];
    FastLED.show();
    FastLED.delay(hold);
  }

  // Now that we are out, go through the order array and turn off led's. This gives
  // us the spidery effect we desire
  if (del) {
    for (int8_t led = 0; led < numLED; led++ ) {
      if (order[led] == 255) {
        break;
      }
      leds[order[led]] = CRGB::Black;
      FastLED.show();
      FastLED.delay(hold);

    }
  }
  // Cleanup
  delete order;

}

/*
// Game of Life. Using non-local neighbor rules, which are meant for 8 nearest neighbors
// but should stil work for our purposes.
// * If a cell has exactly 3 neighbors, it is "born"
// * If a living cell has < 3 neighbors, it dies of lonliness
// * If a living cell has 3 or 4 neighbors, it stays alive
// * If a living cell has > 4 neighbors, it dies of overcrowding
void game_of_life(uint16_t hold, float starting_chance, boolean continuous) {
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
  delay(hold);
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
    delay(hold);
  }
}

// Starry night like animation
void starry(uint16_t hold, uint8_t r_time, boolean white) {
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
    strip.setPixelColor(led, wheel((uint8_t)random(255)));
    strip.show();
    delay(hold);
  }
}

// Randomly assign each pixel a color
void chaos(uint16_t hold) {
  for (uint8_t led = 0; led < numLED; led++) {
    strip.setPixelColor(led, wheel((uint8_t)random(255)));
  }
  strip.show();
  delay(hold);
  return;
}


// Ripple Smooth Color Transition
void ripple_smooth(uint16_t hold, uint8_t orientation, boolean rndm) {
  // Orientation == 0 : Hexagon shaped rings
  // Orientation == 1 : Heigh based rings
  uint8_t numCrcl;
  if (orientation == 0) {
    numCrcl = 7;
  }
  else if (orientation == 1) {
    numCrcl = 10;
  }
  else {
    orientation = 0;
    numCrcl = 7;
  }

  if (rndm) {
    uint32_t color = wheel(random(256));
    for (uint8_t i = 0; i < numCrcl; i++) {
      setRingColor(i, color, orientation);
      strip.show();
      delay(hold);
    }
  }
  else {
    for (uint16_t j = 0; j < 256; j += 10) {
      for (uint8_t i = 0; i < numCrcl; i++) {
        setRingColor(i,  wheel(((256 / strip.numPixels()) + j) & 255), orientation);
        strip.show();
        delay(hold);
      }
    }
  }
}

// Ripple every other ring.
void ripple_split(uint16_t hold, uint8_t orientation) {
  // Orientation == 0 : Hexagon shaped rings
  // Orientation == 1 : Heigh based rings
  uint8_t numCrcl;
  if (orientation == 0) {
    numCrcl = 7;
  }
  else if (orientation == 1) {
    numCrcl = 10;
  }
  else {
    orientation = 0;
    numCrcl = 7;
  }

  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t k = 0; k < 2; k++) {
      for (uint8_t i = 0; i < numCrcl; i = i + 2) {
        setRingColor(i + k, wheel(( j + i * 256 / strip.numPixels()) & 255), orientation);
      }
      strip.show();
      delay(hold * 0.4888);
      //Serial.println(hold*0.4888);

      for (uint8_t i = 0; i < numCrcl; i = i + 2) {
        setRingColor(i + k, 0, orientation);
      }
    }
  }

}

// Ripple one circle out then back in.
void ripple_single_rev(uint16_t hold, uint8_t orientation) {
  // Orientation == 0 : Hexagon shaped rings
  // Orientation == 1 : Heigh based rings
  uint8_t numCrcl;
  if (orientation == 0) {
    numCrcl = 7;
  }
  else if (orientation == 1) {
    numCrcl = 10;
  }
  else {
    orientation = 0;
    numCrcl = 7;
  }

  for (uint16_t j = 0; j < 256; j = j + 10) {
    for (uint8_t i = 0; i < numCrcl * 2; i++) {
      if (i < numCrcl) {
        setRingColor(i, wheel(( i * 256 / strip.numPixels() + j) & 255), orientation);
        strip.show();
        delay(hold);
        setRingColor(i, 0, orientation);
      }
      else {
        setRingColor(2 * numCrcl - i, wheel(( (10 - i) * 256 / strip.numPixels() + j) & 255), orientation);
        strip.show();
        delay(hold);
        setRingColor(2 * numCrcl - i, 0, orientation);
      }
    }
  }
}

void ripple_single(uint16_t hold, uint8_t orientation, boolean rev) {
  // Orientation == 0 : Hexagon shaped rings
  // Orientation == 1 : Heigh based rings
  uint8_t numCrcl;
  if (orientation == 0) {
    numCrcl = 7;
  }
  else if (orientation == 1) {
    numCrcl = 10;
  }
  else {
    orientation = 0;
    numCrcl = 7;
  }

  if (!rev) {
    for (uint16_t j = 0; j < 256; j = j + 10) {
      for (uint8_t i = 0; i < numCrcl; i++) {
        setRingColor(i, wheel(( i * 256 / strip.numPixels() + j) & 255), orientation);
        strip.show();
        delay(hold);
        setRingColor(i, 0, orientation);
      }
    }
  }
  else {
    for (uint16_t j = 0; j < 256; j = j + 10) {
      for (int8_t i = numCrcl - 1; i >= 0; i--) {
        setRingColor(i, wheel(( i * 256 / strip.numPixels() + j) & 255), orientation);
        strip.show();
        delay(hold);
        setRingColor(i, 0, orientation);
      }
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint16_t hold, CRGB col) {
  fill_solid(leds, numLED, col);
}
*/
//void rainbow(uint16_t hold) {
//  uint16_t i, j;
//
//  for (j = 0; j < 256; j++) {
//    for (i = 0; i < strip.numPixels(); i++) {
//      strip.setPixelColor(i, wheel((i + j) & 255));
//    }
//    strip.show();
//    delay(hold);
//  }
//}
//
//// Slightly different, this makes the rainbow equally distributed throughout
//void rainbowCycle(uint16_t hold) {
//  uint16_t i, j;
//
//  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
//    for (i = 0; i < strip.numPixels(); i++) {
//      strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
//    }
//    strip.show();
//    delay(hold);
//  }
//}
//
////Theatre-style crawling lights.
//void theaterChase(uint32_t c, uint16_t hold) {
//  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
//    for (int q = 0; q < 3; q++) {
//      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, c);  //turn every third pixel on
//      }
//      strip.show();
//
//      delay(hold);
//
//      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, 0);      //turn every third pixel off
//      }
//    }
//  }
//}
//
////Theatre-style crawling lights with rainbow effect
//void theaterChaseRainbow(uint16_t hold) {
//  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
//    for (int q = 0; q < 3; q++) {
//      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, wheel( (i + j) % 255)); //turn every third pixel on
//      }
//      strip.show();
//
//      delay(hold);
//
//      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, 0);      //turn every third pixel off
//      }
//    }
//  }
//}
