#ifndef ____MatrixAnimation__
#define ____MatrixAnimation__

// C/C++ libraries
#include <stdio.h>

// Arduino Libraries
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
 #include "pins_arduino.h"
#endif

// Hardware libraries
//#include "../RGBMatrixPanel/RGBmatrixPanel.h"           
//#include "../AdafruitGFXLibrary/Adafruit_GFX.h"
#include "Adafruit_GFX.h"
#include "RGBmatrixPanel.h"           

// Should move this to constructor for modularity,
// but I onlyhave this panel for now.
#define CLK 11  // MUST be on PORTB!
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

class MatrixAnimation : public RGBmatrixPanel {

	public:
		// Constructors
		MatrixAnimation(const int xmin = 0, const int xmax = 32, const int ymin = 0, const int ymax = 32) :
		RGBmatrixPanel(A, B, C, D, CLK, LAT, OE, false) {
			// Set bounds of panel (useless when we only have one, but modularity...)
			_xmin = xmin;
			_xmax = xmax;
			_ymin = ymin;
			_ymax = ymax;

			// RGBmatrixPanel Stuff
			//_matrix = new RGBmatrixPanel(A, B, C, D, CLK, LAT, OE, false);
			this->begin();

			// Read in noise from unused analog port to randomly generate a seed
			randomSeed(analogRead(4));

			// Fill color arrays
			_col_sz = new uint16_t[_xmax/4];
			rand_color_init();
			set_2col(this->Color444(7, 0, 0), this->Color444(7, 0, 7));
		}
		
		~MatrixAnimation() {
			delete _col_sz;
			//delete _matrix;
		}

		// Helper Funcitons
		float gaus(float mu, float sigma);
		uint8_t rand_color_init();
		uint8_t set_2col(uint16_t col1, uint16_t col2);
		uint8_t uramp(int16_t _x, int16_t _y);
		uint8_t dramp(int16_t _x, int16_t _y);

		// Developed 
		uint8_t random_state();
		uint8_t spectrum(uint8_t bits, int del);
		uint8_t cross_sweep(uint8_t quadrant, uint8_t width, uint16_t color, int del);
		uint8_t square_zoom(int del);
		uint8_t zoom_2col(int del);

		// Needs finishing
		uint8_t hue_cycle();
		uint8_t vertical_race();
		uint8_t line_trace();
		uint8_t gol();
		uint8_t spiral();

	protected:
		// For potential error handling in the future
		uint8_t _ret; 

		// Time keeping
		int _cur_time, _prev_time;

		uint16_t _col_2c[2]; // 2 color zoom array
		uint16_t *_col_sz;  // random color zoom array

		// Bounds of Matrix
		uint8_t _xmin, _xmax, _ymin, _ymax;

		//RGBmatrixPanel *_matrix = NULL;
};

#endif
