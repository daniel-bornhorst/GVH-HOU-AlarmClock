#ifndef ClockDisply_h
#define ClockDisply_h

#include "Arduino.h"
#include "ClockGlobals.h"
#include "ClockDisplayAnimations.h"
#include "elapsedMillis.h"


// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include "Adafruit_LEDBackpack.h"

#include "HT16K33Driver.h"


typedef enum DisplayState
{
  DISPLAY_OFF,
  SCRIPTED_ANIMATION,
  MANUAL_STATIC,
  BUFFER_SCROLL,
  VU_METER
  
} display_state_;



class ClockDisplay {
public:
  // Constructors
  ClockDisplay(); // Default constructor

  void setup();
  void loop();

  void playIdleAnimation();
  void playGlitchAnimation();
  void playSleepAnimation();
  void playWakeAnimation();
  void playHourAnimation();
  void playMinuteAnimation();
  void playSnoozAnimation();
  void playSnoozQueueAnimation(int animationIndex);

  void setVuMeter(uint8_t level);

  void scrollString(String buff);

  void displayInt(int displayVal);
  void displayString(String displayString);
  void displayTime(int hours = 7, int minutes = 6);

  // Member functions
  void setTime(int hours, int minutes); // Set the time
  void setStringBuffer(String buff);
  void setStringBuffer(int intValBuff);
  void clear();


private:

  HT16K33Driver _matrix = HT16K33Driver();
  elapsedMillis _frameTimer;

  int _hours;
  int _minutes;

  DisplayState _displayState = DISPLAY_OFF;
  int _refreshRate;

  // Display Scrolling Variables
  uint8_t _displayBuffer[4];
  String _stringBuffer;
  uint8_t _stringScrollIndex;
  const unsigned long _scrollStepRate = 200;

  // Variables for scripted behavior 
  const AnimationFrame* _currentAnimation;
  int _frameIndex;
  bool _frameAdvanced = false;
  int _anmiationRepetitions = 0;

  void setDisplayState(DisplayState newState);
  void scriptedAnimationLoop(bool newAnimation = false);
  void scrollLoop();
  void randomizeDisplayBuffer(const uint8_t* frame);
  void loadDisplayBuffer(const uint8_t* frame);
  void loadAnimation(const AnimationFrame* newAnimation);

};

#endif