void setup() {
  Serial.begin(9600);

}

void loop() {
  byte nn[] = {255, 13, 255, 22, 255, 30};
  for (byte k = 0; k < 6; k++) {
    Serial.print(nn[k]);
    Serial.print(" ");
  }
  Serial.println();
  byte _tmp, i;
  int8_t j;
  for (i = 1; i < 6; i++) {
    _tmp = nn[i];
    j = i - 1;
    while (j >= 0 && nn[j] > _tmp) {
      nn[j + 1] = nn[j];
      j--;

    }
    Serial.print("TMP - ");
    Serial.println(_tmp);
    nn[j + 1] = _tmp;
    for (byte k = 0; k < 6; k++) {
      Serial.print(nn[k]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("##################");
  delay(5000);

}
