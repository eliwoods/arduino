// Initialize board with random state;
void rand_init(float chance, uint32_t color, bool rnd_color) {
  float rnd;
  // Set color to white if one isn't specified and we aren't grabbing
  // random colors
  if (color == 0 && !rnd_color) {
    color = Wheel(255);
  }
  // Set led state
  for (uint8_t led = 0; led < numLED; led++) {
    rnd = random(100) / 99.; // Turn int into random number between 0 and 1
    if (rnd > chance) {
      strip.setPixelColor(led, 0);
    }
    else {
      if (rnd_color) {
        strip.setPixelColor(led, Wheel(random(256)));
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
byte nearestNeighbor(byte led, byte *nn, boolean continuous, boolean square) {
  // Check if led is in a edge ring. Edge ring is definted
  // as a ring with any component on the edge.
  if (!continuous) {
    // For cleanup later on
    for (byte _neigh = 0; _neigh < 6; _neigh++) {
      nn[_neigh] = 255;
    }
    // Use row number to determine the neighbors we need to exclude
    if (led == 38) { // Don't ask me why I had to do this shit. Most ridiculous bug of my life..
      byte _nn[] = {30, 31, 255, 39, 255, 45};
      for (byte i = 0; i < 6; i++) {
        nn[i] = _nn[i];
      }
    }
    else if (getRowNum(led) == 0) {
      byte _nn[] = {led - 1, led + 1, led + 6, led + 7};
      for (byte i = 0; i < 4; i++) {
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
      byte _nn[] = {led - 7, led - 6, led - 1, led + 1, led + 7, led + 8};
      for (byte i = 0; i < 6; i++) {
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
      byte _nn[] = {led - 8, led - 7, led - 1, led + 1, led + 8, led + 9};
      for (byte i = 0; i < 6; i++) {
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
      byte _nn[] = {led - 9, led - 8, led - 1, led + 1, led + 8, led + 9};
      for (byte i = 0; i < 6; i++) {
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
      byte _nn[] = {led - 9, led - 8, led - 1, led + 1, led + 7, led + 8};
      for (byte i = 0; i < 6; i++) {
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
      byte _nn[] = {led - 8, led - 7, led - 1, led + 1, led + 6, led + 7};
      for (byte i = 0; i < 6; i++) {
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
      byte _nn[] = {led - 7, led - 6, led - 1, led + 1};
      for (byte i = 0; i < 4; i++) {
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
    byte _tmp, i;
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

    byte numNeigh = 0;
    for (byte _neigh = 0; _neigh < 6; _neigh++) {
      if (nn[_neigh] != 255) {
        numNeigh++;
      }
    }
    return numNeigh;
  }
}

// Return Ring Number (really need to figure out hex coordinate system..)
byte getRingNum(byte led, byte ringStyle) {
  // ringStyle = 0: 5 rings with center ring being one pixel
  // ringStyle = 1: 4 rings with center ring being 3 horizontal pixels
  if (ringStyle == 0) {
    if (led == 25) {
      return 0;
    }
    else if (led == 17 || led == 26 || led == 34 || led == 33 || led == 24 || led == 16) {
      return 1;
    }
    else if (led == 9 || led == 10 || led == 18 || led == 27 || led == 35 || led == 42
             || led == 40 || led == 41 || led == 32 || led == 23 || led == 15 || led == 8) {
      return 2;
    }
    else if (led == 1 || led == 2 || led == 3 || led == 4 || led == 11 || led == 19 || led == 28 || led == 36 || led == 43 ||
             led == 49 || led == 48 || led == 47 || led == 46 || led == 39 || led == 31 || led == 22 || led == 14 || led == 7) {
      return 3;
    }
    else if (led == 5 || led == 12 || led == 20 || led == 29 || led == 37 || led == 44 || led == 45
             || led == 38 || led == 30 || led == 21 || led == 13 || led == 6 || led == 0) {
      return 4;
    }
  }
  if (ringStyle == 1) {

  }
}

// Return Row Number
byte getRowNum(byte led) {
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
void setRingColor(uint8_t ring, uint32_t color, byte ringStyle) {
  if (ringStyle == 0) {
    if (ring == 0 ) {
      strip.setPixelColor(25, color);
    }
    else if (ring == 1) {
      strip.setPixelColor(17, color);
      strip.setPixelColor(26, color);
      strip.setPixelColor(34, color);
      strip.setPixelColor(33, color);
      strip.setPixelColor(24, color);
      strip.setPixelColor(16, color);

    }
    else if (ring == 2) {
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(18, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(35, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(32, color);
      strip.setPixelColor(23, color);
      strip.setPixelColor(15, color);


    }
    else if (ring == 3) {
      strip.setPixelColor(1, color);
      strip.setPixelColor(2, color);
      strip.setPixelColor(3, color);
      strip.setPixelColor(4, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(19, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(49, color);
      strip.setPixelColor(48, color);
      strip.setPixelColor(47, color);
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
  if (ringStyle == 1) {
    if (ring == 0) {
      strip.setPixelColor(24, color);
      strip.setPixelColor(25, color);
      strip.setPixelColor(26, color);
    }
    else if (ring == 1) {
      strip.setPixelColor(15, color);
      strip.setPixelColor(16, color);
      strip.setPixelColor(17, color);
      strip.setPixelColor(18, color);
      strip.setPixelColor(27, color);
      strip.setPixelColor(35, color);
      strip.setPixelColor(34, color);
      strip.setPixelColor(33, color);
      strip.setPixelColor(32, color);
      strip.setPixelColor(23, color);
    }
    else if (ring == 2) {
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      strip.setPixelColor(10, color);
      strip.setPixelColor(11, color);
      strip.setPixelColor(19, color);
      strip.setPixelColor(28, color);
      strip.setPixelColor(36, color);
      strip.setPixelColor(43, color);
      strip.setPixelColor(42, color);
      strip.setPixelColor(41, color);
      strip.setPixelColor(40, color);
      strip.setPixelColor(39, color);
      strip.setPixelColor(31, color);
      strip.setPixelColor(22, color);
      strip.setPixelColor(14, color);
    }
    else if (ring == 3) {
      for (int i = 0; i < 6; i++) {
        strip.setPixelColor(i, color);
      }
      strip.setPixelColor(12, color);
      strip.setPixelColor(20, color);
      strip.setPixelColor(29, color);
      strip.setPixelColor(37, color);
      strip.setPixelColor(44, color);
      for (int i = 49; i > 44; i--) {
        strip.setPixelColor(i, color);
      }
      strip.setPixelColor(38, color);
      strip.setPixelColor(30, color);
      strip.setPixelColor(21, color);
      strip.setPixelColor(13, color);
      strip.setPixelColor(6, color);
    }
  }
}
