#include "ClockDisplay.h"
#include "ClockGlobals.h"
#include "elapsedMillis.h"
#include <Bounce2.h>

#ifdef ARDUINO_TEENSY41  //---------------------------------------------------------------------
#define ENCODER_DO_NOT_USE_INTERRUPTS

#include <Encoder.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11  // not actually used
#define SDCARD_SCK_PIN 13   // not actually used


// Audio Stuff
AudioPlaySdWav playWav1;
AudioPlaySdWav playWav2;
AudioPlaySdWav playWav3;
AudioSynthNoisePink noise1;
//AudioAnalyzeRMS peak1;
AudioAnalyzePeak peak1;
AudioMixer4 mixer1;
AudioOutputI2S headphones;
AudioConnection c1(noise1, 0, mixer1, 0);
AudioConnection c2(playWav1, 0, mixer1, 1);
AudioConnection c3(playWav2, 0, mixer1, 2);
AudioConnection c4(playWav3, 0, mixer1, 3);
AudioConnection c5(mixer1, peak1);
AudioConnection c6(mixer1, 0, headphones, 0);
AudioConnection c7(mixer1, 0, headphones, 1);

// Peripheral Devies
Encoder tunerEncoder(tunerEncoderPin1, tunerEncoderPin2);
#endif  //---------------------------------------------------------------------
ClockDisplay display;

// State holders
ClockState clockState = IDLE;
ToggleSwitchState toggleSwitchState = OFF_SWITCH_STATE;

// Buttons
Bounce2::Button sleepButton = Bounce2::Button();
Bounce2::Button wakeButton = Bounce2::Button();
Bounce2::Button hourButton = Bounce2::Button();
Bounce2::Button minuteButton = Bounce2::Button();
Bounce2::Button snoozButton = Bounce2::Button();

// Timers
elapsedMillis idleTimeoutTimer;
elapsedMillis vuMeterRefreshTimer;
elapsedMillis glitchTimer;
elapsedMillis ledToggleTimer;
elapsedMillis ledOnTimeTimer;
elapsedMillis modeSwitchPollTimer;
elapsedMillis dynamicRefreshUpdateTimer;

// Consts
const int defaultIdleTimeoutTime = 5000;
long unsigned int tunerModeTimeoutTime = 3000;
//long unsigned int glitchTimeoutTime = 600000; // 10 min
long unsigned int glitchTimeoutTime = 240000;  // 4 min
//long unsigned int glitchTimeoutTime = 60000; // 1 min
//long unsigned int glitchTimeoutTime = 30000; // 30 sec
long unsigned const int ledOnTime = 10;
const int vuMeterRefreshRate = 42;
long unsigned const int modeSwitchPollRate = 10;
long unsigned const int dynamicRefreshUpdateRate = 250;
long unsigned const int watchdogInterval = 30000;  // 30 secnds

// Variables
long unsigned int idleTimeoutTime = defaultIdleTimeoutTime;
long tunerPosition = 0;
long unsigned int ledToggleTime = 0;
bool ledToggle = false;


void setup() {

  // Serial
  Serial.begin(9600);
  delay(1000);

  // Button Setup
  sleepButton.attach(SLEEP_BUTTON, INPUT_PULLUP);
  wakeButton.attach(WAKE_BUTTON, INPUT_PULLUP);
  hourButton.attach(HOUR_BUTTON, INPUT_PULLUP);
  minuteButton.attach(MINUTE_BUTTON, INPUT_PULLUP);
  snoozButton.attach(SNOOZ_BUTTON, INPUT_PULLUP);

  sleepButton.interval(50);
  wakeButton.interval(50);
  hourButton.interval(50);
  minuteButton.interval(50);
  snoozButton.interval(50);

  sleepButton.setPressedState(LOW);
  wakeButton.setPressedState(LOW);
  hourButton.setPressedState(LOW);
  minuteButton.setPressedState(LOW);
  snoozButton.setPressedState(LOW);

  pinMode(MODE_SWITCH, INPUT);

  // Display Setup
  display.setup();
  display.setTime(7, 6);
  display.playIdleAnimation();

#ifdef ARDUINO_TEENSY41

  // SD Card Setup
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Unable to access the SD card");
    delay(500);
  }

  delay(1000);
  // Audio Setup
  AudioMemory(10);
  playWav1.play("ETNL.WAV");
  delay(1000);
  playWav2.play("LUCIUS.WAV");
  delay(1000);
  playWav3.play("NUMBERS.WAV");
  delay(1000);

  noise1.amplitude(0.5);

  mixer1.gain(0, 0.0);
  mixer1.gain(1, 0.0);
  mixer1.gain(2, 0.0);
  mixer1.gain(3, 0.0);
