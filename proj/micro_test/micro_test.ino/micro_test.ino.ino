#define LEFT_IN A0
#define RIGHT_IN A2


void setup() {
  Serial.begin(9600);

  pinMode(LEFT_IN, INPUT_PULLUP);
  pinMode(RIGHT_IN, INPUT_PULLUP);

}

void loop() {
  Serial.print("LEFT = ");
  Serial.print(analogRead(LEFT_IN));
  Serial.print("\tRIGHT = ");
  Serial.println(analogRead(RIGHT_IN));
  delay(100);
}
