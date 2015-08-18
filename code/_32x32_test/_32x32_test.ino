// testshapes demo for Adafruit RGBmatrixPanel library.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For 32x32 RGB LED matrix:
// http://www.adafruit.com/products/607

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// If your 32x32 matrix has the SINGLE HEADER input,
// use this pinout:
#define CLK 11  // MUST be on PORTB!
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

#define XMIN 0
#define XMAX 32
#define YMIN 0
#define YMAX 32
// If your matrix has the DOUBLE HEADER input, use:
//#define CLK 8  // MUST be on PORTB!
//#define LAT 9
//#define OE  10
//#define A   A3
//#define B   A2
//#define C   A1
//#define D   A0
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// Global arrays for handling important stuff between function calls
uint16_t cols2[2];
uint16_t cols[XMAX / 4];

// Global time keeping variable
int cur_time, prev_time;

/////////////// TO DO ///////////////
void hue_cycle(); // Cycle through colors using ColorHSV function
void vertical_race(); // Think matrix text
void line_trace(); // Think snake
void gol(); // Game of Life (could be its own library tbh), might be too intense
void spiral(); // You know what this means

// My attempt at a memory efficient star setting
void random_state() {
  bool pix_state[XMAX][YMAX];
  
  // Randomly ramp up pixels
  for(int x=0; x<XMAX; x++) {
    for(int y=0; y<YMAX; y++) {
      if(random(2) > 0) {
          uramp(x,y);
          pix_state[x][y] = true; 
      } 
    }
  }
  
  // Ramp down pixels that were on
  for(int x=0; x<XMAX; x++) {
    for(int y=0; y<YMAX; y++) {
      if(pix_state[x][y]) {
        dramp(x,y);
      }
    }
  }
  
}

// Ramp on or off pixel to white
void uramp(int16_t _x, int16_t _y) {

  for (uint8_t i = 0; i < 16; i++) {
    matrix.drawPixel(_x, _y, matrix.Color444(i, i, i));
    delay(20);
  }

}

void dramp(int16_t _x, int16_t _y) {

  for (uint8_t i = 15; i != 255; i--) {
    matrix.drawPixel(_x, _y, matrix.Color444(i, i, i));
    delay(20);
  }

}

float gaus(float mu = 1.0, float sigma = 1.0) {
  float x1, x2, w, y1;
  static float y2;       // Static declaration will exist in mem until exit
  static int phase = 0;  //

  if (phase == 1) {       // If we have a precalculated
    phase = 0;
    return y2 * sigma + mu;
  }
  phase = 1;

  do {
    x1 = 2.0 * (random(1e6) / 1.0e6) - 1.0;
    x2 = 2.0 * (random(1e6) / 1.0e6) - 1.0;
    w = x1 * x1 + x2 * x2;
  } while ( w >= 1.0 );

  w = sqrt( (-2.0 * log( w ) ) / w );
  y1 = x1 * w;
  y2 = x2 * w;
  return y1 * sigma + mu;
}

// Cycle through colors using ColorXXX
void spectrum(int bits, int del) {
  //Error checking
  if (del <= 0) {
    del = 1;
  }
  // Check bits, but pretty sure that shit doesn't work still

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

}

void cross_sweep(int quadrant, int width, uint16_t color, int del = 50) {

  // Error check on width
  if (width <= 0) {
    width = 1;
  }
  if (width > XMAX) {
    width = XMAX;
  }

  // Set bounds based on quadrants and loop
  // quadrant: 0 -> 3 clockwise from (0,0)
  int xmin, ymin, xmax, ymax;
  switch (quadrant) {
    // (0,0)
    case 0:
      for (int z = XMIN; z < XMAX - width; z++) {
        // If you want a color in the background just fill whole screen before drawing line
        //matrix.fillScreen(matrix.Color333(0, 7, 7));
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, YMIN, z + w, YMAX, color);
          matrix.drawLine(XMIN, z + w, XMAX, z + w, color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, YMIN, z + w, YMAX, matrix.Color333(0, 0, 0));
          matrix.drawLine(XMIN, z + w, XMAX, z + w, matrix.Color333(0, 0, 0));
        }
      }
      break;
    // (XMAX, 0)
    case 1:
      for (int z = XMIN; z < XMAX - width; z++) {
        for (int w = 0; w < width; w++) {
          matrix.drawLine(XMAX - (z + w), YMIN, XMAX - (z + w), YMAX, color);
          matrix.drawLine(XMIN, z + w, XMAX, z + w, color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(XMAX - (z + w), YMIN, XMAX  - (z + w), YMAX, matrix.Color333(0, 0, 0));
          matrix.drawLine(XMIN, z + w, XMAX, z + w, matrix.Color333(0, 0, 0));
        }
      }
      break;
    // (XMAX, YMAX)
    case 2:
      for (int z = XMIN; z < XMAX + width; z++) {
        for (int w = 0; w < width; w++) {
          matrix.drawLine(XMAX - (z + w), YMIN,  XMAX - (z + w), YMAX, color);
          matrix.drawLine(XMIN, YMAX - (z + w), XMAX,  YMAX - (z + w), color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(XMAX - (z + w), YMIN, XMAX - (z + w), YMAX, matrix.Color333(0, 0, 0));
          matrix.drawLine(XMIN, YMAX - (z + w), XMAX, YMAX - (z + w), matrix.Color333(0, 0, 0));
        }
      }
      break;
    // (0, YMAX)
    case 3:
      for (int z = XMIN; z < XMAX + width; z++) {
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, YMIN, z + w, YMAX, color);
          matrix.drawLine(XMIN, YMAX - (z + w), XMAX, YMAX - (z + w), color);
        }
        delay(del);
        // To clear the lines, otherwise they stay on screen
        for (int w = 0; w < width; w++) {
          matrix.drawLine(z + w, YMIN, z + w, YMAX, matrix.Color333(0, 0, 0));
          matrix.drawLine(XMIN, YMAX - (z + w), XMAX, YMAX - (z + w), matrix.Color333(0, 0, 0));
        }
      }
      break;
  }


}

