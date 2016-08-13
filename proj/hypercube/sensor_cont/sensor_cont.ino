// Defines for pin names
#define LASER0_IN 0
#define LASER1_IN 1
#define LASER2_IN 2
#define LASER3_IN 3
#define PIEZO0_IN 4
#define PIEZO1_IN 5

#define LASER_POWER 0
#define LASER0_OUT 1
#define LASER1_OUT 2
#define LASER2_OUT 3
#define LASER3_OUT 4
#define PIEZO0_OUT 5
#define PIEZO1_OUT 6
#define LASER_MULT_OUT 7

// Some variables for the various controllers
const uint16_t laser_threshold = 25; //This is totally arbitrary, will need some tuning.
const uint16_t piezo_threshold = 700; // Pretty sure this one is right
boolean triggered[] = {false, false, false, false};
const uint32_t piezo_wait = 15;

void setup() {
  // Set the pin modes for the outputs
  pinMode(LASER_POWER, OUTPUT);
  pinMode(LASER0_OUT, OUTPUT);
  pinMode(LASER1_OUT, OUTPUT);
  pinMode(LASER2_OUT, OUTPUT);
  pinMode(LASER3_OUT, OUTPUT);
  pinMode(PIEZO0_OUT, OUTPUT);
  pinMode(PIEZO1_OUT, OUTPUT);
  pinMode(LASER_MULT_OUT, OUTPUT);
}

void loop() {

  check_lasers();
  check_piezos();

}

// TODO: Add check for multiple lasers being broken, add something to check how long it's been since they were broken
void check_lasers() {
    // Could probably just tie them to the 5V but w/e
    digitalWrite(LASER_POWER, HIGH);
    
    // Check the value of each of the analog inputs. If they are above the threshold, then
    // trigger then send a high signal out. This will be interpereted by the teensy
    // as an interrupt
    for (uint8_t ch = 0; ch < 4; ch++) {
      // Check if we have already triggered this channel, this is incase someone
      // is holding their hand in front of the lasers or something dumb like that.
      if (!triggered[ch]) {
        if (analogRead(ch) > threshold) {
          triggered[ch] = true;
          // So that we can visualize it on the test strip
          if (ch == LASER0_IN) {
            digitalWrite(LASER0_OUT, HIGH);
          }
          else if (ch == LASER1_IN) {
            digitalWrite(LASER1_OUT, HIGH);
          }
          else if (ch == LASER2_IN) {
            digitalWrite(LASER2_OUT, HIGH);
          }
          else if (ch == LASER3_IN) {
            digitalWrite(LASER3_OUT, HIGH);
          }
        }
      }
      else {
        if (analogRead(ch) < threshold) {
          triggered[ch] = false;
        }
      }
    } // End loop over channels
}

void check_piezos() {
  // Use these to make sure we don't get consecutive readings from one flick
  static boolean waiting0 = false;
  static boolean waiting1 = false;

  // For checking the time since the last piezo trigger
  static uint32_t last_millis0 = 99;
  static uint32_t last_millis1 = 99;

  // Check the first piezo and set the flag to start the timer if it is over the threshold
  if(analogRead(PIEZO0_IN) > piezo_threshold && !waiting0) {
    analogWrite(PIEZO0_OUT, HIGH);
    //waiting0 = true;        
    //last_millis0 = millis();
  }
  // This way we only send the interrupt when the threshold is crossed
  analogWrite(PIEZO0_OUT, LOW);

  // Check that
  //if((int32_t)(millis()-last_millis()) >= piezo_wait) {
  //  waiting0 = false;
  //}

  // Check the first piezo and set the flag to start the timer if it is over the threshold
  if(analogRead(PIEZO1_IN) > piezo_threshold && !waiting1) {
    analogWrite(PIEZO1_OUT, HIGH);
    //waiting1 = true;        
    //last_millis1 = millis();
  }
  // This way we only send the interrupt when the threshold is crossed
  analogWrite(PIEZO1_OUT, LOW);

  // Check that
  //if((int32_t)(millis()-last_millis()) >= piezo_wait) {
  //  waiting1 = false;
  //}
}
