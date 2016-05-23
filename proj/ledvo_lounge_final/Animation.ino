
/////////////////////////////////////////////////////////////////////////////////////////
// These are the high level animations. Basically they use the animation base function //
// and some clever rearranging to get some particularly snazzy animations.             //
/////////////////////////////////////////////////////////////////////////////////////////


// Just do theater chase animations on only inner and outer perimaters and run
// them in opposite directions
void theater_perim_opp(uint8_t theat_opt) {
  // Variable that we increment to get the actual animations
  static uint8_t pal_index;

  // Increment the palette index at a user controlled rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  // Now fill the two hexagons. Options are as follows:
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Mirror
  // theat_opt == 2 : Theater Chase Mirror 2
  // theat_opt == 3 : Theater Chase Tri
  // theat opt == 4 : Theater Chase Tri Mirror
  // theat_opt == 5 : Theater Chase Tri Mirror 2
  // theat_opt == 6 : Theater Mod
  // theat_opt == 7 : Theater Mod Mirror
  // theat_opt == 8 : Theater Mod Mirror 2
//  if (theat_opt == 0) {
    theater_chase(ih_leds, ih_LED_total, pal_index);
    theater_chase(oh_leds, oh_LED_total, 255 - pal_index);
//  }
//  else if (theat_opt == 1) {
//    theater_chase(ih_leds(0, 3 * led_strand - 1), ih_num_strand / 2, pal_index);
//    ih_leds(ih_num_strand - 1, 3 * led_strand) = ih_leds(0, 3 * led_strand - 1);
//    theater_chase(oh_leds(0, 6 * led_strand - 1), oh_num_strand / 2, 255 - pal_index);
//    oh_leds(oh_num_strand - 1, 6 * led_strand) = oh_leds(0, 6 * led_strand - 1);
//  }
//  else if (theat_opt == 2) {
//    theater_chase(ih_leds(0, 2 * led_strand - 1), 2 * led_strand, pal_index);
//    ih_leds(4 * led_strand - 1, 2 * led_strand) = ih_leds(0, 2 * led_strand - 1);
//    ih_leds(4 * led_strand, ih_LED_total - 1) = ih_leds(0, 2 * led_strand - 1);
//    theater_chase(oh_leds(0, 4 * led_strand - 1), 4 * led_strand, pal_index);
//    oh_leds(8 * led_strand - 1, 4 * led_strand) = oh_leds(0, 4 * led_strand - 1);
//    oh_leds(8 * led_strand, oh_LED_total - 1) = oh_leds(0, 4 * led_strand - 1);
//  }
//  else if (theat_opt == 3) {
//    theater_chase_tri(ih_leds, ih_LED_total, pal_index);
//    theater_chase_tri(oh_leds, oh_LED_total, 255 - pal_index);
//  }
//  else if (theat_opt == 4) {
//    theater_chase_tri(ih_leds(0, 3 * led_strand - 1), ih_num_strand / 2, pal_index);
//    ih_leds(ih_num_strand - 1, 3 * led_strand) = ih_leds(0, 3 * led_strand - 1);
//    theater_chase_tri(oh_leds(0, 6 * led_strand - 1), oh_num_strand / 2, 255 - pal_index);
//    oh_leds(oh_num_strand - 1, 6 * led_strand) = oh_leds(0, 6 * led_strand - 1);
//  }
//  else if (theat_opt == 5) {
//    theater_chase_tri(ih_leds(0, 2 * led_strand - 1), 2 * led_strand, pal_index);
//    ih_leds(4 * led_strand - 1, 2 * led_strand) = ih_leds(0, 2 * led_strand - 1);
//    ih_leds(4 * led_strand, ih_LED_total - 1) = ih_leds(0, 2 * led_strand - 1);
//    theater_chase_tri(oh_leds(0, 4 * led_strand - 1), 4 * led_strand, pal_index);
//    oh_leds(8 * led_strand - 1, 4 * led_strand) = oh_leds(0, 4 * led_strand - 1);
//    oh_leds(8 * led_strand, oh_LED_total - 1) = oh_leds(0, 4 * led_strand - 1);
//  } else if (theat_opt == 6) {
//    theater_chase_mod(ih_leds, ih_LED_total, pal_index);
//    theater_chase_mod(oh_leds, oh_LED_total, 255 - pal_index);
//  }
//  else if (theat_opt == 7) {
//    theater_chase_mod(ih_leds(0, 3 * led_strand - 1), ih_num_strand / 2, pal_index);
//    ih_leds(ih_num_strand - 1, 3 * led_strand) = ih_leds(0, 3 * led_strand - 1);
//    theater_chase_mod(oh_leds(0, 6 * led_strand - 1), oh_num_strand / 2, 255 - pal_index);
//    oh_leds(oh_num_strand - 1, 6 * led_strand) = oh_leds(0, 6 * led_strand - 1);
//  }
//  else if (theat_opt == 8) {
//    theater_chase_mod(ih_leds(0, 2 * led_strand - 1), 2 * led_strand, pal_index);
//    ih_leds(4 * led_strand - 1, 2 * led_strand) = ih_leds(0, 2 * led_strand - 1);
//    ih_leds(4 * led_strand, ih_LED_total - 1) = ih_leds(0, 2 * led_strand - 1);
//    theater_chase_mod(oh_leds(0, 4 * led_strand - 1), 4 * led_strand, pal_index);
//    oh_leds(8 * led_strand - 1, 4 * led_strand) = oh_leds(0, 4 * led_strand - 1);
//    oh_leds(8 * led_strand, oh_LED_total - 1) = oh_leds(0, 4 * led_strand - 1);
//  }
  FastLED.show();

}

