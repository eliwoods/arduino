// This is for all ISR functions which will be used to capture button input while running animations.

// If the board is on, set the board state to of and wipe array clean.
// Otherwise, set the state to on.
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

// Flip reverse flag for animations that can be played in two directions
void rev_ISR() {
  if(_rev) {
    _rev = false;
  }
  else {
    _rev = true;
  }
}

// Currently just one button to increment the animation being played. Having to
void anim_ISR() {
  anim = (anim+1)%3;
  anim_switch = true;
}


