#pragma once
#ifndef DisplayAnimations_h
#define DisplayAnimations_h

#define defaultHoldTime 100
#define DEFAULT_BRIGHTNESS 15
#define DISPLAY_COLON 0b01000000
#define LAST_FRAME 0b10000000
#define RANDOMIZE_DISPLAY 0b00000001

typedef struct {
  uint8_t digitMasks[4];      // Segment index 2 is the colon ':' on this display
  unsigned long holdTime; // in milliseconds
  uint8_t brightness;     // value of 1 - 15
  uint8_t controlBits;
} animationFrame;

// Description:
//
//  HEX codes 7 segment
//
//          01
//      ---------
//     /         /
//  20 /         / 02
//     /         /
//     /--- 40 --/
//     /         /
//  10 /         / 04
//     /         /
//      ---------
//         08
//
// When RANDOMIZE_DISPLAY is enabled for a frame, all four digits are randomized and &'d with the digitMask (bitwise).
//  This way we can control which segments are being randomized.
//
// Template
// All Off
//{ { 0b00000000, 0b00000000, 0b00000000, 0b00000000 }, defaultHoldTime, DEFAULT_BRIGHTNESS,  LAST_FRAME | DISPLAY_COLON | RANDOMIZE_DISPLAY},
// All On
//{ { 0b11111111, 0b11111111, 0b11111111, 0b11111111 }, defaultHoldTime, DEFAULT_BRIGHTNESS,  LAST_FRAME | DISPLAY_COLON | RANDOMIZE_DISPLAY},

// One segment circles around the whole display
const animationFrame animation1[6] = {
  { { 0b00000001, 0b00000000, 0b00000000, 0b00001000 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000001, 0b00001000, 0b00000000 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00001000, 0b00000001, 0b00000000 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000001 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00010000, 0b00000000, 0b00000000, 0b00000010 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00100000, 0b00000000, 0b00000000, 0b00000100 }, defaultHoldTime, DEFAULT_BRIGHTNESS, LAST_FRAME }
};

const animationFrame animation2[8] = {
  { { 0b00000001, 0b00000000, 0b00000000, 0b00001000 }, defaultHoldTime, 1, 0 },
  { { 0b00000000, 0b00000001, 0b00001000, 0b00000000 }, defaultHoldTime, 3, 0 },
  { { 0b00000000, 0b00000010, 0b00010000, 0b00000000 }, defaultHoldTime, 5, 0 },
  { { 0b00000000, 0b00000100, 0b00100000, 0b00000000 }, defaultHoldTime, 7, 0 },
  { { 0b00000000, 0b00001000, 0b00000001, 0b00000000 }, defaultHoldTime, 9, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000001 }, defaultHoldTime, 11, 0 },
  { { 0b00010000, 0b00000000, 0b00000000, 0b00000010 }, defaultHoldTime, 13, 0 },
  { { 0b00100000, 0b00000000, 0b00000000, 0b00000100 }, defaultHoldTime, 15, LAST_FRAME }
};

const animationFrame animation3[4] = {
  { { 0b00000001, 0b00000001, 0b00000001, 0b00000001 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000010, 0b00000010, 0b00000010, 0b00000010 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b01000000, 0b01000000, 0b01000000, 0b01000000 }, defaultHoldTime, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00100000, 0b00100000, 0b00100000, 0b00100000 }, defaultHoldTime, DEFAULT_BRIGHTNESS, LAST_FRAME },
  
};

#define sevenOhSixFadeTime 100
const animationFrame animation4[28] = {
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 1, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 2, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 3, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 4, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 5, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 6, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 7, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 8, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 9, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 10, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 11, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 12, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 13, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 14, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 15, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 14, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 13, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 12, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 11, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 10, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 9, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 8, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 7, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 6, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 5, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 4, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 3, DISPLAY_COLON },
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 2, LAST_FRAME | DISPLAY_COLON }
};

const animationFrame animation5[48] = {
  { { 0b00000001, 0b00000000, 0b00000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000001, 0b00000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000001, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00000001 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b01000000, 0b00000000, 0b00000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b01000000, 0b00000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b01000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b01000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00001000, 0b00000000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00001000, 0b00000000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00001000 }, 100, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000001, 0b00000000, 0b00000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000001, 0b00000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000001, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00000001 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b01000000, 0b00000000, 0b00000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b01000000, 0b00000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b01000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b01000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00001000, 0b00000000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00001000, 0b00000000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00001000 }, 50, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000001, 0b00000000, 0b00000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000001, 0b00000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000001, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00000001 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b01000000, 0b00000000, 0b00000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b01000000, 0b00000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b01000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b01000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00001000, 0b00000000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00001000, 0b00000000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00001000 }, 25, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000001, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000001, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000001, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00000001 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b01000000, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b01000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b01000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b01000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00001000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00001000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00001000 }, 5, DEFAULT_BRIGHTNESS, LAST_FRAME }
};

const animationFrame animation6[2] = {
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, defaultHoldTime, 9, DISPLAY_COLON},
  { { 0b01111111, 0b01111000, 0b01000000, 0b00000010 }, defaultHoldTime, 2, LAST_FRAME | DISPLAY_COLON | RANDOMIZE_DISPLAY}
};

const animationFrame animation7[12] = { 
  { { 0b00000001, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000001, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000001, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00000001 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b01000000, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b01000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b01000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b01000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00001000, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00001000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00001000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00001000 }, 5, DEFAULT_BRIGHTNESS, LAST_FRAME }
};

const animationFrame animation8[12] = { 
  { { 0b00111001, 0b00000000, 0b00000000, 0b00000000 }, 5, DEFAULT_BRIGHTNESS, 0 },
  { { 0b00000000, 0b00000000, 0b00000000, 0b00001000 }, 5, DEFAULT_BRIGHTNESS, LAST_FRAME }
};

const animationFrame animation9[2] = {
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, defaultHoldTime, 9, DISPLAY_COLON},
  { { 0b00000000, 0b00000111, 0b00111111, 0b01111101 }, defaultHoldTime, 2, LAST_FRAME | DISPLAY_COLON | RANDOMIZE_DISPLAY}
};



#endif