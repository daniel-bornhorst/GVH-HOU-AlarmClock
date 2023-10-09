#pragma once
#ifndef DisplayAnimations_h
#define DisplayAnimations_h

#define defaultHoldTime 20
#define defaultBrightness 15
#define displayColon 0b00000010
#define LAST_FRAME 0b10000000

typedef struct {
  int digitMasks[5];      // Segment index 2 is the colon ':' on this display
  unsigned long holdTime; // in milliseconds
  uint8_t brightness;     // value of 1 - 15
  uint8_t controlBits;
} animationFrame;

// One segment circles around the whole display
const animationFrame animation1[6] = {
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00001000 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00001000, 0b00000000 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000001, 0b00000000 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000001 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00010000, 0b00000000, 0, 0b00000000, 0b00000010 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00100000, 0b00000000, 0, 0b00000000, 0b00000100 }, defaultHoldTime, defaultBrightness, LAST_FRAME }
  // { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00001000 }, defaultHoldTime },
  // { { 0b00000000, 0b00000001, 0, 0b00001000, 0b00000000 }, defaultHoldTime },  // 2 is the colon
  // { { 0b00000000, 0b00001000, 0, 0b00000001, 0b00000000 }, defaultHoldTime },
  // { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000001 }, defaultHoldTime },
  // { { 0b00010000, 0b00000000, 0, 0b00000000, 0b00000010 }, defaultHoldTime },
  // { { 0b00100000, 0b00000000, 0, 0b00000000, 0b00000100 }, defaultHoldTime }
};

//{ { 0b00000000, 0b00000000, 0, 0b00000000, 0b00000000 }, defaultHoldTime, defaultBrightness },
const animationFrame animation2[8] = {
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00001000 }, defaultHoldTime, 1, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00001000, 0b00000000 }, defaultHoldTime, 3, 0 },
  { { 0b00000000, 0b00000010, 0, 0b00010000, 0b00000000 }, defaultHoldTime, 5, 0 },
  { { 0b00000000, 0b00000100, 0, 0b00100000, 0b00000000 }, defaultHoldTime, 7, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000001, 0b00000000 }, defaultHoldTime, 9, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000001 }, defaultHoldTime, 11, 0 },
  { { 0b00010000, 0b00000000, 0, 0b00000000, 0b00000010 }, defaultHoldTime, 13, 0 },
  { { 0b00100000, 0b00000000, 0, 0b00000000, 0b00000100 }, defaultHoldTime, 15, LAST_FRAME }
};

const animationFrame animation3[4] = {
  { { 0b00000001, 0b00000001, 0, 0b00000001, 0b00000001 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00000010, 0b00000010, 0, 0b00000010, 0b00000010 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b01000000, 0b01000000, 0, 0b01000000, 0b01000000 }, defaultHoldTime, defaultBrightness, 0 },
  { { 0b00100000, 0b00100000, 0, 0b00100000, 0b00100000 }, defaultHoldTime, defaultBrightness, LAST_FRAME },
  
};

#define sevenOhSixFadeTime 100
const animationFrame animation4[28] = {
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 1, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 2, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 3, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 4, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 5, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 6, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 7, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 8, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 9, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 10, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 11, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 12, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 13, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 14, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 15, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 14, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 13, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 12, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 11, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 10, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 9, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 8, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 7, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 6, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 5, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 4, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 3, 0 },
  { { 0b00000000, 0b00000111, displayColon, 0b00111111, 0b01111101 }, sevenOhSixFadeTime, 2, LAST_FRAME }
};

const animationFrame animation5[48] = {
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000001, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00000001 }, 100, defaultBrightness, 0 },
  { { 0b01000000, 0b00000000, 0, 0b00000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b01000000, 0, 0b00000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b01000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b01000000 }, 100, defaultBrightness, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00001000, 0b00000000 }, 100, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00001000 }, 100, defaultBrightness, 0 },
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000001, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00000001 }, 50, defaultBrightness, 0 },
  { { 0b01000000, 0b00000000, 0, 0b00000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b01000000, 0, 0b00000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b01000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b01000000 }, 50, defaultBrightness, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00001000, 0b00000000 }, 50, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00001000 }, 50, defaultBrightness, 0 },
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000001, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00000001 }, 25, defaultBrightness, 0 },
  { { 0b01000000, 0b00000000, 0, 0b00000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b01000000, 0, 0b00000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b01000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b01000000 }, 25, defaultBrightness, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00001000, 0b00000000 }, 25, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00001000 }, 25, defaultBrightness, 0 },
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000001, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00000001 }, 5, defaultBrightness, 0 },
  { { 0b01000000, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b01000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b01000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b01000000 }, 5, defaultBrightness, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00001000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00001000 }, 5, defaultBrightness, LAST_FRAME }
};

const animationFrame animation6[2] = {
  { { 0b00000000, 0b00000111, 2, 0b00111111, 0b01111101 }, defaultHoldTime, 9, 0 },
  { { 0b01111111, 0b01111000, 2, 0b01000000, 0b00000010 }, defaultHoldTime, 2, LAST_FRAME }
};

const animationFrame animation7[12] = { 
  { { 0b00000001, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000001, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000001, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00000001 }, 5, defaultBrightness, 0 },
  { { 0b01000000, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b01000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b01000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b01000000 }, 5, defaultBrightness, 0 },
  { { 0b00001000, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00001000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00001000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00001000 }, 5, defaultBrightness, LAST_FRAME }
};

const animationFrame animation8[12] = { 
  { { 0b00111001, 0b00000000, 0, 0b00000000, 0b00000000 }, 5, defaultBrightness, 0 },
  { { 0b00000000, 0b00000000, 0, 0b00000000, 0b00001000 }, 5, defaultBrightness, LAST_FRAME }
};

#endif