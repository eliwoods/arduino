// author: Eli Woods

#include "Adafruit_GFX.h"
#include "RGBmatrixPanel.h"
#include "MatrixAnimation.h" // Animation library


// This variable controls the animations and board. There are no direct
// calls to the the board in the source code now.
MatrixAnimation matrix;

void setup() {

  // Arduino setup
  Serial.begin(9600);

  // Animation Setup
  matrix.rand_color_init();
}

void loop() {

  matrix.zoom_2col(100);
  //matrix.cross_sweep(0, 2, matrix.Color444(7, 0, 7), 100);

}
