// Arrays to hold which led's are in which rings
const uint8_t ring0[] = {25};
const uint8_t ring1[] = {16, 17, 26, 34, 33, 24};
const uint8_t ring2[] = {15, 8, 9, 10, 18, 27, 35, 42, 41, 40, 32, 23};
const uint8_t ring3[] = {14, 7, 1, 2, 3, 4, 11, 19, 28, 36, 43, 49, 48, 47, 46, 39, 31, 22};
const uint8_t ring4[] = {13, 6, 0, 5, 12, 20, 29, 37, 44, 45, 38, 30, 21};


// Initialize board with random state;
void rand_init(float chance, uint32_t color, bool rnd_color) {
  float rnd;
  // Set color to white if one isn't specified and we aren't grabbing
  // random colors
  if (color == 0 && !rnd_color) {
    color = wheel(255);
  }
  // Set led state
  for (uint8_t led = 0; led < numLED; led++) {
    rnd = random(100) / 99.; // Turn int into random number between 0 and 1
    if (rnd > chance) {
      strip.setPixelColor(led, 0);
    }
    else {
      if (rnd_color) {
        strip.setPixelColor(led, wheel(random(256)));
      }
      else {
        strip.setPixelColor(led, color);
      }
    }
  }

  strip.show();
}

// Return array with led indices of nearest neighbors. Will always be 6 if continuous
// flag is used. Will be 3-6 otherwise
uint8_t nearestNeighbor(uint8_t led, uint8_t *nn, boolean continuous, boolean square) {
  // If we have a border at the edge, return the following:
  if (!continuous) {
    // For cleanup later on
    for (uint8_t _neigh = 0; _neigh < 6; _neigh++) {
      nn[_neigh] = 255;
    }
    // Use row number to determine the neighbors we need to exclude
    if (led == 38) { // Don't ask me why I had to do this shit. Most ridiculous bug of my life..
      uint8_t _nn[] = {30, 31, 255, 39, 255, 45};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
    }
    else if (getRowNum(led) == 0) {
      uint8_t _nn[] = {led - 1, led + 1, led + 6, led + 7};
      for (uint8_t i = 0; i < 4; i++) {
        nn[i] = _nn[i];
      }
      if (led == 0) {
        nn[0] = 255;
      }
      if (led == 5) {
        nn[1] = 255;
      }
    }
    else if (getRowNum(led) == 1) {
      uint8_t _nn[] = {led - 7, led - 6, led - 1, led + 1, led + 7, led + 8};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 6) {
        nn[0] = 255;
        nn[2] = 255;
      }
      if (led == 12) {
        nn[1] = 255;
        nn[3] = 255;
      }
    }
    else if (getRowNum(led) == 2) {
      uint8_t _nn[] = {led - 8, led - 7, led - 1, led + 1, led + 8, led + 9};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 13) {
        nn[0] = 255;
        nn[2] = 255;
      }
      if (led == 20) {
        nn[1] = 255;
        nn[3] = 255;
      }
    }
    else if (getRowNum(led) == 3) {
      uint8_t _nn[] = {led - 9, led - 8, led - 1, led + 1, led + 8, led + 9};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 21) {
        nn[0] = 255;
        nn[2] = 255;
        nn[4] = 255;
      }
      if (led == 29) {
        nn[1] = 255;
        nn[3] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 4) {
      uint8_t _nn[] = {led - 9, led - 8, led - 1, led + 1, led + 7, led + 8};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 30) {
        nn[2] = 255;
        nn[4] = 255;
      }
      if (led == 37) {
        nn[3] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 5) {
      uint8_t _nn[] = {led - 8, led - 7, led - 1, led + 1, led + 6, led + 7};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 38) {
        nn[2] = 255;
        nn[4] = 255;
      }
      if (led == 44) {
        nn[3] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 6) {
      uint8_t _nn[] = {led - 7, led - 6, led - 1, led + 1};
      for (uint8_t i = 0; i < 4; i++) {
        nn[i] = _nn[i];
      }
      if (led == 45) {
        nn[2] = 255;
      }
      if (led == 50) {
        nn[3] = 255;
      }
    }

    // Now cleanup array so that all neighbors are packed to the start of the
    // array and also return true number of nearest neighbors. Accomplish this
    // by just sorting in ascending order, since 255 is the
    uint8_t _tmp, i;
    int8_t j;
    for (i = 1; i < 6; i++) {
      _tmp = nn[i];
      j = i - 1;
      while (j >= 0 && nn[j] > _tmp) {
        nn[j + 1] = nn[j];
        j--;
      }
      nn[j + 1] = _tmp;

    }

    uint8_t numNeigh = 0;
    for (uint8_t _neigh = 0; _neigh < 6; _neigh++) {
      if (nn[_neigh] != 255) {
        numNeigh++;
      }
    }
    return numNeigh;
  }
}

