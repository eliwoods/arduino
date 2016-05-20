void setup() {
  Serial.begin(9000);

}

void loop() {
  Serial.println(analogRead(0));
  delay(500);
}
