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

// Represents the available user inputs on the clock
// the values are the teensy pins connected the input
typedef enum ClockInput
{
  ATM_BUTTON = 36,
  VENDE_BUTTON = 35,
  PIANO_BUTTON = 34,
  TURNTABLE_BUTTON = 33,
  SNOOZ_BUTTON = 37,
  MODE_SWITCH = 38,
  TUNING_POT = A2
} clock_input_;

// Represents the different states of the four way toggle
typedef enum ToggleSwitchState
{
  ON_SWITCH_STATE,
  OFF_SWITCH_STATE,
  MUSIC_SWITCH_STATE,
  ALARM_SWITCH_STATE
} toggle_switch_state_;

#endif