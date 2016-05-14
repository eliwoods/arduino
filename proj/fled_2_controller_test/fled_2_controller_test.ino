#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

//#include <FastLED.h>

#define LED0 = 22
#define LED1 = 24

CRGB leds[180];

//const CRGBPalette16 pal =  { CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Red,
//                             CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Red,
//                             CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Red,
//                             CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Red};

void setup() {
  delay(3000);
  FastLED.addLeds<WS2812B, LED0, GRB>(leds, 90);
  FastLED.addLeds<WS2812B, LED1, GRB>(leds+90, 90);
  FastLED.setBrightness(100);
  FastLED.show();
}

void loop() {
  static uint8_t pal_index = 0;
  EVERY_N_MILLISECONDS(100) {
    pal_index++;
  }

//  fill_palette(leds, 180, pal_index, 6, pal, 100, LINEARBLEND);
//  FastLED.show();

}
