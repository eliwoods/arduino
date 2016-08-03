// Super simple, just output a hue and lets see if we can ge the teensy to read it
// in while it outputs to the led strip.

void setup() {
  Serial.begin(11500);

}

void loop() {
  static uint8_t gHue = 0;

  Serial.println(gHue);

  gHue++;
  delay(50);

}
