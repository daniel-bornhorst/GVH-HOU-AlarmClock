#include <Bounce.h>

#include "ClockAudio.h"
#include "ClockDisplay.h"
#include "ClockGlobals.h"

// Peripheral Devies
ClockDisplay display;
ClockAudio audio;

// State holders
ClockState clockState = IDLE;
ToggleSwitchState toggleSwitchState = OFF_SWITCH_STATE;

// Buttons
Bounce atmButton = Bounce(ATM_BUTTON, 100);
Bounce vendeButton = Bounce(VENDE_BUTTON, 100);  // 5 ms debounce time
Bounce pianoButton = Bounce(PIANO_BUTTON, 100);
Bounce turntableButton = Bounce(TURNTABLE_BUTTON, 100);
Bounce snoozButton = Bounce(SNOOZ_BUTTON, 100);

// Timers
elapsedMillis returnToIdleTimer;

void setup() {

  pinMode(ATM_BUTTON, INPUT_PULLUP);
  pinMode(VENDE_BUTTON, INPUT_PULLUP);
  pinMode(PIANO_BUTTON, INPUT_PULLUP);
  pinMode(TURNTABLE_BUTTON, INPUT_PULLUP);
  pinMode(SNOOZ_BUTTON, INPUT_PULLUP);

  display.begin();
  display.playIdleAnimation();

  Serial.begin(9600);
  delay(1000);
}

void loop() {

  // Update our peripheral classes
  display.loop();
  audio.loop();


  // Check for button presses
  atmButton.update();
  vendeButton.update();
  pianoButton.update();
  turntableButton.update();
  snoozButton.update();

  if (atmButton.fallingEdge()) {
    buttonPressed(ATM_BUTTON);
  }
  else if (vendeButton.fallingEdge()) {
    buttonPressed(VENDE_BUTTON);
  }
  else if (pianoButton.fallingEdge()) {
    buttonPressed(PIANO_BUTTON);
  }
  else if (turntableButton.fallingEdge()) {
    buttonPressed(TURNTABLE_BUTTON);
  }
  else if (snoozButton.fallingEdge()) {
    buttonPressed(SNOOZ_BUTTON);
  }

  // Return to IDLE mode timeout
  if (returnToIdleTimer >= 5000 && clockState != IDLE) {
    clockState = IDLE;
    display.playIdleAnimation();
  }

}

void buttonPressed(ClockInput pressedButton) {
  
  returnToIdleTimer = 0;

  if (pressedButton == ATM_BUTTON) {
    clockState = ATM;
    display.playAtmAnimation();
  }
  else if (pressedButton == VENDE_BUTTON) {
    clockState = VENDE;
    display.playVendeAnimation();
  }
  else if (pressedButton == PIANO_BUTTON) {
    clockState = PIANO;
    display.playPianoAnimation();
  }
  else if (pressedButton == TURNTABLE_BUTTON) {
    clockState = TURNTABLE;
    display.playTurntableAnimation();
  }
  else if (pressedButton == SNOOZ_BUTTON) {
    clockState = SNOOZ;
    display.playSnoozAnimation();
  }
}
