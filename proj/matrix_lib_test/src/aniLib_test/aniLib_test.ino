
// author: Eli Woods

#include <MatrixAnimation.h> // Animation library
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

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

}
