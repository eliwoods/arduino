// Turn off the damn thing. Debounce included below.
void kill_ISR() {
  if(sState) {
    sState = false;
    for (uint8_t _LED = 0; _LED < strip.numPixels(); _LED++) {
      strip.setPixelColor(_LED, 0);
    }
  }
  else {
    sState = true;
  }
}

void debounce_kill() {
  if((int32_t)(micros()-last_micros) >= debounce_time*1000) {
    kill_ISR();
    last_micros = micros();
  }
}

// Reverse the direction of an animation if applicable. Debounce function
// included below.
void rev_ISR() {
  if(_rev) {
    _rev = false;
  }
  else {
    _rev = true;
  }
}

void debounce_rev() {
  if((int32_t)(micros() - last_micros) >= debounce_time*1000) {
    rev_ISR();
    last_micros = micros();
  }
}

// Increment the animation counter. This lets us switch animations. Debounce function
// included below.
void anim_ISR() {
  anim = (anim+1)%3;
  anim_switch = true;
}

void debounce_anim() {
  if((int32_t)(micros() - last_micros) >= debounce_time*1000) {
    anim_ISR();
    last_micros = micros();
  }
}