#endif

  delay(1000);

  // Simple Mode Check
  // Start in simple mode unless these three buttons are held during boot
  // if ( !(sleepButton.isPressed() && minuteButton.isPressed() && snoozButton.isPressed()) ) {
  //   clockState = SIMPLE_MODE;
  //   display.displayTime();
  //   Serial.println("WE IN SIMP MODE YO!");
  // }
}

void loop() {

  if (clockState != SIMPLE_MODE) {
    display.loop();
    inputPollingLoop();
  }

  switch (clockState) {
    case SIMPLE_MODE:
      break;
    case IDLE:
      checkForGlitchTimeout();
      break;
    case GLITCH:
      checkForIdleTimeout();
      break;
    case SLEEP:
      break;
    case MUSIC:
      musicStateLoop();
      break;
    case TUNER:
      tunerLoop();
      checkForIdleTimeout();
      break;
    default:
      checkForIdleTimeout();
      break;
  }

  if (playWav1.isPlaying() == false) {
    Serial.println("Start playing 1");
    playWav1.play("ETNL.WAV");
    delay(10); // wait for library to parse WAV info
  }
  if (playWav2.isPlaying() == false) {
    Serial.println("Start playing 2");
    playWav2.play("LUCIUS.WAV");
    delay(10); // wait for library to parse WAV info
  }
  if (playWav3.isPlaying() == false) {
    Serial.println("Start playing 3");
    playWav3.play("NUMBERS.WAV");
    delay(10); // wait for library to parse WAV info
  }
}


