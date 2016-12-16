void power_ISR() {
  if (power_on) {
    power_on = false;
  }
  else {
    power_on = true;
  }
}

void debounce_power() {
  if ((int32_t)(micros() - last_micros) >= debounce_time*1000) {
    power_ISR();
    last_micros = micros();
  }
}

