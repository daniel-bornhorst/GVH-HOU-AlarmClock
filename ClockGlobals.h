#ifndef ClockGlobals_h
#define ClockGlobals_h

typedef enum ClockState
{
  IDLE,
  ATM,
  VENDE,
  PIANO,
  TURNTABLE,
  SNOOZ,
  ACTUAL_TIME
} clock_state_;

// Represents the buttons on the clock
// the values are the teensy pins connected the button
typedef enum ClockInput
{
  ATM_BUTTON = 33,
  VENDE_BUTTON = 34,
  PIANO_BUTTON = 35,
  TURNTABLE_BUTTON = 36,
  SNOOZ_BUTTON = 37,
  MODE_SWITCH = 38,
  TUNING_POT = A2,
} clock_input_;

#endif