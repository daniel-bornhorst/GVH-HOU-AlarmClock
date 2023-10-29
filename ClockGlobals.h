#ifndef ClockGlobals_h
#define ClockGlobals_h

typedef enum ClockState
{
  IDLE,
  GLITCH,
  SLEEP,
  WAKE,
  HOUR,
  MINUTE,
  SNOOZ,
  MUSIC,
  ACTUAL_TIME,
  TUNER
} clock_state_;

// Represents the available user inputs on the clock
// the values are the teensy pins connected the input
typedef enum ClockInput
{
  SLEEP_BUTTON = 36,
  WAKE_BUTTON = 35,
  HOUR_BUTTON = 34,
  MINUTE_BUTTON = 33,
  SNOOZ_BUTTON = 37,
  MODE_SWITCH = 38,
  TUNING_POT = A1
} clock_input_;

// Represents the different states of the four way toggle
typedef enum ToggleSwitchState
{
  ON_SWITCH_STATE,
  OFF_SWITCH_STATE,
  MUSIC_SWITCH_STATE,
  ALARM_SWITCH_STATE
} toggle_switch_state_;

const uint8_t tunerEncoderPin1 = 30;
const uint8_t tunerEncoderPin2 = 31;

const uint8_t tunerLedPinLeft = 27;
const uint8_t tunerLedPinRight  = 28;

#endif