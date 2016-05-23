void setup() {
  Serial.begin(9000);

}

void loop() {
  Serial.println("RATE: ");
  Serial.println(analogRead(0));
  Serial.println("HUE: ");
  Serial.println(analogRead(1));
  Serial.println("VAL: ");
  Serial.println(analogRead(2));
  Serial.println("STROBE: ");
  Serial.println(analogRead(3));

  delay(400);

}
