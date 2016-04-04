// Arrays to hold which led's are in which rings
const uint8_t ring0[] = {48};
const uint8_t ring1[] = {34, 35, 49, 62, 61, 47};
const uint8_t ring2[] = {33, 21, 22, 23, 36, 50, 63, 75, 74, 73, 60, 46};
const uint8_t ring3[] = {32, 20, 10, 11, 12, 13, 24, 37, 51, 64, 76, 86, 85, 84, 83, 72, 59, 45};
const uint8_t ring4[] = {31, 19, 9, 1, 2, 3, 4, 5, 14, 25, 38, 52, 65, 77, 87, 95, 94, 93, 92, 91, 82, 71, 58, 44};
const uint8_t ring5[] = {30, 18, 8, 0, 6, 15, 26, 39, 53, 66, 78, 88, 96, 90, 81, 70, 57, 43};
const uint8_t ring6[] = {29, 17, 7, 16, 27, 40, 54, 67, 79, 89, 80, 69, 56, 42};
const uint8_t ring7[] = {28, 41, 68, 55};

// Arrays for rings based on height
//const uint8_t e_ring0[] = ring0;
//const uint8_t e_ring1[] = ring 1;
const uint8_t e_ring2[] = {22, 36, 63, 74, 60, 33};
const uint8_t e_ring3[] = {11, 12, 23, 24, 37, 50, 64, 76, 75, 85, 84, 73, 72, 59, 46, 32, 20, 21};
const uint8_t e_ring4[] = {3, 13, 25, 51, 77, 86, 93, 83, 71, 45, 19, 10};
const uint8_t e_ring5[] = {1, 2, 4, 5, 14, 38, 52, 65, 88, 87, 95, 94, 92, 91, 82, 81, 58, 44, 31, 9};
const uint8_t e_ring6[] = {0, 6, 26, 39, 53, 66, 78, 96, 90, 70, 57, 43, 30, 18};
const uint8_t e_ring7[] = {15, 88, 81, 80};
const uint8_t e_ring8[] = {16, 40, 67, 89, 80, 56, 29, 7};
const uint8_t e_ring9[] = {42, 17, 27, 54, 79, 69, 42, 17};
const uint8_t e_ring10[] = {28, 41, 68, 55};

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

