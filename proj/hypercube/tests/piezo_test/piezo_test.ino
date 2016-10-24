//#include <FastLED.h>

#define NUMLED 30
#define LEDIN 6

//CRGBArray<NUMLED> leds;
uint8_t gHue;

void setup() {
  Serial.begin(9600);
 
//  FastLED.addLeds<WS2812B, LEDIN, GRB>(leds, NUMLED);
//  FastLED.setBrightness(100);
//  FastLED.show(); 

}

void loop() {
//  gHue = map(analogRead(0 ), 0, 1023, 0, 255);

//  fill_solid( leds, NUMLED, CHSV(gHue, 255, 100));
//  FastLED.show();

    Serial.print(analogRead(4));
    Serial.print("\t");
    Serial.println(analogRead(5));

}