void inputPollingLoop() {

  // Check for button presses ---------------------------------------------
  sleepButton.update();
  wakeButton.update();
  hourButton.update();
  minuteButton.update();
  snoozButton.update();

  if (sleepButton.fell()) {
    buttonPressed(SLEEP_BUTTON);
  } else if (wakeButton.fell()) {
    buttonPressed(WAKE_BUTTON);
  } else if (hourButton.fell()) {
    buttonPressed(HOUR_BUTTON);
  } else if (minuteButton.fell()) {
    buttonPressed(MINUTE_BUTTON);
  } else if (snoozButton.fell()) {
    buttonPressed(SNOOZ_BUTTON);
  } else if (snoozButton.isPressed()) {
    if (dynamicRefreshUpdateTimer > dynamicRefreshUpdateRate) {
      idleTimeoutTimer = 0;
      dynamicRefreshUpdateTimer = 0;
      display.setRefreshRate((snoozButton.currentDuration() * 2) + 1);
      //Serial.println(snoozButton.currentDuration());
    }
  } else if (snoozButton.released()) {
    idleTimeoutTimer = 0;
  }


  // Check The Mode Switch -----------------------------------------------------
  if (modeSwitchPollTimer >= modeSwitchPollRate) {
    int switchVal = analogRead(MODE_SWITCH);
    ToggleSwitchState newSwitchState;

    if (switchVal < 50) {
      newSwitchState = OFF_SWITCH_STATE;
    } else if (switchVal < 300) {
      newSwitchState = ON_SWITCH_STATE;
    } else if (switchVal < 600) {
      newSwitchState = MUSIC_SWITCH_STATE;
    } else {
      newSwitchState = ALARM_SWITCH_STATE;
    }

    if (newSwitchState != toggleSwitchState) {
      toggleSwitchState = newSwitchState;
    }

    // Serial.print("Switch val: ");
    // Serial.print(toggleSwitchState);
    // Serial.println();
    modeSwitchPollTimer = 0;
  }
  

#ifdef ARDUINO_TEENSY41
  // Poll Tuner Encoder -------------------------------------------------------
  // Only change state if value has changed by a determined amount
  long newTunerPosition = tunerEncoder.read();
  if (newTunerPosition != tunerPosition) {

    clockState = MUSIC;
    idleTimeoutTime = tunerModeTimeoutTime;
    idleTimeoutTimer = 0;

    tunerPosition = newTunerPosition;
    if(tunerPosition < 0) {
      tunerPosition = 0;
      tunerEncoder.write(0);
    }
    
    float gainNoise;
    float gainRadio;

    if (tunerPosition >= 0 && tunerPosition < 9) {
      //noise down, radio station 1 up
      gainNoise = (map(tunerPosition, 0, 8, 100, 0)/100.0);
      gainRadio = (map(tunerPosition, 0, 8, 0, 100)/100.0);
      mixer1.gain(0, gainNoise);
      mixer1.gain(1, gainRadio);
    }
    else if (tunerPosition >= 9 && tunerPosition < 17) {
      //noise up, radio station 1 down
      gainNoise = (map(tunerPosition-9, 0, 7, 0, 100)/100.0);
      gainRadio = (map(tunerPosition-9, 0, 7, 100, 0)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(1, gainRadio);
    }
    else if (tunerPosition >= 17 && tunerPosition < 26) {
      //noise down, radio station 2 up
      gainNoise = (map(tunerPosition-17, 0, 8, 100, 0)/100.0);
      gainRadio = (map(tunerPosition-17, 0, 8, 0, 100)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(2, gainRadio);

    }
    else if (tunerPosition >= 26 && tunerPosition < 33) {
      //noise up, radio station 2 down
      gainNoise = (map(tunerPosition-26, 0, 6, 0, 100)/100.0);
      gainRadio = (map(tunerPosition-26, 0, 6, 100, 0)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(2, gainRadio);
    }
    else if (tunerPosition >= 33 && tunerPosition < 42) {
      //noise down, radio station 3 up
      gainNoise = (map(tunerPosition-33, 0, 8, 100, 0)/100.0);
      gainRadio = (map(tunerPosition-33, 0, 8, 0, 100)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(3, gainRadio);

    }
    else if (tunerPosition >= 42 && tunerPosition <= 50) {
      //noise up, radio station 3 down
      gainNoise = (map(tunerPosition-42, 0, 7, 0, 100)/100.0);
      gainRadio = (map(tunerPosition-42, 0, 7, 100, 0)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(3, gainRadio);

    }
    else {
      mixer1.gain(0, 0);
      mixer1.gain(1, 0);
      mixer1.gain(2, 0);
      mixer1.gain(3, 0);
    }

    Serial.print("gainNoise: ");
    Serial.print(gainNoise);
    Serial.print("  gainRadio: ");
    Serial.println(gainRadio);


    Serial.println(tunerPosition);

    // mixer1.gain(0, 0.1*tunerPosition);
    // mixer1.gain(2, 1.0 - 0.6*tunerPosition);
  }
#endif

}


void tunerLoop() {
  display.displayInt(tunerPosition);
}


void musicStateLoop() {
#ifdef ARDUINO_TEENSY41
  if (clockState == MUSIC) {
    if (vuMeterRefreshTimer >= vuMeterRefreshRate) {

      vuMeterRefreshTimer = 0;

      if (peak1.available()) {
        //uint8_t peakScaled = peak1.read() * 70.0;
        uint8_t peakScaled = peak1.read()*15;
        display.setVuMeter(peakScaled);
      }
    }
  }
#endif
}


void buttonPressed(ClockInput pressedButton) {

  idleTimeoutTimer = 0;

  if (pressedButton == SLEEP_BUTTON) {
    clockState = SLEEP;
#ifdef ARDUINO_TEENSY41
    //playWav1.play("GLADIATORS.WAV");
#endif
    display.playSleepAnimation();
  } else if (pressedButton == WAKE_BUTTON) {
    clockState = WAKE;
#ifdef ARDUINO_TEENSY41
    //playWav1.play("DEMNTEDCIRCUS.WAV");
    //playWav1.play("LONGDJENT.WAV");
//playWav1.play("ALARM2.WAV");
#endif
    display.playWakeAnimation();
  } else if (pressedButton == HOUR_BUTTON) {
    clockState = HOUR;
#ifdef ARDUINO_TEENSY41
    //playWav1.play("3SECSAWSWEEP.WAV");
#endif
    display.playHourAnimation();
  } else if (pressedButton == MINUTE_BUTTON) {
    clockState = MINUTE;
#ifdef ARDUINO_TEENSY41
    //playWav1.play("3SECSINESWEEP.WAV");
#endif
    //display.scrollString("Gordon KILLED JARED");
    //display.scrollString("yo");
    display.playMinuteAnimation();
  } else if (pressedButton == SNOOZ_BUTTON) {
    clockState = SNOOZ;
#ifdef ARDUINO_TEENSY41
    //playWav1.stop();
#endif
    display.playSnoozAnimation();
  }

#ifdef ARDUINO_TEENSY41
  Serial.print("audio usage:  ");
  Serial.println(AudioProcessorUsage());
#endif
}


void checkForIdleTimeout() {
  if (idleTimeoutTimer >= idleTimeoutTime) {
    clockState = IDLE;
    glitchTimer = 0;
    idleTimeoutTime = defaultIdleTimeoutTime;
    display.playIdleAnimation();

    mixer1.gain(0, 0.0);
    mixer1.gain(1, 0.0);
  }
}


void checkForGlitchTimeout() {
  if (glitchTimer >= glitchTimeoutTime) {
    clockState = GLITCH;
    glitchTimer = 0;
    idleTimeoutTimer = 0;
    idleTimeoutTime = defaultIdleTimeoutTime;
    display.playGlitchAnimation();
  }
}
