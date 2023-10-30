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
  SLEEP_BUTTON = 3,
  WAKE_BUTTON = 1,
  HOUR_BUTTON = 2,
  MINUTE_BUTTON = 8,
  SNOOZ_BUTTON = 10,
  MODE_SWITCH = A0,
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