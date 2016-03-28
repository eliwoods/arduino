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

void rev_ISR() {
  if(_rev) {
    _rev = false;
  }
  else {
    _rev = true;
  }
}

void anim_ISR() {
  anim = (anim+1)%3;
  anim_switch = true;
}


