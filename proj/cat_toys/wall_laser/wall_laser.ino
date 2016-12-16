// Check shape memory servo miga motors

#include <Servo.h>

// Pins
#define PHI 10
#define THETA 9

// Ranges
#define DEL_MIN 1000
#define DEL_MAX 2500
#define THETA_MIN 45
#define THETA_MAX 90
#define PHI_MIN 0
#define PHI_MAX 70

// Defined in physics spherical coordinates
Servo theta_serv, phi_serv;

// Servo positions
uint8_t theta = 0;    // variable to store the servo position
uint8_t phi = 0;

// Randomly chosen delay
//uint16_t theta_delay = 0;
//uint16_t phi_delay = 0;
uint16_t del;

void setup() {
  // Servo setup
  theta_serv.attach(THETA);
  phi_serv.attach(PHI);

  // Random Number Setup
  randomSeed(analogRead(0));
}

void loop() {
  // Pick new position
  theta = random(THETA_MIN, THETA_MAX);
  phi = random(PHI_MIN, PHI_MAX);

  // Write position to servo
  theta_serv.write(theta);
  phi_serv.write(phi);

  // Wait
  del = random(DEL_MIN,DEL_MAX);
  delay(del);
  
}

