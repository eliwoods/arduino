// Super simple, just output a hue and lets see if we can ge the teensy to read it
// in while it outputs to the led strip.

void setup() {
  Serial.begin(9600);
}

void loop() {
  static uint8_t gHue = 0;

  Serial.write(gHue);

  gHue++;
  delay(20);

}
