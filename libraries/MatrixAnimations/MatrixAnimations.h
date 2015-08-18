//
//  MatrixAnimations.h
//  
//
//  Created by Eli Woods on 8/17/15.
//
//

#ifndef ____MatrixAnimations__
#define ____MatrixAnimations__

#include <stdio.h>
#include "../RGBMatrixPanel/RGBMatrixPanel.h"
#include "../AdafuitGFXLibrary/Adafruit_GFX.h"

class MatrixAnimations {

	public:
		// Random Funcitons
		int init();
		float gaus(float, float);

		// Developed 
		uint8_t random_state();
		uint8_t uramp(int16_t, int16_t);
		uint8_t dramp(int16_t, int16_t);
		uint8_t spectrum(uint8_t, int);
		uint8_t cross_sweep(uint8_t, uint8_t, uint16_t color, int);
		uint16_t square_zoom(int, uint16_t*);

		// Needs finishing
		uint8_t hue_cycle();
		uint8_t vertical_race();
		uint8_t line_trace();
		uint8_t gol();
		uint8_t spiral();


	protected:
		uint8_t xmin;
		uint8_t xmax;
		uint8_t ymin;
		uint8_t ymax;

		RGBMatrixPanel *matrix;

};

#endif
