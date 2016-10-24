void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP); 
}

void loop() {
  Serial.println(analogRead(0));
  delay(100);
}
