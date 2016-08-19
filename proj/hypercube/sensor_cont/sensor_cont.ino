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
const uint16_t laser_threshold = 25; // Should still be generally correct
const uint16_t piezo_threshold = 700; // Pretty sure this one is right
const uint16_t piezo_tol = 200;
boolean triggered[] = {false, false, false, false};

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

// Check which lasers are being broken and send out an interrupt signal from the right port. Also check that if 
// a laser has been broken that we don't repeatedly send out an interrupt since the teensy will be looking for
// the rising edge of a pulse.
void check_lasers() {
    // Could probably just tie them to the 5V but w/e
    digitalWrite(LASER_POWER, HIGH);
    
    // Check the value of each of the analog inputs. If they are above the threshold, then
    // trigger then send a high signal out. This will be interpereted by the teensy
    // as an interrupt
    for (uint8_t ch = 0; ch < 4; ch++) {
      // Check if we have already triggered this channel, this is incase someone
      // is holding their hand in front of the lasers or something dumb like that.
      //if (!triggered[ch]) {
        if (analogRead(ch) > laser_threshold) {
          //triggered[ch] = true;
          // Write HIGH to the correct output
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
      //}
      //else {
        if (analogRead(ch) < laser_threshold) {
          triggered[ch] = false;
          if (ch == LASER0_IN) {
            digitalWrite(LASER0_OUT, LOW);
          }
          else if (ch == LASER1_IN) {
            digitalWrite(LASER1_OUT, LOW);
          }
          else if (ch == LASER2_IN) {
            digitalWrite(LASER2_OUT, LOW);
          }
          else if (ch == LASER3_IN) {
            digitalWrite(LASER3_OUT, LOW);
          }
        }
      //}
    } // End loop over channels
}

void check_piezos() {
  // Check the first piezo and set the flag to start the timer if it is over the threshold
  if(analogRead(PIEZO0_IN) > piezo_threshold + piezo_tol || analogRead(PIEZO0_IN) < piezo_threshold - piezo_tol) {
    analogWrite(PIEZO0_OUT, HIGH);
  }
  else {
    analogWrite(PIEZO0_OUT, LOW);
  }

  // Check the first piezo and set the flag to start the timer if it is over the threshold
  if(analogRead(PIEZO1_IN) > piezo_threshold + piezo_tol || analogRead(PIEZO1_IN) < piezo_threshold - piezo_tol) {
    analogWrite(PIEZO1_OUT, HIGH);
  }
  else {
    analogWrite(PIEZO1_OUT, LOW);
  }

}