// Run a fake eq on the diagonals from the top and bottom. Do 3 separate
// "channels", one for each diagonal
void whole_eq_3() {
  static uint8_t pal_index[] = {0, 0, 0};
  static uint16_t lead_max[] = {d_LED_num / 2, d_LED_num / 2, d_LED_num / 2};
  static uint16_t lead[] = {0, 0, 0};

  // Fill the bars at a input dependent rate. Lets try this with a triangular wave at first
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 200));
    for (uint8_t dd = 0; dd < 3; dd++) {
      lead[dd] = (lead[dd] + 1) % (lead_max[dd] * 2);
    }
  }

  // Check if the diagonals bars are about to be empty. Increment the palette index and
  // generate a new max length if so
  for (uint8_t dd = 0; dd < 3; dd++) {
    if (lead[dd] == 0) {
      pal_index[dd] += 12;
      lead_max[dd] = random16(d_LED_num / 3, d_LED_num); // from 1/3 length -> length
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill.
  // Draw this to the led template and then assign to the appropriate strands before going onto the next
  // diagonal
  for (uint8_t dd = 0; dd < 3; dd++) {
    if (lead[dd] < lead_max[dd]) {
      fill_solid(led_tmplt, lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    else {
      fill_solid(led_tmplt, 2 * lead_max[dd] - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    // First set of diagonals
    if (dd == 0) {
      // Diagonal
      d_leds[0](0, led_strand - 1) = led_tmplt;
      d_leds[0](led_strand, d_LED_num - 1) = led_tmplt(led_strand - 1, 0);
      // Inner Hex
      ih_leds(0, led_strand - 1) = led_tmplt(led_strand - 1, 0);
      ih_leds(led_strand, 2 * led_strand - 1) = led_tmplt;
      // Outer Hex
      oh_leds(led_strand, 2 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
      oh_leds(2 * led_strand, 3 * led_strand - 1) = led_tmplt;
    }
    if (dd == 1) {
      // Diagonal
      d_leds[1](0, led_strand - 1) = led_tmplt;
      d_leds[1](led_strand, d_LED_num - 1) = led_tmplt(led_strand - 1, 0);
      // Inner Hex
      ih_leds(2 * led_strand, 3 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
      ih_leds(3 * led_strand, 4 * led_strand - 1) = led_tmplt;
      // Outer Hex
      oh_leds(5 * led_strand, 6 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
      oh_leds(6 * led_strand, 7 * led_strand - 1) = led_tmplt;
    }
    if (dd == 2) {
      // Diagonal
      d_leds[2](0, led_strand - 1) = led_tmplt;
      d_leds[2](led_strand, d_LED_num - 1) = led_tmplt(led_strand - 1, 0);
      // Inner Hex
      ih_leds(4 * led_strand, 5 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
      ih_leds(5 * led_strand, 6 * led_strand - 1) = led_tmplt;
      // Outer Hex
      oh_leds(9 * led_strand, 10 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
      oh_leds(10 * led_strand, 11 * led_strand - 1) = led_tmplt;
    }
    
  }

  FastLED.show();

  fadeToBlackBy(led_tmplt, led_strand, 2);

}

// Similar to whole_eq_3, except we have just one channel so all the panels
// are doing the same thing
void whole_eq() {
  static uint8_t pal_index = 0;
  static uint16_t lead_max = d_LED_num / 2;
  static uint16_t lead = 0;

  // Fill the bars at a input dependent rate. Lets try this with a triangular wave at first
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
    lead = (lead + 1) % (lead_max * 2);
    pal_index++;
  }

  // Check if the diagonals bars are about to be empty. Increment the palette index and
  // generate a new max length if so
  for (uint8_t dd = 0; dd < 3; dd++) {
    if (lead == 0) {
      lead_max = random16(led_strand / 3, led_strand ); // from 1/3 length -> length
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill.
  // Draw this to the led template and then assign that one template to all the appropriate strands
  if (lead < lead_max) {
    fill_solid(led_tmplt, lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else {
    fill_solid(led_tmplt, 2 * lead_max - lead, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  // First set of diagonals
  for (uint8_t dd = 0; dd < 3; dd++) {
    d_leds[dd](0, led_strand - 1) = led_tmplt;
    d_leds[dd](led_strand, d_LED_num - 1) = led_tmplt(led_strand - 1, 0);
  }
  // Inner Hex
  ih_leds(0, led_strand - 1) = led_tmplt(led_strand - 1, 0);
  ih_leds(led_strand, 2 * led_strand - 1) = led_tmplt;
  ih_leds(2 * led_strand, 3 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
  ih_leds(3 * led_strand, 4 * led_strand - 1) = led_tmplt;
  ih_leds(4 * led_strand, 5 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
  ih_leds(5 * led_strand, 6 * led_strand - 1) = led_tmplt;
  // Outer Hex
  oh_leds(led_strand, 2 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
  oh_leds(2 * led_strand, 3 * led_strand - 1) = led_tmplt;
  oh_leds(5 * led_strand, 6 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
  oh_leds(6 * led_strand, 7 * led_strand - 1) = led_tmplt;
  oh_leds(9 * led_strand, 10 * led_strand - 1) = led_tmplt(led_strand - 1, 0);
  oh_leds(10 * led_strand, 11 * led_strand - 1) = led_tmplt;

  fadeToBlackBy(led_tmplt, led_strand, 2);

  FastLED.show();

}

// Flash the whole diagonal strips in various orders depending on the elapsed time. First
// flash from 0 -> 3, then 0 -> 3 -> 0, then random strips
void diagonal_flash_timed() {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;
  static uint8_t anim_set = 0;
  static uint8_t current_strand = 0;
  static uint8_t next_strand = 0;
  static boolean just_switched = true;
  static boolean flagged = false;

  // First, keep track of how long we've been running this animation
  EVERY_N_SECONDS(10) {
    anim_set = (anim_set + 1) % 3;
    // Reset some shit every time we switch animations
    for (uint8_t dd = 0; dd < 3; dd++) {
      fill_solid(d_leds[dd], d_LED_num, CRGB::Black);
      fill_solid(ih_leds, ih_LED_total, CRGB::Black);
      fill_solid(oh_leds, oh_LED_total, CRGB::Black);
    }
    brightness = 0;
    current_strand = 0;
    just_switched = true;
    flagged = false;
  }

  // We have to use the just_switched flag in order keep the code from running over certain
  // conditions until they have actually changed.
  // Flash 0 -> 3
  if (anim_set == 0) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        current_strand = (current_strand + 1) % 3;
        if (current_strand == 0) {
          pal_index += 12;
        }
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Flash strands 0 -> 3 -> 0
  if (anim_set == 1) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
      brightness += 16;
      if (just_switched) {
        just_switched = false;
      }
    }

    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        if (current_strand == 0) {
          flagged = false;
          pal_index += 12;
        }
        if (current_strand == 2) {
          flagged = true;
        }
        if (flagged) {
          current_strand--;
        }
        else {
          current_strand++;
        }
        just_switched = true;
      }
    }

    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Flash random strands
  if (anim_set == 2) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        next_strand = random8(3);
        while (next_strand == current_strand) {
          next_strand = random8(3);
        }
        current_strand = next_strand;
        pal_index += 12;
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }

}

// The symmetrical version of the saw structure. Has mirroring at 1 point on the half we animate,
// so there is a 2 fold symmetry all together. We'll run some theater chase animations on it in the usual
// manner. 0 and 1 are the two different orientations. Didn't want them on a timer like the sub patterns,
// so I'm making them two explicit animations
void saw_chaser_0(uint8_t theat_opt) {
  // Some variables for animating the chasers
  static uint8_t pal_index = 0;

  // Control the chasers at a user input rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  // Now fill the two hexagons. Options are as follows:
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Mirror
  // theat_opt == 2 : Theater Chase Tri
  // theat opt == 3 : Theater Chase Tri Mirror
  // theat_opt == 4 : Theater Mod
  // theat_opt == 5 : Theater Mod Mirror
  if (theat_opt == 0) {
    theater_chase(led_tmplt, led_strand, pal_index);
    theater_chase(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(0, led_strand - 1) = led_tmplt;
    ih_leds(ih_LED_total - led_strand, ih_LED_total - 1) = led_tmplt;
    // Outer Hex
    theater_chase(oh_leds(2 * led_strand, 10 * led_strand - 1), 8 * led_strand, pal_index);
    //Diagonals
    d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
    d_leds[2] = led_tmplt_2;
  }
  else if (theat_opt == 1) {
    theater_chase(led_tmplt, led_strand, pal_index);
    theater_chase(led_tmplt_2, 2 * led_strand, pal_index);
    theater_chase(led_tmplt_4, 4 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(0, led_strand - 1) = led_tmplt;
    ih_leds(ih_LED_total - 1, ih_LED_total - led_strand) = led_tmplt;
    // Outer Hex
    oh_leds(2 * led_strand, 6 * led_strand - 1) = led_tmplt_4;
    oh_leds(10 * led_strand - 1, 6 * led_strand) = led_tmplt_4;
    //Diagonals
    d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
    d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  }
  else if (theat_opt == 2) {
    theater_chase_tri(led_tmplt, led_strand, pal_index);
    theater_chase_tri(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(0, led_strand - 1) = led_tmplt;
    ih_leds(ih_LED_total - led_strand, ih_LED_total - 1) = led_tmplt;
    // Outer Hex
    theater_chase_tri(oh_leds(2 * led_strand, 10 * led_strand - 1), 8 * led_strand, pal_index);
    //Diagonals
    d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
    d_leds[2] = led_tmplt_2;
  }
  else if (theat_opt == 3) {
    theater_chase_tri(led_tmplt, led_strand, pal_index);
    theater_chase_tri(led_tmplt_2, 2 * led_strand, pal_index);
    theater_chase_tri(led_tmplt_4, 4 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(0, led_strand - 1) = led_tmplt;
    ih_leds(ih_LED_total - 1, ih_LED_total - led_strand) = led_tmplt;
    // Outer Hex
    oh_leds(2 * led_strand, 6 * led_strand - 1) = led_tmplt_4;
    oh_leds(10 * led_strand - 1, 6 * led_strand) = led_tmplt_4;
    //Diagonals
    d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
    d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  }
  else if (theat_opt == 4) {
    theater_chase_mod(led_tmplt, led_strand, pal_index);
    theater_chase_mod(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(0, led_strand - 1) = led_tmplt;
    ih_leds(ih_LED_total - led_strand, ih_LED_total - 1) = led_tmplt;
    // Outer Hex
    theater_chase_mod(oh_leds(2 * led_strand, 10 * led_strand - 1), 8 * led_strand, pal_index);
    //Diagonals
    d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
    d_leds[2] = led_tmplt_2;
  }
  else if (theat_opt == 5) {
    theater_chase_mod(led_tmplt, led_strand, pal_index);
    theater_chase_mod(led_tmplt_2, 2 * led_strand, pal_index);
    theater_chase_mod(led_tmplt_4, 4 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(0, led_strand - 1) = led_tmplt;
    ih_leds(ih_LED_total - 1, ih_LED_total - led_strand) = led_tmplt;
    // Outer Hex
    oh_leds(2 * led_strand, 6 * led_strand - 1) = led_tmplt_4;
    oh_leds(10 * led_strand - 1, 6 * led_strand) = led_tmplt_4;
    //Diagonals
    d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
    d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  }

  FastLED.show();

}

void saw_chaser_1(uint8_t theat_opt) {
  // Some variables for animating the chasers
  static uint8_t pal_index = 0;

  // Control the chasers at a user input rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 100));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  // Now fill the two hexagons. Options are as follows:
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Mirror
  // theat_opt == 2 : Theater Chase Tri
  // theat opt == 3 : Theater Chase Tri Mirror
  // theat_opt == 4 : Theater Mod
  // theat_opt == 5 : Theater Mod Mirror
  if (theat_opt == 0) {
    theater_chase(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    theater_chase(ih_leds(led_strand, 5 * led_strand - 1), 4 * led_strand, pal_index);
    // Outer Hex
    oh_leds(0, 2 * led_strand - 1) = led_tmplt_2;
    oh_leds(10 * led_strand, 12 * led_strand - 1) = led_tmplt_2;
    //Diagonals
    d_leds[0] = led_tmplt_2;
    d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  }
  else if (theat_opt == 1) {
    theater_chase(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(led_strand, 3 * led_strand - 1) = led_tmplt_2;
    ih_leds(5 * led_strand - 1, 3 * led_strand) = led_tmplt_2;
    // Outer Hex
    oh_leds(0, 2 * led_strand - 1) = led_tmplt_2;
    oh_leds(12 * led_strand - 1, 10 * led_strand) = led_tmplt_2;
    //Diagonals
    d_leds[0] = led_tmplt_2;
    d_leds[2] = led_tmplt_2;
  }
  else if (theat_opt == 2) {
    theater_chase_tri(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    theater_chase_tri(ih_leds(led_strand, 5 * led_strand - 1), 4 * led_strand, pal_index);
    // Outer Hex
    oh_leds(0, 2 * led_strand - 1) = led_tmplt_2;
    oh_leds(10 * led_strand, 12 * led_strand - 1) = led_tmplt_2;
    //Diagonals
    d_leds[0] = led_tmplt_2;
    d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  }
  else if (theat_opt == 3) {
    theater_chase_tri(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(led_strand, 3 * led_strand - 1) = led_tmplt_2;
    ih_leds(5 * led_strand - 1, 3 * led_strand) = led_tmplt_2;
    // Outer Hex
    oh_leds(0, 2 * led_strand - 1) = led_tmplt_2;
    oh_leds(12 * led_strand - 1, 10 * led_strand) = led_tmplt_2;
    //Diagonals
    d_leds[0] = led_tmplt_2;
    d_leds[2] = led_tmplt_2;
  }
  else if (theat_opt == 4) {
    theater_chase_mod(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    theater_chase_mod(ih_leds(led_strand, 5 * led_strand - 1), 4 * led_strand, pal_index);
    // Outer Hex
    oh_leds(0, 2 * led_strand - 1) = led_tmplt_2;
    oh_leds(10 * led_strand, 12 * led_strand - 1) = led_tmplt_2;
    //Diagonals
    d_leds[0] = led_tmplt_2;
    d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  }
  else if (theat_opt == 5) {
    theater_chase_mod(led_tmplt_2, 2 * led_strand, pal_index);
    // Inner Hexagon
    ih_leds(led_strand, 3 * led_strand - 1) = led_tmplt_2;
    ih_leds(5 * led_strand - 1, 3 * led_strand) = led_tmplt_2;
    // Outer Hex
    oh_leds(0, 2 * led_strand - 1) = led_tmplt_2;
    oh_leds(12 * led_strand - 1, 10 * led_strand) = led_tmplt_2;
    //Diagonals
    d_leds[0] = led_tmplt_2;
    d_leds[2] = led_tmplt_2;
  }

  FastLED.show();

}

// The same shape as the chaser saw, except we illuminate everything in one color
// that transitions through the template. Again, splitting it up into two different animations
// for a little more control
void saw_solid_0() {
  // Variable for transitioning between colors
  static uint8_t pal_index = 0;

  // Control palette index at a user input rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  // Fill the inner hex sections
  fill_solid(ih_leds(0, led_strand - 1), led_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  fill_solid(ih_leds(5 * led_strand, 6 * led_strand - 1), led_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  // Diagonals
  fill_solid(d_leds[0], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  fill_solid(d_leds[2], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  // Outer Hex
  fill_solid(oh_leds(2 * led_strand, 10 * led_strand - 1), 8 * led_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));

  FastLED.show();
}

// Second configuration to solid saw
void saw_solid_1() {
  // Variable for transitioning between colors
  static uint8_t pal_index = 0;

  // Control palette index at a user input rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  // Fill the inner hex sections
  fill_solid(ih_leds(led_strand, 5 * led_strand - 1), led_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  // Diagonals
  fill_solid(d_leds[0], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  fill_solid(d_leds[2], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  // Outer Hex
  fill_solid(oh_leds(0, 2 * led_strand - 1), 2 * led_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  fill_solid(oh_leds(10 * led_strand, 12 * led_strand - 1), 2 * led_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));

  FastLED.show();
}

// Similar to diagonal flashes, except with trapezoids. Basically just recycling the same iterations but drawing different shapes
void trap_solid() {
  static uint8_t brightness = 0;
  static uint8_t pal_index = 0;
  static uint8_t anim_set = 0;
  static uint8_t current_trap = 0;
  static uint8_t next_trap = 0;
  static boolean just_switched = true;
  static boolean flagged = false;

  // First, keep track of how long we've been running this animation
  EVERY_N_SECONDS(10) {
    anim_set = (anim_set + 1) % 4;
    // Reset some shit every time we switch animations
    reset_all();
    brightness = 0;
    current_trap = 0;
    just_switched = true;
    flagged = false;
  }

  // We have to use the just_switched flag in order keep the code from running over certain
  // conditions until they have actually changed.
  // Flash 0 -> 4
  if (anim_set == 0) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        reset_all();
        FastLED.show();
        current_trap = (current_trap + 1) % 4;
        if (current_trap == 0) {
          pal_index += 12;
        }
        just_switched = true;
      }
    }
    draw_trapezoid(current_trap, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Flash strands 0 -> 4 -> 0
  if (anim_set == 1) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 100));
      brightness += 16;
      if (just_switched) {
        just_switched = false;
      }
    }

    if (!just_switched) {
      if (brightness == 0) {
        reset_all();
        FastLED.show();
        if (current_trap == 0) {
          flagged = false;
          pal_index += 12;
        }
        if (current_trap == 3) {
          flagged = true;
        }
        if (flagged) {
          current_trap--;
        }
        else {
          current_trap++;
        }
        just_switched = true;
      }
    }
    draw_trapezoid(current_trap, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Turn 0->4 on, then turn 0->4 off
  if (anim_set == 2) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 100));
      if (!flagged) {
        brightness += 16;
      }
      else {
        brightness -= 16;
      }
      if (just_switched) {
        just_switched = false;
      }
    }

    if (!just_switched && !flagged && brightness == 0) {
      
      if ( current_trap == 3) {
        flagged = true;
        brightness = 255;
      }
      current_trap = (current_trap + 1) % 4;
      just_switched = true;
    }
    if (!just_switched && flagged && brightness == 15) {
      if (current_trap == 3) {
        flagged = false;
        brightness = 0;
      }
      else {
        brightness = 255;
      }
      draw_trapezoid(current_trap, CRGB::Black);
      FastLED.show();
      current_trap = (current_trap + 1) % 4;
      // Iterate next color
      if (current_trap == 0) {
        pal_index += 12;
      }
      just_switched = true;
    }
    draw_trapezoid(current_trap, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Flash random traps
  if (anim_set == 3) {
    EVERY_N_MILLISECONDS_I(thisTimer, 10) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 100));
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        reset_all();
        FastLED.show();
        next_trap = random8(4);
        while (next_trap == current_trap) {
          next_trap = random8(4);
        }
        current_trap = next_trap;
        pal_index += 12;
        just_switched = true;
      }
    }
    draw_trapezoid(current_trap, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
}

// A whole structure chaser pattern. Sort of follows the trapezoids, kind of doesn't at the same time. It's basically
// the symmetrical equivalent
void all_trap_chaser(uint8_t theat_opt) {
  // Used to iterate on the sub patterns
  static uint8_t pal_index = 0;

  // Control the iteration at a user input rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1023, 1, 300));
    if (anim_reverse) {
      pal_index--;
    }
    else {
      pal_index++;
    }
  }

  // theat_opt == 0 : theater chase
  // theat_opt == 1 : theater chase tri
  // theat_opt == 2 : thater mod
  if (theat_opt == 0) {
    theater_chase(led_tmplt, led_strand, pal_index);
    theater_chase(led_tmplt_2, 2 * led_strand, pal_index);
    theater_chase(led_tmplt_4, 4 * led_strand, pal_index);
  }
  else if (theat_opt == 1) {
    theater_chase_tri(led_tmplt, led_strand, pal_index);
    theater_chase_tri(led_tmplt_2, 2 * led_strand, pal_index);
    theater_chase_tri(led_tmplt_4, 4 * led_strand, pal_index);
  }
  else if (theat_opt == 2) {
    theater_chase_mod(led_tmplt, led_strand, pal_index);
    theater_chase_mod(led_tmplt_2, 2 * led_strand, pal_index);
    theater_chase_mod(led_tmplt_4, 4 * led_strand, pal_index);
  }

  // Fill the strands with the appropriate templates
  // Inner Hex
  ih_leds(0, led_strand - 1) = led_tmplt;
  ih_leds(ih_LED_total - 1, ih_LED_total - led_strand) = led_tmplt;
  ih_leds(3 * led_strand - 1, led_strand) = led_tmplt_2;
  ih_leds(3 * led_strand, 5 * led_strand - 1) = led_tmplt_2;
  // Diagonals
  d_leds[0](d_LED_num - 1, 0) = led_tmplt_2;
  d_leds[1] = led_tmplt_2;
  d_leds[2](d_LED_num - 1, 0) = led_tmplt_2;
  // Outer Hex
  oh_leds(2 * led_strand - 1, 0) = led_tmplt_2;
  oh_leds(2 * led_strand, 6 * led_strand - 1) = led_tmplt_4;
  oh_leds(10 * led_strand - 1, 6 * led_strand) = led_tmplt_4;
  oh_leds(10 * led_strand, oh_LED_total - 1) = led_tmplt_2;

  FastLED.show();
}
