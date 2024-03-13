#ifndef ClockGlobals_h
#define ClockGlobals_h

//#define DEBUG 1

#ifdef DEBUG
# define DEBUG_PRINTLN(x) Serial.println(x);
# define DEBUG_PRINT(x) Serial.print(x);
#else
# define DEBUG_PRINTLN(x) do {} while (0)
# define DEBUG_PRINT(x) do {} while (0)
#endif


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
  ON_MODE,
  OFF_MODE,
  RADIO_MODE,
  ALARM_MODE,
  GORDON,
  FILE_NOT_FOUND
} clock_state_;

// Represents the available user inputs on the clock
// the values are the teensy pins connected the input

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