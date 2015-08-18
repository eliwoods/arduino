#ifndef ____MatrixAnimation__
#define ____MatrixAnimation__

#include <stdio.h>
#include "../RGBMatrixPanel/RGBmatrixPanel.h"
#include "../AdafuitGFXLibrary/Adafruit_GFX.h"

class MatrixAnimation {

	public:
		MatrixAnimation();
		~MatrixAnimation();

		// Random Funcitons
		float gaus(float mu, float sigma);

		// Developed 
		uint8_t random_state();
		uint8_t uramp(int16_t _x, int16_t _y);
		uint8_t dramp(int16_t _x, int16_t _y);
		uint8_t spectrum(uint8_t bits, int del);
		uint8_t cross_sweep(uint8_t quadrant, uint8_t width, uint16_t color, int del);
		uint16_t square_zoom(int del, uint16_t *_cols);

		// Needs finishing
		uint8_t hue_cycle();
		uint8_t vertical_race();
		uint8_t line_trace();
		uint8_t gol();
		uint8_t spiral();


	protected:
		// For potential error handling in the future
		uint8_t ret; 

		uint8_t xmin;
		uint8_t xmax;
		uint8_t ymin;
		uint8_t ymax;

		RGBMatrixPanel *matrix;

};

#endif
