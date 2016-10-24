#include "MatrixAnimation.h"

uint8_t MatrixAnimation::random_state() {
	bool **pix_state = new bool*[_xmax];
	for(int x=0; x<_xmax; x++)
		pix_state[x] = new bool[_ymax];
  
  // Randomly ramp up pixels
  for(int x=0; x<_xmax; x++) {
    for(int y=0; y<_ymax; y++) {
      if(random(2) > 0) {
          uramp(x,y);
          pix_state[x][y] = true; 
      } 
    }
  }
  
  // Ramp down pixels that were on
  for(int x=0; x<_xmax; x++) {
    for(int y=0; y<_ymax; y++) {
      if(pix_state[x][y]) {
        dramp(x,y);
      }
    }
  }

	// Cleanup
	delete[] pix_state;

	return 0;
}

uint8_t MatrixAnimation::rand_color_init() {
	uint8_t r, g, b;
  for (int i = 0; i < _xmax/4; i++) {
    // CANDYLAND!
    /*r = random(6, 10);
    g = random(1, r);
    b = random(r, 16);*/

    // I think this works better though
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
    _col_sz[i] = this->Color444(r, g, b);
    //_col_sz[i] = this.Color444((uint8_t)random(2,8), (uint8_t)random(10,14), (uint8_t)random(6,10));
  }
}

uint8_t MatrixAnimation::set_2col(uint16_t col0, uint16_t col1) {
	_col_2c[0] = col0;
	_col_2c[1] = col1;
}

uint8_t MatrixAnimation::uramp(int16_t x, int16_t y) {
	for (uint8_t i = 0; i < 16; i++) {
    this->drawPixel(x, y, this->Color444(i, i, i));
    delay(20);
  }

	return 0;
}

