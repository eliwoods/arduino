void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT_PULLUP); 
}

void loop() {
  if(analogRead(0) == 1023) {
    Serial.print("Thumb On\t");
  }
  else {
    Serial.print("Thumb Off\t");
  }
  Serial.println(analogRead(1));
  delay(100);
}
