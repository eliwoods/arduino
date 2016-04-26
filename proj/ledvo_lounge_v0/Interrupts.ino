// Basically these are the commands that will get run when the button is pressed on an interrupt pin.
// Look at the setup function to see what pins are being used for interrupts. These are just the short
// and sweet interrupt service requests and their debounce methods since all buttons are shit. Also include any
// functions that

// Increment the palette counter. This lets us switch palettes. Debounce function
// included below.
void palette_ISR() {
  gPaletteCounter = (gPaletteCounter+1)%numPalettes;
}

void debounce_palette() {
  if((int32_t)(micros() - last_micros) >= debounce_time*1000) {
    palette_ISR();
    last_micros = micros();
  }
}

// This actually handles the switching of the global palette based on the palette counter
void updateGPalette() {
  switch(gPaletteCounter) {
    case 0:
      update_RainbowBlack_p();
      break;
    case 1:
      update_WhiteRainbow_p();
      break;
    case 2:
      update_WhiteCol_p();
      break;
    case 3:
      update_ColCol_p();
      break;
    case 4:
      update_ColLead_p();
      break;
    case 5:
      gPalette = WhiteBlack_p;
      break;
    // Have to use gGradientPalettes array because the gradient defines
    // are below us. Maybe we should move this to the Palettes tab so we don't
    // have to use this array shit.
    case 6:
      gPalette = gGradientPalettes[0];
      break;
    case 7:
      gPalette = gGradientPalettes[1];
      break;
    case 8:
      gPalette = gGradientPalettes[2];
      break;
  }
}

// Turn on and off palette autopilot mode. I need to figure out if we should slowly
// morph and ignore the modular palettes or what.
void palette_auto_ISR() {
  if (palette_autopilot) {
    palette_autopilot = false;
  }
  else {
    palette_autopilot = true;
  }
}

void debounce_palette_auto() {
  if ((int32_t)(micros() - last_micros) >= debounce_time*1000) {
    palette_ISR();
    last_micros = micros();
  }
}

// To let us switch animations with an input button
void anim_ISR() {
  gAnimCounter = (gAnimCounter+1)%numAnimation;
}

void debounce_anim() {
  if ((int32_t)(micros() - last_micros) >= debounce_time*1000) {
    anim_ISR();
    last_micros = micros();
  }
}

