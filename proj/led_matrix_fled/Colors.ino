//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
//uint32_t wheel(uint8_t WheelPos) {
//  WheelPos = 255 - WheelPos;
//  if (WheelPos < 85) {
//    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  }
//  if (WheelPos < 170) {
//    WheelPos -= 85;
//    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//  WheelPos -= 170;
//  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//}
//
//// Shift all the colors down one and generate a new color for the first ring
//void update_cols() {
//  for(uint8_t c = maxCrcl - 1; c > 0; c--) {
//    cols[c] = cols[c-1];
//  }
//  cols[0] = wheel(random(256));
//}