uint8_t MatrixAnimation::dramp(int16_t x, int16_t y) {
	for (uint8_t i = 15; i != 255; i--) {
    this->drawPixel(x, y, this->Color444(i, i, i));
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
      this->fillScreen(this->Color333(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 8; i++) {
      green = 7 - i;
      blue = i;
      this->fillScreen(this->Color333(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 8; i++) {
      blue = 7 - i;
      red  = i;
      this->fillScreen(this->Color333(red, green, blue));
      delay(del);
    }
  }

  if (bits == 4) {
    for (int i = 1; i < 16; i++) {
      red = 15 - i;
      green = i;
      this->fillScreen(this->Color444(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 16; i++) {
      green = 15 - i;
      blue = i;
      this->fillScreen(this->Color444(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 16; i++) {
      blue = 15 - i;
      red  = i;
      this->fillScreen(this->Color444(red, green, blue));
      delay(del);
    }
  }

  if (bits == 8) {
    for (int i = 1; i < 256; i++) {
      red = 255 - i;
      green = i;
      this->fillScreen(this->Color888(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 256; i++) {
      green = 255 - i;
      blue = i;
      this->fillScreen(this->Color888(red, green, blue));
      delay(del);
    }
    for (int i = 1; i < 256; i++) {
      blue = 255 - i;
      red  = i;
      this->fillScreen(this->Color888(red, green, blue));
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
  if (width > _xmax) {
    width = _xmax;
  }

  // Set bounds based on quadrants and loop
  // quadrant: 0 -> 3 clockwise from (0,0)
  switch (quadrant) {
    // (0,0)
    case 0:
      for (int z = _xmin; z < _xmax - width; z++) {
        // If you want a color in the background just fill whole screen before drawing line
        //this->fillScreen(this->Color333(0, 7, 7));
        for (int w = 0; w < width; w++) {
          this->drawLine(z + w, _ymin, z + w, _ymax, color);
          this->drawLine(_xmin, z + w, _xmax, z + w, color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          this->drawLine(z + w, _ymin, z + w, _ymax, this->Color333(0, 0, 0));
          this->drawLine(_xmin, z + w, _xmax, z + w, this->Color333(0, 0, 0));
        }
      }
      break;
    // (_xmax, 0)
    case 1:
      for (int z = _xmin; z < _xmax - width; z++) {
        for (int w = 0; w < width; w++) {
          this->drawLine(_xmax - (z + w), _ymin, _xmax - (z + w), _ymax, color);
          this->drawLine(_xmin, z + w, _xmax, z + w, color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          this->drawLine(_xmax - (z + w), _ymin, _xmax  - (z + w), _ymax, this->Color333(0, 0, 0));
          this->drawLine(_xmin, z + w, _xmax, z + w, this->Color333(0, 0, 0));
        }
      }
      break;
    // (_xmax, _ymax)
    case 2:
      for (int z = _xmin; z < _xmax + width; z++) {
        for (int w = 0; w < width; w++) {
          this->drawLine(_xmax - (z + w), _ymin,  _xmax - (z + w), _ymax, color);
          this->drawLine(_xmin, _ymax - (z + w), _xmax,  _ymax - (z + w), color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          this->drawLine(_xmax - (z + w), _ymin, _xmax - (z + w), _ymax, this->Color333(0, 0, 0));
          this->drawLine(_xmin, _ymax - (z + w), _xmax, _ymax - (z + w), this->Color333(0, 0, 0));
        }
      }
      break;
    // (0, _ymax)
    case 3:
      for (int z = _xmin; z < _xmax + width; z++) {
        for (int w = 0; w < width; w++) {
          this->drawLine(z + w, _ymin, z + w, _ymax, color);
          this->drawLine(_xmin, _ymax - (z + w), _xmax, _ymax - (z + w), color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          this->drawLine(z + w, _ymin, z + w, _ymax, this->Color333(0, 0, 0));
          this->drawLine(_xmin, _ymax - (z + w), _xmax, _ymax - (z + w), this->Color333(0, 0, 0));
        }
      }
      break;
  }
	return 0;
}

uint8_t MatrixAnimation::square_zoom(int del) {
	
  // Error checking
  if (del < 0) {
    del = 0;
  }

  int j = 0;
  for (int16_t i = 1; i <= _xmax / 2; i++) {
    this->drawRect(_xmax / 2 - i , _ymax / 2 - i, i * 2, i * 2, _col_sz[j]);
    if (i % 2 == 0) {
      j++;
    }
  }
  delay(del);

  // Shift color values down the line and add a new one to the beginning
  uint16_t _tmp;
  for (int k = _xmax / 4 - 1; k > 0; k--) {
    _col_sz[k] = _col_sz[k - 1];
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
  _col_sz[0] = this->Color444(r, g, b);
  //_col_sz[0] = this->Color444((uint8_t)random(2,8), (uint8_t)random(10,14), (uint8_t)random(6,10));
	
	return 0;
}

uint8_t MatrixAnimation::zoom_2col(int del = 100) {

	for (int16_t j = 0; j < 2; j++) {
		for (int16_t i = 1; i <= _xmax / 2; i++) {

			if (j == 0) {
				if ( i % 4 == 0 || i % 4 == 1) {
					this->drawRect(_xmax / 2 - i, _ymax / 2 - i, i * 2, i * 2, _col_2c[0]);
				}
				else {
					this->drawRect(_xmax / 2 - i, _ymax / 2 - i, i * 2, i * 2, _col_2c[1]);
				}
			}
			else {
				if ( i % 4 == 2 || i % 4 == 3) {
					this->drawRect(_xmax / 2 - i, _ymax / 2 - i, i * 2, i * 2, _col_2c[0]);
				}
				else {
					this->drawRect(_xmax / 2 - i, _ymax / 2 - i, i * 2, i * 2, _col_2c[1]);
				}
			}

		}
		delay(del);
	}

	return 0;
}

uint8_t MatrixAnimation::hue_cycle() {

	return _ret;
}

uint8_t MatrixAnimation::vertical_race() {

	return _ret;
}

