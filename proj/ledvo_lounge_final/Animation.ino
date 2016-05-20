
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
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 1, 100));
    pal_index++;
  }

  // Now fill the two hexagons. Options are as follows:
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Mirror
  // theat_opt == 2 : Theater Chase Mirror 2
  // theat_opt == 2 : Theater Chase Tri
  // theat opt == 3 : Theater Chase Tri Mirror
  // theat_opt == 4 : Theater Mod
  // theat_opt == 5 : Theater Mod Mirror
  if (theat_opt == 0) {
    theater_chase(ih_leds, ih_LED_total, pal_index);
    theater_chase(oh_leds, oh_LED_total, 255 - pal_index);
  }
//  else if (theat_opt == 1) {
//    theater_chase_mir(ih_leds, ih_num_strand, pal_index);
//    theater_chase_mir(oh_leds, oh_num_strand, 255-pal_index);
//  }
  else if (theat_opt == 2) {
    theater_chase_tri(ih_leds, ih_LED_total, pal_index);
    theater_chase_tri(oh_leds, oh_LED_total, 255 - pal_index);
  }
//  else if (theat_opt == 3) {
//    theater_chase_tri_mir(ih_leds, ih_num_strand, pal_index);
//    theater_chase_tri_mir(oh_leds, oh_num_strand, 255 - pal_index);
//  }
  else if (theat_opt == 4) {
    theater_chase_mod(ih_leds, ih_LED_total, pal_index);
    theater_chase_mod(oh_leds, oh_LED_total, 255 - pal_index);
  }
//  else if (theat_opt == 5) {
//    theater_chase_mod_mir(ih_leds, ih_num_strand, pal_index);
//    theater_chase_mod_mir(oh_leds, oh_num_strand, 255 - pal_index);
//  }
  FastLED.show();

}

