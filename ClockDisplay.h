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
  elapsedMillis _frameTimer;

  int _hours;
  int _minutes;

  uint8_t _displayBuffer[4];

  // This variable specifies whether or not the current animation is scripted or programatic
  bool _scriptedAnimation;

  // Variables for scripted behavior 
  const animationFrame* _currentAnimation;
  int _frameIndex;
  int _anmiationRepetitions = 0;

  // Variables for programatic behavior
  int _refreshRate;

  void randomizeDisplayBuffer();
  void loadDisplayBuffer(const uint8_t* frame);

};

#endif