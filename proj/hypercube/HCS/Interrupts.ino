// This will hold all of the ISR's and their debounce functions. The later might
// not be needed since our "buttons" are actually just signals from another arduino

void laser0_ISR() {
  laser0_on = !laser0_on;
}

void laser1_ISR() {
  laser1_on = !laser1_on;
}

void laser2_ISR() {
  laser2_on = !laser2_on;
}

void laser3_ISR() {
  laser3_on = !laser3_on;
}

void piezo0_ISR() {
  piezo0_flicked = !piezo0_flicked;
}

void piezo1_ISR() {
  piezo1_flicked = !piezo1_flicked;
}

void do_not_press_ISR() {
  do_not_pressed = true;
}

void debounce_do_not_press() {
  if((int32_t)(millis()-last_millis) >= debounce_time*1000) {
    last_millis = millis();
    do_not_press_ISR();
  }
}