/*
/////////////////////////
// DIAGONAL ANIMATIONS //
/////////////////////////

void whole_eq() {
  static uint8_t pal_index[] = {0, 0, 0, 0, 0, 0};
  static uint16_t lead_max[] = {d_LED_num / 2, d_LED_num / 2,
                                d_LED_num / 2, d_LED_num / 2,
                                d_LED_num / 2, d_LED_num / 2
                               };
  static uint16_t lead[] = {0, 0, 0, 0, 0, 0};

  // Fill the bars at a input dependent rate. Lets try this with a triangular wave at first
  EVERY_N_MILLISECONDS(50) {
    //thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 10, 200));
    for (uint8_t dd = 0; dd < 6; dd++) {
      lead[dd] = (lead[dd] + 1) % (lead_max[dd] * 2);
    }
  }

  // Check if the diagonals bars are about to be empty. Increment the palette index and
  // generate a new max length if so
  for (uint8_t dd = 0; dd < 6; dd++) {
    if (lead[dd] == 0) {
      pal_index[dd] += 16;
      lead_max[dd] = random16(d_LED_num / 3, d_LED_num); // from 1/3 length -> length
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill
  for (uint8_t dd = 0; dd < 6; dd++) {
    if (lead[dd] < lead_max[dd]) {
      fill_solid(d_leds[dd], lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    else {
      fill_solid(d_leds[dd], 2 * lead_max[dd] - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
  }
  FastLED.show();
  for (uint8_t dd = 0; dd < 6; dd++ ) {
    fadeToBlackBy(d_leds[dd], d_LED_num, 20);
  }

}

void diagonal_tri() {
  static uint8_t pal_index[] = {0, 0, 0, 0, 0, 0};
  // Since the offsets are constant let's just set them here
  static uint8_t lead[] = {0, 4, 8, 12, 16, 20};
  static boolean first_pass = true;

  // Fill the bars at a input dependent rate. This basically runs a triangular
  // wave on each strip, but we offset them so you can see the whole waveform
  EVERY_N_MILLISECONDS(60) {
    //thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 1253, 10, 200));
    for (uint8_t dd = 0; dd < 6; dd++) {
      lead[dd] = (lead[dd] + 1) % (2 * d_LED_num);
    }
    if (first_pass) {
      first_pass = false;
    }
  }

  // Check if the first strand is about to be empty and increment the color index. The
  // first if statement is to make sure the first strip doesn't get a color rotation ahead
  if (!first_pass) {
    for (uint8_t dd = 0; dd < 6; dd++) {
      if ( lead[dd] == 0 ) {
        pal_index[dd] += 16;
      }
    }
  }

  // Now actually draw the bars. Check which way we are going on the wave and either empty or fill
  for (uint8_t dd = 0; dd < 6; dd++) {
    // Going up
    if (lead[dd] < d_LED_num) {
      fill_solid(d_leds[dd], lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
    // Going down
    else {
      fill_solid(d_leds[dd], 2 * d_LED_num - lead[dd], ColorFromPalette(gPalette, pal_index[dd], gBrightness, gBlending));
    }
  }

  FastLED.show();

  for (uint8_t dd = 0; dd < 6; dd++ ) {
    fadeToBlackBy(d_leds[dd], d_LED_num, 20);
  }
}

// THESE ARE THE ACTUAL FULL BODY ANIMATIONS. THESE WILL MAKE THE SLICES AND

// Flash the whole diagonal strips in various orders depending on the elapsed time. First
// flash from 0 -> 6, then flash opposite sides, then 0 -> 6 -> 0, then ranndom strips
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
    anim_set = (anim_set + 1) % 4;
    // Reset some shit every time we switch animations
    for (uint8_t dd = 0; dd < 6; dd++) {
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
  if (anim_set == 0) {
    EVERY_N_MILLISECONDS(10) {
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
        current_strand = (current_strand + 1) % 6;
        if (current_strand == 0) {
          pal_index += 16;
        }
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Flash opposite sides 0 -> 3 (yay symmetry!!!)
  if (anim_set == 1) {
    EVERY_N_MILLISECONDS(10) {
      brightness += 16;
      if ( just_switched) {
        just_switched = false;
      }
    }
    if (!just_switched) {
      if (brightness == 0) {
        fill_solid(d_leds[current_strand], d_LED_num, CRGB::Black);
        fill_solid(d_leds[current_strand + 3], d_LED_num, CRGB::Black);
        fill_solid(ih_leds, ih_LED_total, CRGB::Black);
        fill_solid(oh_leds, oh_LED_total, CRGB::Black);
        FastLED.show();
        current_strand = (current_strand + 1) % 3;
        if ( current_strand == 0) {
          pal_index += 16;
        }
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(d_leds[current_strand + 3], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }
  // Flash strands 0 -> 6 -> 0
  if (anim_set == 2) {
    EVERY_N_MILLISECONDS(10) {
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
          pal_index += 16;
        }
        if (current_strand == 5) {
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
  if (anim_set == 3) {
    EVERY_N_MILLISECONDS(10) {
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
        next_strand = random8(6);
        while (next_strand == current_strand) {
          next_strand = random8(6);
        }
        current_strand = next_strand;
        pal_index += 16;
        just_switched = true;
      }
    }
    fill_solid(d_leds[current_strand], d_LED_num, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(ih_leds, ih_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    fill_solid(oh_leds, oh_LED_total, ColorFromPalette(gPalette, pal_index, brightness, gBlending));
    FastLED.show();
  }

}


// Simple theater chase animations, except that it snakes between inner perimeter,
// diagonals, and outer perimeter. Switch the orientation every once in a while, for a little
// added pizzaz
void theater_saw(uint8_t theat_opt) {
  // Incrememnt this to get the theater chase animations going.
  static uint8_t pal_index = 0;

  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 1, 200));
    pal_index++;
  }

  // Tell us which orientation we should be using
  static boolean orient = true;

  EVERY_N_MINUTES(2) {
    if (orient) {
      orient = false;
    }
    else {
      orient = true;
    }
    // Reset all the strands for a clean transition
    reset_all();
  }

  // Now, depending on the orientation, use the variables above to actually run the
  // animations on the selected features
  // theat_opt == 0 : Theater Chase
  // theat_opt == 1 : Theater Chase Bounce
  // theat_opt == 2 : Theater Chase Mod
  if (orient) {
    if (theat_opt == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 0) {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
        // Now set the inner hex sides
        theater_chase(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase(oh_leds, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 1) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 0) {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
        // Now set the inner hex sides
        theater_chase_tri(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_tri(oh_leds, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 2) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 0) {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
        // Now set the inner hex sides
        theater_chase_mod(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_mod(oh_leds, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
  }
  else {
    if (theat_opt == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 1) {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        // Now set the inner hex sides
        theater_chase(ih_leds, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase(oh_leds + oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 1) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 1) {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        // Now set the inner hex sides
        theater_chase_tri(ih_leds, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_tri(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_tri(oh_leds + oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_tri(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
    if (theat_opt == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        if (dd % 2 == 1) {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        // Now set the inner hex sides
        theater_chase_mod(ih_leds, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        theater_chase_mod(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
        // Now set the outer hex sides
        theater_chase_mod(oh_leds + oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
        theater_chase_mod(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
      }
    }
  }
  FastLED.show();
}

// Has the same overall pattern as the theater_saw, except that we illuminate whole sections,
// slowly building up the shape. Then we go black and build it up for the other orientation.
// Maybe we'll flash the whole shape a couple of times before switching orientations. (but not for now)
// Can switch the color every time we switch orientation.
void saw_solid() {
  // Use this to track which panel we are illuminating, since there are the same number of
  // panels regardless orientation. Also use pal_index to control the color of the illuminated
  // panels
  static uint8_t pan_index = 0;
  static uint8_t pal_index = 0;

  // Use these to track the orientation and if we should blackout the whole thing
  static boolean switched = true;
  static boolean orient = true;

  // Set everything to black if we are switching orientations, or if we are running
  // this animation for the first time
  if (switched) {
    reset_all();
    switched = false;
  }

  // Increment the panel we're supposed to illuminate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0 , 687, 1, 200));
    pan_index = (pan_index + 1) % 12; // 12 Sides at a time
  }

  // Lets see how this looks by going from off to on without any ramping
  if (pan_index == 0) {
    fill_solid(d_leds[0], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 1) {
    if (orient) {
      fill_solid(oh_leds, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(ih_leds, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 2) {
    fill_solid(d_leds[1], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 3) {
    if (orient) {
      fill_solid(ih_leds + ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(oh_leds + oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 4) {
    fill_solid(d_leds[2], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 5) {
    if (orient) {
      fill_solid(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 6) {
    fill_solid(d_leds[3], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 7) {
    if (orient) {
      fill_solid(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 8) {
    fill_solid(d_leds[4], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 9) {
    if (orient) {
      fill_solid(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
  }
  else if (pan_index == 10) {
    fill_solid(d_leds[5], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
  }
  else if (pan_index == 11) {
    if (orient) {
      fill_solid(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    else {
      fill_solid(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
    }
    // Now switch the orientation and set flag to reset the strands
    if (orient) {
      orient = false;
    }
    else {
      orient = true;
    }
    switched = true;
    pal_index += 16;
  }

  FastLED.show();
}

// This animation runs theater chase in loops around alternating trapazoids made up of two diagonals and
// adjacent inner and outer hexagon panels
void theater_trap_eo(uint8_t theat_opt) {
  // Use this to iterate on the theater chase animations
  static uint8_t pal_index = 0;

  // So we know which trapazoids to illuminate
  static boolean orient = true;

  // Control the theater_chase animations at an input determined rate
  EVERY_N_MILLISECONDS_I(thisTimer, 50) {
    thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 1, 150));
    pal_index++;
  }

  // Switch up the orientation every once in a while
  EVERY_N_MINUTES(1) {
    if (orient) {
      orient = false;
    }
    else {
      orient = true;
    }
    // Reset the strands whenever we switch
    reset_all();
  }

  // Now run the appropriate animation on the appropriate trapazoids
  if (orient) {
    if (theat_opt == 0) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase(ih_leds, ih_LED_per_strand, 255 - pal_index);
      theater_chase(oh_leds, oh_LED_per_strand, pal_index);

      theater_chase(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);

      theater_chase(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
    }
    if (theat_opt == 1) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_tri(ih_leds, ih_LED_per_strand, 255 - pal_index);
      theater_chase_tri(oh_leds, oh_LED_per_strand, pal_index);

      theater_chase_tri(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_tri(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);

      theater_chase_tri(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_tri(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
    }
    if (theat_opt == 2) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_mod(ih_leds, ih_LED_per_strand, 255 - pal_index);
      theater_chase_mod(oh_leds, oh_LED_per_strand, pal_index);

      theater_chase_mod(ih_leds + 2 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_mod(oh_leds + 2 * oh_LED_per_strand, oh_LED_per_strand, pal_index);

      theater_chase_mod(ih_leds + 4 * ih_LED_per_strand, ih_LED_per_strand, 255 - pal_index);
      theater_chase_mod(oh_leds + 4 * oh_LED_per_strand, oh_LED_per_strand, pal_index);
    }
  }
  else {
    if (theat_opt == 0) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase(oh_leds + oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);
    }
    if (theat_opt == 1) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_tri(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_tri(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_tri(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_tri(oh_leds + oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_tri(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_tri(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_tri(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_tri(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);
    }
    if (theat_opt == 2) {
      // Diagonals
      for (uint8_t dd = 0; dd < 6; dd++) {
        if ( dd % 2 == 0) {
          theater_chase_mod(d_leds[dd], d_LED_num, 255 - pal_index);
        }
        else {
          theater_chase_mod(d_leds[dd], d_LED_num, pal_index);
        }
      }
      // Inner and outer perimeters
      theater_chase_mod(ih_leds + ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_mod(oh_leds + oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_mod(ih_leds + 3 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_mod(oh_leds + 3 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);

      theater_chase_mod(ih_leds + 5 * ih_LED_per_strand, ih_LED_per_strand, pal_index);
      theater_chase_mod(oh_leds + 5 * oh_LED_per_strand, oh_LED_per_strand, 255 - pal_index);
    }
  }
}

// Same idea as theater_trap, except that we illuminate the solid trapazoids and alternate
// the subset at a user controlled rate.
void trap_eo_solid() {
  // Keep track of which side we are illuminating and the color of them
  static uint8_t pan_index = 0;
  static uint8_t pal_index = 0;


  // Use this to select which set of animations we want to run. This way we can step through a couple
  // different variations of the solid trapazoid shit
  static uint8_t anim_set = 0;
  static uint8_t num_runs = 0; // Use this to keep track of how many times we've run one set

  // First flash individual trapezoids
  if (anim_set == 0) {
    // Control the rate at which we illuminate panels at a user controlled rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 50, 200));
      pan_index = (pan_index + 1) % 6;

      // Turn everything off each time we switch sides
      reset_all();
    }

    // Illuminate the appropriate trapazoid.
    draw_trapezoid(pan_index, pal_index);
    if (pan_index == 5) {
      // Now change the color and increment the number of runs
      pal_index += 16;
      num_runs++;
    }

    FastLED.show();
    // Check how many times we've looped over this set, and move onto the next if we're at that point
    if (num_runs == 20) {
      anim_set = 1;
      num_runs = 0;
      pan_index = 0;
      reset_all();
    }
  }
  // Flash opposing trapezoids 2x the usual amount since it runs 2x as fast
  else if (anim_set == 1) {
    // Control the rotation at a user input rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analagRead(RATE_POT), 0, 687, 50, 200));
      pan_index = (pan_index + 1) % 3;
    }

    //

  }
  // Now turn the whole structure on one trapezoid at a time. The same idea as the previous animation
  // set except that we only run it once and we don't reset the structure after each trapezoid
  else if (anim_set == 2) {
    // Control the rate at which we illuminate panels at a user controlled rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 50, 200));
      pan_index = (pan_index + 1) % 6;
    }

    // Check which panel we have selected and illuminate the appropriate trapazoid.
    draw_trapezoid(pan_index, pal_index);
    // Now change the color and increment the number of runs if we're on the last panel
    if (pan_index == 5) {      
      pal_index += 16;
      num_runs++;
    }

    FastLED.show();
    // Check how many times we've looped over this set, and move onto the next if we're at that point
    if (num_runs == 1) {
      anim_set = 3;
      num_runs = 0;
      pan_index = 0;
      reset_all();
    }
  }
  // Now flash the whole structure on and off a few times, lets say 20?
  else if (anim_set == 3) {
    // Control flashing at a user input rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 50, 200));
      pan_index++;
    }

    if (pan_index % 2 == 0) {
      fill_all(pal_index);
    }
    else {
      reset_all();
      // Now change the color and increment the number of times we've looped over this animation set
      pal_index += 16;
      num_runs++;
    }
    FastLED.show();

    if (num_runs == 20) {
      anim_set = 4;
      pan_index = 0;
      num_runs = 0;
      reset_all();
    }
  }
  // Now flash every other set of trapezoids, alternating evens and odds for about 20x as well before reseting the whole animatino loop
  else if (anim_set == 5) {
    // Control the flashing at a user input rate
    EVERY_N_MILLISECONDS_I(thisTimer, 100) {
      thisTimer.setPeriod(map(analogRead(RATE_POT), 0, 687, 50, 200));
      pan_index++;

      // Reset the whole array every time we increment
      reset_all();
    }

    // If even, turn on all even trapezoids
    if ( pan_index % 2 == 0) {
      for (uint8_t dd = 0; dd < 6; dd++) {
        fill_solid(d_leds[dd], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        if (dd % 2 == 0) {
          fill_solid(ih_leds + dd * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
          fill_solid(oh_leds + dd * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        }
      }
    }
    // If odd, turn on all odd trapezoids
    else {
      for (uint8_t dd = 0; dd < 6; dd++) {
        fill_solid(d_leds[dd], d_LED_num, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        if (dd % 2 == 1) {
          fill_solid(ih_leds + dd * ih_LED_per_strand, ih_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
          fill_solid(oh_leds + dd * oh_LED_per_strand, oh_LED_per_strand, ColorFromPalette(gPalette, pal_index, gBrightness, gBlending));
        }
      }
      // Increment color and run counter
      num_runs++;
      pal_index += 16;
    }

    FastLED.show();

    if (num_runs == 20) {
      anim_set = 0;
      num_runs = 0;
      pan_index = 0;
      reset_all();
    }
  }

}
*/