// Compute array with led indices of nearest neighbors. Will always return 6 if continuous
// flag is used. Will return 2-6 otherwise. Assumes we have hexagonal neighbors. There's probably
// a better way to do this but who really know with this configuration.
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
      uint8_t _nn[] = {led - 1, led + 1, led + 8, led + 9};
      for (uint8_t i = 0; i < 4; i++) {
        nn[i] = _nn[i];
      }
      if (led == 0) {
        nn[0] = 255;
      }
      if (led == 6) {
        nn[1] = 255;
      }
    }
    else if (getRowNum(led) == 1) {
      uint8_t _nn[] = {led - 9, led - 8, led - 1, led + 1, led + 10, led + 11};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 7) {
        nn[0] = 255;
        nn[1] = 255;
        nn[2] = 255;
      }
      if (led == 8) {
        nn[0] = 255;
      }
      if (led == 15) {
        nn[1] = 255;
      }
      if (led == 16) {
        nn[0] = 255;
        nn[1] = 255;
        nn[3] = 255;
      }
    }
    else if (getRowNum(led) == 2) {
      uint8_t _nn[] = {led - 11, led - 10, led - 1, led + 1, led + 12, led + 13};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 17) {
        nn[0] = 255;
        nn[2] = 255;
      }
      if (led == 27) {
        nn[1] = 255;
        nn[3] = 255;
      }
    }
    else if (getRowNum(led) == 3) {
      uint8_t _nn[] = {led - 13, led - 12, led - 1, led + 1, led + 13, led + 14};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 28) {
        nn[0] = 255;
        nn[1] = 255;
        nn[2] = 255;
        nn[4] = 255;
      }
      if (led == 29) {
        nn[0] = 255;
      }
      if (led == 40) {
        nn[1] = 255;
      }
      if (led == 41) {
        nn[0] = 255;
        nn[1] = 255;
        nn[3] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 4) {
      uint8_t _nn[] = {led - 14, led - 13, led - 1, led + 1, led + 13, led + 14};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 42) {
        nn[2] = 255;
      }
      if (led == 54) {
        nn[3] = 255;
      }
    }
    else if (getRowNum(led) == 5) {
      uint8_t _nn[] = {led - 14, led - 13, led - 1, led + 1, led + 12, led + 13};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 55) {
        nn[0] = 255;
        nn[2] = 255;
        nn[4] = 255;
        nn[5] = 255;
      }
      if (led == 56) {
        nn[4] = 255;
      }
      if (led == 67) {
        nn[5] = 255;
      }
      if (led == 68) {
        nn[1] = 255;
        nn[3] = 255;
        nn[4] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 6) {
      uint8_t _nn[] = {led - 13, led - 12, led - 1, led + 1, led + 10, led + 11};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 69) {
        nn[2] = 255;
        nn[4] = 255;
      }
      if (led == 79) {
        nn[3] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 7) {
      uint8_t _nn[] = {led - 9, led - 10, led - 1, led + 1, led + 8, led + 9};
      for (uint8_t i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
      if (led == 80) {
        nn[2] = 255;
        nn[4] = 255;
        nn[5] = 255;
      }
      if (led == 81) {
        nn[4] = 255;
      }
      if (led == 88) {
        nn[5] = 255;
      }
      if (led == 89) {
        nn[3] = 255;
        nn[4] = 255;
        nn[5] = 255;
      }
    }
    else if (getRowNum(led) == 8) {
      uint8_t _nn[] = {led - 9, led - 8, led - 1, led + 1};
      for (uint8_t i = 0; i < 4; i++) {
        nn[i] = _nn[i];
      }
      if (led == 90) {
        nn[2] = 255;
      }
      if (led == 96) {
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

// Return the ring number for a given led
uint8_t getRingNum(uint8_t led, boolean height) {
  if (height) {
    if (led == 48) {
      return 0;
    }
    for (uint8_t _led = 0; _led < sizeof(ring1); _led++) {
      if (ring1[_led] == led) {
        return 1;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring2); _led++) {
      if (e_ring2[_led] == led) {
        return 2;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring3); _led++) {
      if (e_ring3[_led] == led) {
        return 3;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring4); _led++) {
      if (e_ring4[_led] == led) {
        return 4;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring5); _led++) {
      if (e_ring5[_led] == led) {
        return 5;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring6); _led++) {
      if (e_ring6[_led] == led) {
        return 6;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring7); _led++) {
      if (e_ring7[_led] == led) {
        return 7;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring8); _led++) {
      if (e_ring8[_led] == led) {
        return 8;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring9); _led++) {
      if (e_ring9[_led] == led) {
        return 9;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(e_ring10); _led++) {
      if (e_ring10[_led] == led) {
        return 10;
      }
    }
  }
  else {
    if (led == 48) {
      return 0;
    }
    for (uint8_t _led = 0; _led < sizeof(ring1); _led++) {
      if (ring1[_led] == led) {
        return 1;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(ring2); _led++) {
      if (ring2[_led] == led) {
        return 2;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(ring3); _led++) {
      if (ring3[_led] == led) {
        return 3;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(ring4); _led++) {
      if (ring4[_led] == led) {
        return 4;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(ring5); _led++) {
      if (ring5[_led] == led) {
        return 5;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(ring6); _led++) {
      if (ring6[_led] == led) {
        return 6;
      }
    }
    for (uint8_t _led = 0; _led < sizeof(ring7); _led++) {
      if (ring7[_led] == led) {
        return 7;
      }
    }
  }
}

// Return the row number for a given led
uint8_t getRowNum(uint8_t led) {
  if (led >= 0 && led < 7) {
    return 0;
  }
  else if (led > 6 && led < 17) {
    return 1;
  }
  else if (led > 16 && led < 28) {
    return 2;
  }
  else if (led > 27 && led < 42) {
    return 3;
  }
  else if (led > 41 && led < 55) {
    return 4;
  }
  else if (led > 54 && led < 69) {
    return 5;
  }
  else if (led > 68 && led < 80) {
    return 6;
  }
  else if (led > 79 && led < 90) {
    return 7;
  }
  else if (led > 89 && led < numLED) {
    return 8;
  }
}

// Paints the whole ring with desired color
void setRingColor(uint8_t ring, uint32_t color, uint8_t orientation) {
  // Normal hexagon shaped concentric rings
  if (orientation == 0) {
    if (ring == 0 ) {
      strip.setPixelColor(48, color);
    }
    else if (ring == 1) {
      for (uint8_t led = 0; led < sizeof(ring1); led++) {
        strip.setPixelColor(ring1[led], color);
      }
    }
    else if (ring == 2) {
      for (uint8_t led = 0; led < sizeof(ring2); led++) {
        strip.setPixelColor(ring2[led], color);
      }
    }
    else if (ring == 3) {
      for (uint8_t led = 0; led < sizeof(ring3); led++) {
        strip.setPixelColor(ring3[led], color);
      }
    }
    else if (ring == 4) {
      for (uint8_t led = 0; led < sizeof(ring4); led++) {
        strip.setPixelColor(ring4[led], color);
      }
    }
    else if (ring == 5) {
      for (uint8_t led = 0; led < sizeof(ring5); led++) {
        strip.setPixelColor(ring5[led], color);
      }
    }
    else if (ring == 6) {
      for (uint8_t led = 0; led < sizeof(ring6); led++) {
        strip.setPixelColor(ring6[led], color);
      }
    }
    else if (ring == 7) {
      for (uint8_t led = 0; led < sizeof(ring7); led++) {
        strip.setPixelColor(ring7[led], color);
      }
    }
  }
  // For height based rings
  else if (orientation == 1) {
    if (ring == 0 ) {
      strip.setPixelColor(48, color);
    }
    else if (ring == 1) {
      for (uint8_t led = 0; led < sizeof(ring1); led++) {
        strip.setPixelColor(ring1[led], color);
      }
    }
    else if (ring == 2) {
      for (uint8_t led = 0; led < sizeof(e_ring2); led++) {
        strip.setPixelColor(e_ring2[led], color);
      }
    }
    else if (ring == 3) {
      for (uint8_t led = 0; led < sizeof(e_ring3); led++) {
        strip.setPixelColor(e_ring3[led], color);
      }
    }
    else if (ring == 4) {
      for (uint8_t led = 0; led < sizeof(e_ring4); led++) {
        strip.setPixelColor(e_ring4[led], color);
      }
    }
    else if (ring == 5) {
      for (uint8_t led = 0; led < sizeof(e_ring5); led++) {
        strip.setPixelColor(e_ring5[led], color);
      }
    }
    else if (ring == 6) {
      for (uint8_t led = 0; led < sizeof(e_ring6); led++) {
        strip.setPixelColor(e_ring6[led], color);
      }
    }
    else if (ring == 7) {
      for (uint8_t led = 0; led < sizeof(e_ring7); led++) {
        strip.setPixelColor(e_ring7[led], color);
      }
    }
    else if (ring == 8) {
      for (uint8_t led = 0; led < sizeof(e_ring8); led++) {
        strip.setPixelColor(e_ring8[led], color);
      }
    }
    else if (ring == 9) {
      for (uint8_t led = 0; led < sizeof(e_ring9); led++) {
        strip.setPixelColor(e_ring9[led], color);
      }
    }
    else if (ring == 10) {
      for (uint8_t led = 0; led < sizeof(e_ring10); led++) {
        strip.setPixelColor(e_ring10[led], color);
      }
    }
  }
}

// Paints a ring one led at a time, basically used for spirals right now.
// Techincally this is an animation, but it's never called in the main
// loop so I'm calling it a utility
void fillRing(uint8_t ring, uint32_t color, uint16_t hold, uint8_t orientation, boolean rev) {
  // For normal concentric hexagon shaped rings
  if (orientation == 0) {
    // If forward
    if (!rev) {
      if (ring == 0) {
        strip.setPixelColor(25, color);
        strip.show();
        delay(hold);
      }
      else if (ring == 1) {
        for (uint8_t led = 0; led < sizeof(ring1); led++) {
          strip.setPixelColor(ring1[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 2) {
        for (uint8_t led = 0; led < sizeof(ring2); led++) {
          strip.setPixelColor(ring2[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 3) {
        for (uint8_t led = 0; led < sizeof(ring3); led++) {
          strip.setPixelColor(ring3[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 4) {
        for (uint8_t led = 0; led < sizeof(ring4); led++) {
          strip.setPixelColor(ring4[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 5) {
        for (uint8_t led = 0; led < sizeof(ring5); led++) {
          strip.setPixelColor(ring5[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 6) {
        for (uint8_t led = 0; led < sizeof(ring6); led++) {
          strip.setPixelColor(ring6[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 7) {
        for (uint8_t led = 0; led < sizeof(ring7); led++) {
          strip.setPixelColor(ring7[led], color);
          strip.show();
          delay(hold);
        }
      }
    }
    // if reversed
    else {
      if (ring == 0) {
        strip.setPixelColor(25, color);
        strip.show();
        delay(hold);
      }
      else if (ring == 1) {
        for (int8_t led = sizeof(ring1) - 1; led >= 0; led--) {
          strip.setPixelColor(ring1[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 2) {
        for (int8_t led = sizeof(ring2) - 1; led >= 0; led--) {
          strip.setPixelColor(ring2[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 3) {
        for (int8_t led = sizeof(ring3) - 1; led >= 0; led--) {
          strip.setPixelColor(ring3[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 4) {
        for (int8_t led = sizeof(ring4) - 1; led >= 0; led--) {
          strip.setPixelColor(ring4[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 5) {
        for (int8_t led = sizeof(ring5) - 1; led >= 0; led--) {
          strip.setPixelColor(ring5[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 6) {
        for (int8_t led = sizeof(ring6) - 1; led >= 0; led--) {
          strip.setPixelColor(ring6[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 7) {
        for (int8_t led = sizeof(ring7) - 1; led >= 0; led--) {
          strip.setPixelColor(ring7[led], color);
          strip.show();
          delay(hold);
        }
      }
    }
  }
  // For height based rings
  else if (orientation == 1) {
    // If forward
    if (!rev) {
      if (ring == 0) {
        strip.setPixelColor(25, color);
        strip.show();
        delay(hold);
      }
      else if (ring == 1) {
        for (uint8_t led = 0; led < sizeof(ring1); led++) {
          strip.setPixelColor(ring1[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 2) {
        for (uint8_t led = 0; led < sizeof(e_ring2); led++) {
          strip.setPixelColor(e_ring2[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 3) {
        for (uint8_t led = 0; led < sizeof(e_ring3); led++) {
          strip.setPixelColor(e_ring3[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 4) {
        for (uint8_t led = 0; led < sizeof(e_ring4); led++) {
          strip.setPixelColor(e_ring4[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 5) {
        for (uint8_t led = 0; led < sizeof(e_ring5); led++) {
          strip.setPixelColor(e_ring5[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 6) {
        for (uint8_t led = 0; led < sizeof(e_ring6); led++) {
          strip.setPixelColor(e_ring6[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 7) {
        for (uint8_t led = 0; led < sizeof(e_ring7); led++) {
          strip.setPixelColor(e_ring7[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 8) {
        for (uint8_t led = 0; led < sizeof(e_ring8); led++) {
          strip.setPixelColor(e_ring8[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 9) {
        for (uint8_t led = 0; led < sizeof(e_ring9); led++) {
          strip.setPixelColor(e_ring9[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 10) {
        for (uint8_t led = 0; led < sizeof(e_ring10); led++) {
          strip.setPixelColor(e_ring10[led], color);
          strip.show();
          delay(hold);
        }
      }
    }
    // if reversed
    else {
      if (ring == 0) {
        strip.setPixelColor(25, color);
        strip.show();
        delay(hold);
      }
      else if (ring == 1) {
        for (int8_t led = sizeof(ring1) - 1; led >= 0; led--) {
          strip.setPixelColor(ring1[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 2) {
        for (int8_t led = sizeof(e_ring2) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring2[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 3) {
        for (int8_t led = sizeof(e_ring3) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring3[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 4) {
        for (int8_t led = sizeof(e_ring4) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring4[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 5) {
        for (int8_t led = sizeof(e_ring5) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring5[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 6) {
        for (int8_t led = sizeof(e_ring6) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring6[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 7) {
        for (int8_t led = sizeof(e_ring7) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring7[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 8) {
        for (int8_t led = sizeof(e_ring8) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring8[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 9) {
        for (int8_t led = sizeof(e_ring9) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring9[led], color);
          strip.show();
          delay(hold);
        }
      }
      else if (ring == 10) {
        for (int8_t led = sizeof(e_ring10) - 1; led >= 0; led--) {
          strip.setPixelColor(e_ring10[led], color);
          strip.show();
          delay(hold);
        }
      }
    }
  }
}
