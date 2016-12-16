/*
  LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

uint8_t mnt, sec;
uint32_t last_second, curr_second;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Initialize Some Variables
  last_second = 99;
  curr_second = 0;
  mnt = 0;
  sec = 0;
  // Print a message to the LCD.
  for (uint8_t x = 0; x < 10; x++) {
    if(x%2 == 0) {
      lcd.setCursor(0, 0);
    }
    else {
      lcd.setCursor(0,1);
    }
    lcd.print("WELCOME TO HELL!");
    delay(500);
    lcd.clear();
    delay(500);
  }
  lcd.setCursor(0,0);
  lcd.print("DRINK IN:");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  static uint32_t t = 0;
  static 
  static uint32_t curr_second = 0;
  curr_second = millis() / 1000;
  if (curr_second - last_second >= 1) {
    last_second = curr_second;
    t++;
  }
  lcd.print(t, DEC);
}

