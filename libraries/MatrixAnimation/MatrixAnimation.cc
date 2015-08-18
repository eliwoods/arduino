#include "MatrixAnimation.h"

MatrixAnimation::MatrixAnimation(RGBmatrixPanel *_matrix, int _xmin, int _xmax, int _ymin, int _ymax) {
	xmin = _xmin;
	xmax = _xmax;
	ymin = _ymin;
	ymax = _ymax;
	matrix = _matrix;	
}

MatrixAnimation::~MatrixAnimation();

uint8_t MatrixAnimation::random_state() {
	bool pix_state[xmax][ymax];
  
  // Randomly ramp up pixels
  for(int x=0; x<xmax; x++) {
    for(int y=0; y<ymax; y++) {
      if(random(2) > 0) {
          uramp(x,y);
          pix_state[x][y] = true; 
      } 
    }
  }
  
  // Ramp down pixels that were on
  for(int x=0; x<xmax; x++) {
    for(int y=0; y<ymax; y++) {
      if(pix_state[x][y]) {
        dramp(x,y);
      }
    }
  }

	return 0;
}

uint8_t MatrixAnimation::uramp(int16_t _x, int16_t _y) {
	for (uint8_t i = 0; i < 16; i++) {
    matrix.drawPixel(_x, _y, matrix.Color444(i, i, i));
    delay(20);
  }

	return 0;
}

uint8_t MatrixAnimation::dramp(int16_t _x, int16_t _y) {
	for (uint8_t i = 15; i != 255; i--) {
    matrix.drawPixel(_x, _y, matrix.Color444(i, i, i));
    delay(20);
  }

	return 0;
}

uint8_t MatrixAnimation::spectrum(uint8_t bits, int del) {
	//Error checking
  if (del <= 0) {
    del = 1;
  }
  if (bits != 3 && bits != 4 && bits != 8) {
    return -1;
  }

  uint8_t red, green, blue;

  if (bits == 3) {
    for (int i = 1; i < 8; i++) {
      red = 7 - i;
      green = i;
      matrix.fillScreen(matrix.Color333(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 8; i++) {
      green = 7 - i;
      blue = i;
      matrix.fillScreen(matrix.Color333(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 8; i++) {
      blue = 7 - i;
      red  = i;
      matrix.fillScreen(matrix.Color333(red, green, blue));
      delay(del);
    }
  }

  if (bits == 4) {
    for (int i = 1; i < 16; i++) {
      red = 15 - i;
      green = i;
      matrix.fillScreen(matrix.Color444(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 16; i++) {
      green = 15 - i;
      blue = i;
      matrix.fillScreen(matrix.Color444(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 16; i++) {
      blue = 15 - i;
      red  = i;
      matrix.fillScreen(matrix.Color444(red, green, blue));
      delay(del);
    }
  }

  if (bits == 8) {
    for (int i = 1; i < 256; i++) {
      red = 255 - i;
      green = i;
      matrix.fillScreen(matrix.Color888(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 256; i++) {
      green = 255 - i;
      blue = i;
      matrix.fillScreen(matrix.Color888(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 256; i++) {
      blue = 255 - i;
      red  = i;
      matrix.fillScreen(matrix.Color888(red, green, blue));
      delay(del);
    }
  }

	return 0;
}

uint8_t MatrixAnimation::cross_sweep(uint8_t quadrant, uint8_t width, uint16_t color, int del = 50) {
	// Error check on width
  if (width <= 0) {
    width = 1;
  }
  if (width > xmax) {
    width = xmax;
  }

  // Set bounds based on quadrants and loop
  // quadrant: 0 -> 3 clockwise from (0,0)
  switch (quadrant) {
    // (0,0)
    case 0:
      for (int z = xmin; z < xmax - width; z++) {
        // If you want a color in the background just fill whole screen before drawing line
        //matrix.fillScreen(matrix.Color333(0, 7, 7));
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, ymin, z + w, ymax, color);
          matrix.drawLine(xmin, z + w, xmax, z + w, color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, ymin, z + w, ymax, matrix.Color333(0, 0, 0));
          matrix.drawLine(xmin, z + w, xmax, z + w, matrix.Color333(0, 0, 0));
        }
      }
      break;
    // (xmax, 0)
    case 1:
      for (int z = xmin; z < xmax - width; z++) {
        for (int w = 0; w < width; w++) {
          matrix.drawLine(xmax - (z + w), ymin, xmax - (z + w), ymax, color);
          matrix.drawLine(xmin, z + w, xmax, z + w, color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(xmax - (z + w), ymin, xmax  - (z + w), ymax, matrix.Color333(0, 0, 0));
          matrix.drawLine(xmin, z + w, xmax, z + w, matrix.Color333(0, 0, 0));
        }
      }
      break;
    // (xmax, ymax)
    case 2:
      for (int z = xmin; z < xmax + width; z++) {
        for (int w = 0; w < width; w++) {
          matrix.drawLine(xmax - (z + w), ymin,  xmax - (z + w), ymax, color);
          matrix.drawLine(xmin, ymax - (z + w), xmax,  ymax - (z + w), color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(xmax - (z + w), ymin, xmax - (z + w), ymax, matrix.Color333(0, 0, 0));
          matrix.drawLine(xmin, ymax - (z + w), xmax, ymax - (z + w), matrix.Color333(0, 0, 0));
        }
      }
      break;
    // (0, ymax)
    case 3:
      for (int z = xmin; z < xmax + width; z++) {
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, ymin, z + w, ymax, color);
          matrix.drawLine(xmin, ymax - (z + w), xmax, ymax - (z + w), color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, ymin, z + w, ymax, matrix.Color333(0, 0, 0));
          matrix.drawLine(xmin, ymax - (z + w), xmax, ymax - (z + w), matrix.Color333(0, 0, 0));
        }
      }
      break;
  }
	return 0;
}

uint8_t MatrixAnimation::square_zoom(int del, uint16_t *_cols) {
	
  // Error checking
  if (del < 0) {
    del = 0;
  }
  if (col_bits != 3 && col_bits != 4 && col_bits != 8) {
    return -1;
  }

  int j = 0;
  for (int16_t i = 1; i <= xmax / 2; i++) {
    matrix.drawRect(xmax / 2 - i , ymax / 2 - i, i * 2, i * 2, _cols[j]);
    if (i % 2 == 0) {
      j++;
    }
  }
  delay(del);

  // Shift color values down the line and add a new one to the beginning
  uint16_t _tmp;
  for (int k = xmax / 4 - 1; k > 0; k--) {
    _cols[k] = _cols[k - 1];
  }
  /*uint8_t r = random(6, 10);
  uint8_t g = random(1, r);
  uint8_t b = random(r, 16);*/
  uint8_t r, g, b;
  r = 0;
  g = 0;
  b = 0;
  while ( r == 0 && g == 0 && b == 0) {
    if (random(2) > 0) {
      r = random(1, 16);
    }
    else {
      r = 0;
    }
    if (random(2) > 0) {
      g = random(1, 16);
    }
    else {
      g = 0;
    }
    if (random(2) > 0) {
      b = random(1, 16);
    }
    else {
      b = 0;
    }
  }
  _cols[0] = matrix.Color444(r, g, b);
  //_cols[0] = matrix.Color444((uint8_t)random(2,8), (uint8_t)random(10,14), (uint8_t)random(6,10));
	
	return 0;
}

uint8_t MatrixAnimation::hue_cycle() {

	return ret;
}

uint8_t MatrixAnimation::vertical_race() {

	return ret;
}

