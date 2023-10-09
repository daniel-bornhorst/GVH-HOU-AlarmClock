#ifndef ClockDisply_h
#define ClockDisply_h

#include "Arduino.h"
#include "ClockGlobals.h"
#include "ClockDisplayAnimations.h"

// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include "Adafruit_LEDBackpack.h"

//#include "HT16K33.h"
#include "HT16K33Driver.h"



class ClockDisplay {
public:
  // Constructors
  ClockDisplay(); // Default constructor

  void begin();
  void loop();

  void playIdleAnimation();
  void playAtmAnimation();
  void playVendeAnimation();
  void playPianoAnimation();
  void playTurntableAnimation();
  void playSnoozAnimation();

  // Member functions
  void setClock(int hours, int minutes); // Set the time

private:

  HT16K33Driver _matrix;
  int _hours;
  int _minutes;

  unsigned long _currentMillis = 0;
  unsigned long _previousMillis = 0;
  int _frameIndex;
  int _anmiationRepetitions = 0;
  int _numRepeats = 3;

  const animationFrame* _currentAnimation;
  unsigned long _currentFrameHoldTimeMillis = 0;

};

#endif