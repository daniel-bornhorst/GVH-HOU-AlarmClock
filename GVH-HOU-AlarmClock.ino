#include <Bounce.h>

#include "ClockAudio.h"
#include "ClockDisplay.h"
#include "ClockGlobals.h"

ClockDisplay display;
ClockAudio audio;

ClockState clockState = IDLE;

Bounce atmButton = Bounce(ATM_BUTTON, 5);
Bounce vendeButton = Bounce(VENDE_BUTTON, 5);  // 5 ms debounce time
Bounce pianoButton = Bounce(PIANO_BUTTON, 5);
Bounce turntableButton = Bounce(TURNTABLE_BUTTON, 5);
Bounce snoozButton = Bounce(SNOOZ_BUTTON, 5);

void setup() {

  pinMode(ATM_BUTTON, INPUT_PULLUP);
  pinMode(VENDE_BUTTON, INPUT_PULLUP);
  pinMode(PIANO_BUTTON, INPUT_PULLUP);
  pinMode(TURNTABLE_BUTTON, INPUT_PULLUP);
  pinMode(SNOOZ_BUTTON, INPUT_PULLUP);

  display.begin();

  Serial.begin(9600);
  delay(1000);
}

void loop() {

  display.loop();
  audio.loop();

  atmButton.update();
  vendeButton.update();
  pianoButton.update();
  turntableButton.update();
  snoozButton.update();

  if (atmButton.fallingEdge()) {
    Serial.println("atm");
    clockState = ATM;
    display.playSleepAnimation();
  }
  else if (vendeButton.fallingEdge()) {
    Serial.println("vende");
    clockState = VENDE;
    display.playWakeAnimation();
  }
  else if (pianoButton.fallingEdge()) {
    Serial.println("piano");
    clockState = PIANO;
    display.playHourAnimation();
  }
  else if (turntableButton.fallingEdge()) {
    Serial.println("turntable");
    clockState = TURNTABLE;
    display.playMinuteAnimation();
  }
  else if (snoozButton.fallingEdge()) {
    Serial.println("snooz");
    clockState = SNOOZ;
    display.playSnoozAnimation();
  }

}

void buttonPress(ClockInput pressedButton) {
  if (pressedButton == ATM_BUTTON) {
    clockState = ATM;
    display.playSleepAnimation();
  }
  else if (pressedButton == VENDE_BUTTON) {
    clockState = VENDE;
    display.playWakeAnimation();
  }
  else if (pressedButton == PIANO_BUTTON) {
    clockState = PIANO;
    display.playHourAnimation();
  }
  else if (pressedButton == TURNTABLE_BUTTON) {
    clockState = TURNTABLE;
    display.playMinuteAnimation();
  }
  else if (pressedButton == SNOOZ_BUTTON) {
    clockState = SNOOZ;
    display.playSnoozAnimation();
  }
}
