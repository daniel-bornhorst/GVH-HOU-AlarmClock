#ifndef ClockGlobals_h
#define ClockGlobals_h

//#define ARDUINO_TEENSY41 1

typedef enum ClockState
{
  SIMPLE_MODE,
  SIMPLE_BREATH_MODE,
  IDLE,
  GLITCH,
  SLEEP,
  WAKE,
  HOUR,
  MINUTE,
  SNOOZ,
  RADIO
} clock_state_;

// Represents the available user inputs on the clock
// the values are the teensy pins connected the input
#ifdef ARDUINO_TEENSY41

// Pin Values for Teensy
typedef enum ClockInput
{
  SLEEP_BUTTON = 28,
  WAKE_BUTTON = 30,
  HOUR_BUTTON = 29,
  MINUTE_BUTTON = 31,
  SNOOZ_BUTTON = 26,
  ON_SWITCH = 9,
  OFF_SWITCH = 10,
  RADIO_SWITCH = 11,
  ALARM_SWITCH = 12
} clock_input_;
#else

// Pin Values for QTPY
typedef enum ClockInput
{
  SLEEP_BUTTON = 3,
  WAKE_BUTTON = 1,
  HOUR_BUTTON = 2,
  MINUTE_BUTTON = 8,
  SNOOZ_BUTTON = 10,
  MODE_SWITCH = A0,
  TUNING_POT = A1
} clock_input_;
#endif

// Represents the different states of the four way toggle
typedef enum ToggleSwitchState
{
  NO_SWITCH_STATE,
  ON_SWITCH_STATE,
  OFF_SWITCH_STATE,
  RADIO_SWITCH_STATE,
  ALARM_SWITCH_STATE
} toggle_switch_state_;


#endif