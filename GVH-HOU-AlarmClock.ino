#include <Bounce.h>

//#include "ClockAudio.h"
#include "ClockDisplay.h"
#include "ClockGlobals.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

AudioPlaySdWav playWav1;
AudioPlayMemory sound0;
AudioMixer4 mix1;
AudioOutputI2S2 headphones;
AudioConnection c1(playWav1, 0, mix1, 0);
AudioConnection c2(sound0, 0, mix1, 1);
AudioConnection c3(mix1, 0, headphones, 0);
AudioConnection c4(mix1, 0, headphones, 1);

// Peripheral Devies
ClockDisplay display;
//ClockAudio audio;

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
elapsedMillis idleTimeout;

void setup() {

  AudioMemory(10);

  pinMode(ATM_BUTTON, INPUT_PULLUP);
  pinMode(VENDE_BUTTON, INPUT_PULLUP);
  pinMode(PIANO_BUTTON, INPUT_PULLUP);
  pinMode(TURNTABLE_BUTTON, INPUT_PULLUP);
  pinMode(SNOOZ_BUTTON, INPUT_PULLUP);

  display.setup();
  display.playIdleAnimation();

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  mix1.gain(0, 0.1);
  mix1.gain(1, 0.1);

  //audio.setup();

  Serial.begin(9600);
  delay(1000);
}

void loop() {

  // Update our peripheral classes
  display.loop();
  //audio.loop();


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
  if (idleTimeout >= 5000 && clockState != IDLE) {
    clockState = IDLE;
    display.playIdleAnimation();
  }

}

void buttonPressed(ClockInput pressedButton) {
  
  idleTimeout = 0;

  if (pressedButton == ATM_BUTTON) {
    clockState = ATM;
    playWav1.play("GLADIATORS.WAV");
    display.playAtmAnimation();
  }
  else if (pressedButton == VENDE_BUTTON) {
    clockState = VENDE;
    playWav1.play("DEMNTEDCIRCUS.WAV");
    display.playVendeAnimation();
  }
  else if (pressedButton == PIANO_BUTTON) {
    clockState = PIANO;
    playWav1.play("3SECSAWSWEEP.WAV");
    display.playPianoAnimation();
  }
  else if (pressedButton == TURNTABLE_BUTTON) {
    clockState = TURNTABLE;
    display.stringBuffer = "    Gordon SuckS SHIt";
    playWav1.play("3SECSQUARESWEEP.WAV");
    display.playTurntableAnimation();
  }
  else if (pressedButton == SNOOZ_BUTTON) {
    clockState = SNOOZ;
    playWav1.stop();
    display.playSnoozAnimation();
  }

  Serial.print("audio usage:  ");
  Serial.println(AudioProcessorUsage());
}