void square_zoom(int del, uint16_t *_cols) {

  // Error checking
  if (del < 0) {
    del = 0;
  }
  // FIGURE OUT HOW TO DO THIS ERROR CHECK!!! doesn't work if uncommented
  /*if (col_bits != 3 && col_bits != 4 && col_bits != 8) {
    Serial.print("INVALID NUMBER OF COLOR BITS.\n");
  }*/

  int j = 0;
  for (int16_t i = 1; i <= XMAX / 2; i++) {
    matrix.drawRect(XMAX / 2 - i , YMAX / 2 - i, i * 2, i * 2, _cols[j]);
    if (i % 2 == 0) {
      j++;
    }
  }
  delay(del);

  // Shift color values down the line and add a new one to the beginning
  uint16_t _tmp;
  for (int k = XMAX / 4 - 1; k > 0; k--) {
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

}

void square_zoom2(int del, uint16_t _cols[]) {
  for (int16_t j = 0; j < 2; j++) {
    for (int16_t i = 1; i <= XMAX / 2; i++) {

      if (j == 0) {
        if ( i % 4 == 0 || i % 4 == 1) {
          matrix.drawRect(XMAX / 2 - i, YMAX / 2 - i, i * 2, i * 2, _cols[0]);
        }
        else {
          matrix.drawRect(XMAX / 2 - i, YMAX / 2 - i, i * 2, i * 2, _cols[1]);
        }
      }
      else {
        if ( i % 4 == 2 || i % 4 == 3) {
          matrix.drawRect(XMAX / 2 - i, YMAX / 2 - i, i * 2, i * 2, _cols[0]);
        }
        else {
          matrix.drawRect(XMAX / 2 - i, YMAX / 2 - i, i * 2, i * 2, _cols[1]);
        }
      }



    }
    delay(del);
  }
}

void setup() {

  // Initialize important objects
  matrix.begin();
  Serial.begin(9600);

  // Read random analog noise from unused port
  randomSeed(analogRead(4));

  // Initialize color arrays for various functions

  // 2 color zoom
  uint16_t _cols2[2] = {matrix.Color333(0, 0, 7), matrix.Color333(7, 0, 7)};
  for (int i = 0; i < 2; i++) {
    cols2[i] = _cols2[i];
  }

  // MAX/4 color zoom
  uint8_t r, g, b;
  for (int i = 0; i < XMAX / 4; i++) {
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
    cols[i] = matrix.Color444(r, g, b);
    //cols[i] = matrix.Color444((uint8_t)random(2,8), (uint8_t)random(10,14), (uint8_t)random(6,10));
  }

}

void loop() {

  int ret;

  // Random time
  /*long rng_time = gaus(10, 50);
  while (rng_time < 0) {
    rng_time = gaus(10, 50);
  }
  for(int i=0; i<=25; i++) {
    square_zoom((int)rng_time, cols);
  }

  Serial.println(rng_time);*/

  // Ramp zoom speed
  /*for(int time = 5; time <= 50; time++) {
    for(int i=0; i<10; i++) {
      square_zoom(time, cols);
    }
  }
  for(int time = 50; time >= 5; time--) {
    for(int i=0; i<10; i++) {
      square_zoom(time, cols);
    }
  }*/ 

  square_zoom(30, cols);

  //  square_zoom2(100, cols2);
  //random_state();
  //byte pix_state[XMAX][YMAX];
  
  // Randomly ramp up pixels
//  for(int x=0; x<XMAX; x++) {
//    for(int y=0; y<YMAX; y++) {
//      //pix_state[x][y] = 0;
//      if(random(2) > 0) {
////          uramp(x,y);
//          matrix.drawPixel(x, y, matrix.Color444(15, 15, 15));
//          
//          //pix_state[x][y] = 1;
//         //dramp(x,y); 
//      } 
//    }
//  }

  //matrix.fillScreen(matrix.Color444(15, 15, 15));
  //matrix.swapBuffers(false);
  
//  // Ramp down pixels that were on
//  for(int x=0; x<XMAX; x++) {
//    for(int y=0; y<YMAX; y++) {
//      if(pix_state[x][y] == 1) {
//        dramp(x,y);
//      }
//    }
//  }

}
