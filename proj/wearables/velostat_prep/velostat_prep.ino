#define LEFT_IN A0
#define RIGHT_IN A1
#define HEART_IN A2

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_IN, INPUT_PULLUP); 
  pinMode(RIGHT_IN, INPUT_PULLUP); 
}

void loop() {
  Serial.println(analogRead(LEFT_IN));
  delay(100);
}