// Return Ring Number (really need to figure out hex coordinate system..)
uint8_t getRingNum(uint8_t led) {
  if (led == 25) {
    return 0;
  }
  for (uint8_t _led = 0; _led < sizeof(ring1) / sizeof(uint8_t); _led++) {
    if (ring1[_led] == led) {
      return 1;
    }
  }
  for (uint8_t _led = 0; _led < sizeof(ring2) / sizeof(uint8_t); _led++) {
    if (ring2[_led] == led) {
      return 2;
    }
  }

  for (uint8_t _led = 0; _led < sizeof(ring3) / sizeof(uint8_t); _led++) {
    if (ring3[_led] == led) {
      return 3;
    }
  }

  for (uint8_t _led = 0; _led < sizeof(ring4) / sizeof(uint8_t); _led++) {
    if (ring4[_led] == led) {
      return 4;
    }
  }
}

// Return Row Number
uint8_t getRowNum(uint8_t led) {
  if (led >= 0 && led < 6) {
    return 0;
  }
  else if (led > 5 && led < 13) {
    return 1;
  }
  else if (led > 12 && led < 21) {
    return 2;
  }
  else if (led > 20 && led < 30) {
    return 3;
  }
  else if (led > 29 && led < 39) {
    return 4;
  }
  else if (led > 38 && led < 45) {
    return 5;
  }
  else if (led > 44 && led < 50) {
    return 6;
  }
}

// Handle all of the messy work for converting ring number to individual pixels
void setRingColor(uint8_t ring, uint32_t color) {
  if (ring == 0 ) {
    strip.setPixelColor(25, color);
  }
  else if (ring == 1) {
    for (uint8_t led = 0; led < sizeof(ring1) / sizeof(uint8_t); led++) {
      strip.setPixelColor(ring1[led], color);
    }
  }
  else if (ring == 2) {
    for (uint8_t led = 0; led < sizeof(ring2) / sizeof(uint8_t); led++) {
      strip.setPixelColor(ring2[led], color);
    }
  }
  else if (ring == 3) {
    for (uint8_t led = 0; led < sizeof(ring3) / sizeof(uint8_t); led++) {
      strip.setPixelColor(ring3[led], color);
    }
  }
  else if (ring == 4) {
    for (uint8_t led = 0; led < sizeof(ring4) / sizeof(uint8_t); led++) {
      strip.setPixelColor(ring4[led], color);
    }
  }
}

void fillRing(uint8_t ring, uint32_t color, boolean rev) {
  if (!rev) {
    if (ring == 0) {
      strip.setPixelColor(25, color);
      strip.show();
      delay(analogRead(POT));
    }
    if (ring == 1) {
      for (uint8_t led = 0; led < sizeof(ring1) / sizeof(uint8_t); led++) {
        strip.setPixelColor(ring1[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
    if (ring == 2) {
      for (uint8_t led = 0; led < sizeof(ring2) / sizeof(uint8_t); led++) {
        strip.setPixelColor(ring2[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
    if (ring == 3) {
      for (uint8_t led = 0; led < sizeof(ring3) / sizeof(uint8_t); led++) {
        strip.setPixelColor(ring3[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
    if (ring == 4) {
      for (uint8_t led = 0; led < sizeof(ring4) / sizeof(uint8_t); led++) {
        strip.setPixelColor(ring4[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
  }
  else {
    if (ring == 0) {
      strip.setPixelColor(25, color);
      strip.show();
      delay(analogRead(POT));
    }
    if (ring == 1) {
      for (int8_t led = (sizeof(ring1)/sizeof(uint8_t))-1; led >= 0; led--) {
        strip.setPixelColor(ring1[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
    if (ring == 2) {
      for (int8_t led = (sizeof(ring2)/sizeof(uint8_t))-1; led >= 0; led--) {
        strip.setPixelColor(ring2[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
    if (ring == 3) {
      for (int8_t led = (sizeof(ring3)/sizeof(uint8_t))-1; led >= 0; led--) {
        strip.setPixelColor(ring3[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
    if (ring == 4) {
      for (int8_t led = (sizeof(ring4)/sizeof(uint8_t))-1; led >= 0; led--) {
        strip.setPixelColor(ring4[led], color);
        strip.show();
        delay(analogRead(POT));
      }
    }
  }
}

