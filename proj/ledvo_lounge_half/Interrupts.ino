// Basically these are the commands that will get run when the button is pressed on an interrupt pin.
// Look at the setup function to see what pins are being used for interrupts. These are just the short
// and sweet interrupt service requests and their debounce methods since all buttons are shit. Also include any
// functions that

// Increment the palette counter.
void palette_inc_ISR() {
  gPaletteCounter = (gPaletteCounter + 1) % (numPalStyles);
  if(gPaletteCounter == 0) {
    gSchemeNum = (gSchemeNum + 1) % numPalettes;
  }
}

void debounce_palette_inc() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    palette_inc_ISR();
    last_micros = micros();
  }
}

// Decrement the palette counter
void palette_dec_ISR() {
  if (gPaletteCounter == 0) {
    gPaletteCounter = numPalStyles - 1;
    if(gSchemeNum == 0) {
      gSchemeNum = numPalettes - 1;
    }
    else {
      gSchemeNum = gSchemeNum - 1;
    }
  }
  else {
    gPaletteCounter = gPaletteCounter - 1;
    if (gPaletteCounter == 0) {
      if(gSchemeNum == 0) {
        gSchemeNum = numPalettes - 1;
      }
      else {
        gSchemeNum = gSchemeNum - 1;
      }
    }
  }
}

void debounce_palette_dec() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    palette_dec_ISR();
    last_micros = micros();
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
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    palette_auto_ISR();
    last_micros = micros();
  }
}

// Increment global animation counter
void anim_inc_ISR() {
  gAnimCounter = (gAnimCounter + 1) % numAnimation;
  anim_switch = true;
}

void debounce_anim_inc() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    anim_inc_ISR();
    last_micros = micros();
  }
}

// Decrement global animation counter
void anim_dec_ISR() {
  if (gAnimCounter == 0) {
    gAnimCounter = numAnimation - 1;
  }
  else {
    gAnimCounter = gAnimCounter - 1;
  }
  anim_switch = true;
}

void debounce_anim_dec() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    anim_dec_ISR();
    last_micros = micros();
  }
}

// Turn on or off animation autopilot
void anim_auto_ISR() {
  if (!anim_autopilot) {
    anim_autopilot = true;
  }
  else {
    anim_autopilot = false;
  }
}

void debounce_anim_auto() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    anim_auto_ISR();
    last_micros = micros();
  }
}

// Sets flag for strobing DJ animation
void strobe_ISR() {
  if (!run_strobe) {
    run_strobe = true;
    dj_control = true;
  }
  else {
    run_strobe = false;
    dj_control = false;
  }
}

void debounce_strobe() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    strobe_ISR();
    last_micros = micros();
  }
}

// Sets flag for black strobing
void blk_strobe_ISR() {
  if (!run_blkstrobe) {
    run_blkstrobe = true;
  }
  else {
    run_blkstrobe = false;
  }
}

void debounce_blk_strobe() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    blk_strobe_ISR();
    last_micros = micros();
  }
}

// Sets flag for reversing animations
void anim_reverse_ISR() {
  if (!anim_reverse) {
    anim_reverse = true;
  }
  else {
    anim_reverse = false;
  }
}

void debounce_anim_reverse() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    anim_reverse_ISR();
    last_micros = micros();
  }
}

void kill_power_ISR() {
  if (power_on) {
    power_on = false;
  }
  else {
    power_on = true;
  }
}

void debounce_kill_power() {
  if ((int32_t)(micros() - last_micros) >= debounce_time * 1000) {
    kill_power_ISR();
    last_micros = micros();
  }
}

