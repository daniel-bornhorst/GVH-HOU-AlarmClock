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


typedef enum DisplayState
{
  DISPLAY_OFF,
  SCRIPTED_ANIMATION,
  BUFFER_SCROLL,
  VU_METER,
  
} display_state_;



class ClockDisplay {
public:
  // Constructors
  ClockDisplay(); // Default constructor
  String stringBuffer;

  void setup();
  void loop();

  void playIdleAnimation();
  void playAtmAnimation();
  void playVendeAnimation();
  void playPianoAnimation();
  void playTurntableAnimation();
  void playSnoozAnimation();

  void setVuMeter(uint8_t level);

  // Member functions
  void setTime(int hours, int minutes); // Set the time

private:

  HT16K33Driver _matrix;
  elapsedMillis _frameTimer;

  int _hours;
  int _minutes;

  DisplayState _displayState = DISPLAY_OFF;
  int _refreshRate;

  // Display Scrolling Variables
  uint8_t _displayBuffer[4];
  //String _stringBuffer;
  uint8_t _stringScrollIndex;
  unsigned long _scrollStepRate;

  // Variables for scripted behavior 
  const AnimationFrame* _currentAnimation;
  int _frameIndex;
  bool _frameAdvanced = false;
  int _anmiationRepetitions = 0;


  void playScriptedAnimation(bool newAnimation = false);
  void scrollStringBuffer();
  void randomizeDisplayBuffer(const uint8_t* frame);
  void loadDisplayBuffer(const uint8_t* frame);
  void loadAnimation(const AnimationFrame* newAnimation);

};

#endif