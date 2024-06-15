//#define PROTO 1

#include "ClockGlobals.h"
#include "ClockDisplay.h"
#include "elapsedMillis.h"
#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>
#include "GordonSample.h"
#include "GlitchSample.h"

#include <Encoder.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Tlv493d.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11  // not actually used
#define SDCARD_SCK_PIN 13   // not actually used


typedef struct {
  uint8_t pixelData[8][4];
  unsigned long holdTime; // in milliseconds
  uint8_t controlBits;
} PixelFrame;


// Audio Stuff
AudioPlaySdWav playWav1;
AudioPlaySdWav playWav2;
AudioPlaySdWav playWav3;
AudioSynthNoisePink noise1;
AudioPlayMemory playMem1;
//AudioAnalyzeRMS peak1;
AudioAnalyzePeak peak1;
AudioMixer4 mixer1;
AudioMixer4 mixer2;
AudioEffectFade fade1;
AudioSynthWaveform       waveform1;
AudioEffectMultiply      multiply1;
AudioFilterStateVariable filter1;
AudioOutputI2S2 headphones;

// AudioSynthToneSweep      tonesweep1;
// AudioSynthWaveformSineModulated sine_fm1;

AudioConnection c1(noise1, 0, mixer1, 0);
AudioConnection c2(playWav1, 0, mixer1, 1);
AudioConnection c3(playWav2, 0, mixer1, 2);
AudioConnection c4(playWav3, 0, mixer1, 3);
AudioConnection c5(mixer1, fade1);
AudioConnection c6(fade1, 0, mixer2, 0);
AudioConnection c7(playMem1, 0, mixer2, 1);
AudioConnection c8(mixer1, peak1);
AudioConnection c9(mixer2, 0, filter1, 0);
AudioConnection c10(filter1, 2, headphones, 0);
AudioConnection c11(filter1, 2, headphones, 1);
AudioConnection c12(waveform1, 0, multiply1, 1);
AudioConnection c13(playMem1, 0, multiply1, 0);
// AudioConnection C14(tonesweep1, sine_fm1);
// AudioConnection C15(sine_fm1, 0, mixer2, 2);


// Peripheral Devies
ClockDisplay display;


// State holders
ClockState clockState = IDLE;
ToggleSwitchState toggleSwitchState = NO_SWITCH_STATE;


// Magnetic Tuner Position Sensor
Tlv493d magneticSensor = Tlv493d();
static float cookedY = 0;
float rawY = 0;


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
elapsedMillis modeSwitchPollTimer;
elapsedMillis tuningWheelPollTimer;
elapsedMillis radioTimeoutTimer;


// Consts
const unsigned int fiveSeconds = 5000;
const unsigned int tenSeconds = 10000;
const unsigned int fifteenSeconds = 15000;
const unsigned int thirtySeconds = 30000;
const unsigned int oneMinute = 60000;
const unsigned int fourMinutes = 240000;
const unsigned int tenMinutes = 600000;
const unsigned int thirtyMinutes = 1800000;

const int defaultIdleTimeoutTime = fiveSeconds;
const int defaultGlitchTimeoutTime = fourMinutes;
long unsigned int radioTimeoutTime = oneMinute;
long unsigned int glitchTimeoutTime = fourMinutes; // 4 min
const int vuMeterRefreshRate = 42;
long unsigned const int modeSwitchPollRate = 5;
long unsigned const int tuningWheelPollRate = 10;


// Variables
long unsigned int idleTimeoutTime = defaultIdleTimeoutTime;
long tunerPosition = 0;
ClockState stateHistory[10];
bool snoozDirectionToggle = true;
bool radioTimedOut = false;
uint8_t snoozQueueIndex = 0;
int stateChangeCount = 0;
uint8_t stateChangeSinceGlitch = 0;
bool firstSetModeSwitch = true;


void setup() {

  // Serial
  Serial.begin(9600);
  delay(200);

  DEBUG_PRINTLN("Clock boot sequence initiated");
  magneticSensor.begin();
  magneticSensor.setAccessMode(magneticSensor.FASTMODE);
  magneticSensor.disableTemp();

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

  pinMode(ON_SWITCH, INPUT_PULLUP);
  pinMode(OFF_SWITCH, INPUT_PULLUP);
  pinMode(RADIO_SWITCH, INPUT_PULLUP);
  pinMode(ALARM_SWITCH, INPUT_PULLUP);

  // Display Setup
  display.setup();
  display.setTime(7, 6);
  display.playIdleAnimation();

  // SD Card Setup
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Unable to access the SD card");
    delay(500);
  }

  pixelSetup();

  delay(200);
  
  // Audio Setup
  AudioMemory(10);

  mixer1.gain(0, 0.0); // Tuning Noise
  mixer1.gain(1, 0.0); // ETNL
  mixer1.gain(2, 0.0); // Lucius
  mixer1.gain(3, 0.0); // Number Stations
  mixer2.gain(0, 1.0); // Radio Mix
  mixer2.gain(1, 1.0); // Gordon Sample
  //mixer2.gain(2, 0.0); // FM Sweep

  // sine_fm1.amplitude(1.0);
  // sine_fm1.frequency(400);

  filter1.frequency(1000.0);

  fade1.fadeOut(10);

  playWav1.play("ETNL.WAV");
  delay(200);
  
  playWav2.play("LUCIUS.WAV");
  delay(200);

  playWav3.play("NUMBERS.WAV");
  delay(200);

  noise1.amplitude(0.5);

  waveform1.begin(0.2, 1000, WAVEFORM_SINE);

  delay(200);

  // Simple Mode Check
  // Start in simple mode unless these three buttons are held during boot
  // if ( !(sleepButton.isPressed() && minuteButton.isPressed() && snoozButton.isPressed()) ) {
  //   setState(SIMPLE_MODE);
  //   display.displayTime();
  //   Serial.println("WE IN SIMP MODE YO!");
  // }

  DEBUG_PRINTLN("Clock boot sequence complete");
}

void loop() {

  if (clockState != SIMPLE_MODE) {
    display.loop();
    pixelLoop();
    inputPollingLoop();
    audioLoop();
  }

  switch (clockState) {
    case SIMPLE_MODE:
      break;
    case IDLE:
      checkForGlitchTimeout();
      break;
    case GLITCH:
      if (!display.isAnimationRunning()) setState(IDLE);
      break;
    case SLEEP:
      if (!display.isAnimationRunning()) setState(IDLE);
      break;
    case WAKE:
      if (!display.isAnimationRunning()) setState(IDLE);
      break;
    case HOUR:
      if (!display.isAnimationRunning()) setState(IDLE);
      break;
    case MINUTE:
      if (!display.isAnimationRunning()) setState(IDLE);
      break;
    case SNOOZ:
      if (!display.isAnimationRunning() && !isPixelSequenceRunning()) setState(IDLE);
      //checkForIdleTimeout();
      break;
    case ON_MODE:
      checkForIdleTimeout();
      break;
    case OFF_MODE:
      checkForGlitchTimeout();
      break;
    case RADIO_MODE:
      radioStateLoop();
      break;
    case ALARM_MODE:
      checkForGlitchTimeout();
      break;
    case GORDON:
      checkForGlitchTimeout();
      break;
    case FILE_NOT_FOUND:
    default:
      checkForIdleTimeout();
      break;
  }
}


void inputPollingLoop() {
  buttonLoop();
  modeSwitchLoop();
  #ifndef PROTO
  radioTuningWheelLoop();
  #endif
}


void buttonLoop() {
  // Update Button States
  sleepButton.update();
  wakeButton.update();
  hourButton.update();
  minuteButton.update();
  snoozButton.update();

  // Check For Button Presses
  if (sleepButton.fell()) {
    setState(SLEEP);
  } else if (wakeButton.fell()) {
    setState(WAKE);
  } else if (hourButton.fell()) {
    setState(HOUR);
  } else if (minuteButton.fell()) {
    setState(MINUTE);
  } else if (snoozButton.fell()) {
    setState(SNOOZ);
  }
}


void modeSwitchLoop() {


  // Check The Mode Switch at an interval
  if (modeSwitchPollTimer >= modeSwitchPollRate) {
    ToggleSwitchState newSwitchState = NO_SWITCH_STATE;

    if (digitalRead(ON_SWITCH) == 0) {
      newSwitchState = ON_SWITCH_STATE;
    }
    else if (digitalRead(OFF_SWITCH) == 0) {
      newSwitchState = OFF_SWITCH_STATE;
    }
    else if (digitalRead(RADIO_SWITCH) == 0) {
      newSwitchState = RADIO_SWITCH_STATE;
    }
    else if (digitalRead(ALARM_SWITCH) == 0) {
      newSwitchState = ALARM_SWITCH_STATE;
    }

    if (toggleSwitchState != newSwitchState) {
      toggleSwitchState = newSwitchState;

      if (firstSetModeSwitch) { // Read the switch position, do not set the state first time throught this loop.
        modeSwitchPollTimer = 0;
        firstSetModeSwitch = false;
        return;
      }

      switch(toggleSwitchState) {
        case ON_SWITCH_STATE: setState(ON_MODE); break;
        case OFF_SWITCH_STATE: setState(OFF_MODE); break;
        case RADIO_SWITCH_STATE: setState(RADIO_MODE); break;
        case ALARM_SWITCH_STATE: setState(ALARM_MODE); break;
        default: setState(OFF_MODE); break;
      }
    }

    modeSwitchPollTimer = 0;
  }
}


void radioTuningWheelLoop() {

  if (clockState != RADIO_MODE) {
    return;
  }

  if ( tuningWheelPollTimer < tuningWheelPollRate ) {
    return;
  }

  // Only change state if value has changed by a determined amount
  long newTunerPosition;
  int sensorError = magneticSensor.updateData();
  if (sensorError > 0) {
    DEBUG_PRINT("Sensor Error: ");
    DEBUG_PRINTLN(sensorError);
  }

  rawY = magneticSensor.getAzimuth();
  cookedY = 0.85 * cookedY + 0.15 * rawY;
  newTunerPosition = round((cookedY*10)+25);

  if (newTunerPosition != tunerPosition) {


    tunerPosition = newTunerPosition;
    if(tunerPosition < 0) {
      tunerPosition = 0;
    }
    else if (tunerPosition > 50) {
      tunerPosition = 50;
    }
    
    float gainNoise;
    float gainRadio;

    if (tunerPosition >= 0 && tunerPosition < 9) {
      //noise down, radio station 1 up
      gainNoise = (map(tunerPosition, 0, 8, 100, 0)/100.0);
      gainRadio = (map(tunerPosition, 0, 8, 0, 100)/100.0);
      mixer1.gain(0, gainNoise);
      mixer1.gain(1, gainRadio);

      mixer1.gain(2, 0);
      mixer1.gain(3, 0);
    }
    else if (tunerPosition >= 9 && tunerPosition < 17) {
      //noise up, radio station 1 down
      gainNoise = (map(tunerPosition-9, 0, 7, 0, 100)/100.0);
      gainRadio = (map(tunerPosition-9, 0, 7, 100, 0)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(1, gainRadio);

      mixer1.gain(2, 0);
      mixer1.gain(3, 0);
    }
    else if (tunerPosition >= 17 && tunerPosition < 26) {
      //noise down, radio station 2 up
      gainNoise = (map(tunerPosition-17, 0, 8, 100, 10)/100.0);
      gainRadio = (map(tunerPosition-17, 0, 8, 0, 90)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(2, gainRadio);

      mixer1.gain(1, 0);
      mixer1.gain(3, 0);

    }
    else if (tunerPosition >= 26 && tunerPosition < 33) {
      //noise up, radio station 2 down
      gainNoise = (map(tunerPosition-26, 0, 6, 0, 100)/100.0);
      gainRadio = (map(tunerPosition-26, 0, 6, 100, 0)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(2, gainRadio);

      mixer1.gain(1, 0);
      mixer1.gain(3, 0);
    }
    else if (tunerPosition >= 33 && tunerPosition < 42) {
      //noise down, radio station 3 up
      gainNoise = (map(tunerPosition-33, 0, 8, 100, 10)/100.0);
      gainRadio = (map(tunerPosition-33, 0, 8, 0, 90)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(3, gainRadio);

      mixer1.gain(1, 0);
      mixer1.gain(2, 0);

    }
    else if (tunerPosition >= 42 && tunerPosition <= 50) {
      //noise up, radio station 3 down
      gainNoise = (map(tunerPosition-42, 0, 7, 0, 100)/100.0);
      gainRadio = (map(tunerPosition-42, 0, 7, 100, 0)/100.0);

      mixer1.gain(0, gainNoise);
      mixer1.gain(3, gainRadio);

      mixer1.gain(1, 0);
      mixer1.gain(2, 0);

    }
    else {
      // Mute all channels if encoder is out of range
      mixer1.gain(0, 0);
      mixer1.gain(1, 0);
      mixer1.gain(2, 0);
      mixer1.gain(3, 0);
    }

    // Serial.print("gainNoise: ");
    // Serial.print(gainNoise);
    // Serial.print("  gainRadio: ");
    // Serial.println(gainRadio);
    // Serial.println(tunerPosition);
  }

  tuningWheelPollTimer = 0;
}


void audioLoop() {
  // Restart Audio Loops if they have reached the end of the file
  if (playWav1.isPlaying() == false) {
    DEBUG_PRINTLN("Start playing ETNL.WAV");
    playWav1.play("ETNL.WAV");
    delay(10); // wait for library to parse WAV info
  }
  if (playWav2.isPlaying() == false) {
    DEBUG_PRINTLN("Start playing LUCIUS.WAV");
    playWav2.play("LUCIUS.WAV");
    delay(10); // wait for library to parse WAV info
  }
  if (playWav3.isPlaying() == false) {
    DEBUG_PRINTLN("Start playing NUMBERS.WAV");
    playWav3.play("NUMBERS.WAV");
    delay(10); // wait for library to parse WAV info
  }
}


void radioStateLoop() {
  if (clockState != RADIO_MODE) {
    return;
  }

  
  if (radioTimeoutTimer >= radioTimeoutTime) {
    radioTimedOut = true;
    setState(IDLE);
    return;
  }


  if (vuMeterRefreshTimer >= vuMeterRefreshRate) {

    vuMeterRefreshTimer = 0;

    if (peak1.available()) {
      //uint8_t peakScaled = peak1.read() * 70.0;
      uint8_t peakScaled = peak1.read()*14;
      display.setVuMeter(peakScaled);
      setPixelVUMeter(peakScaled);

    }
  }
}


void checkForIdleTimeout() {
  if (idleTimeoutTimer >= idleTimeoutTime) {
    setState(IDLE);
  }
}


void checkForGlitchTimeout() {
  if (glitchTimer >= glitchTimeoutTime) {
    setState(GLITCH);
  }
}


void setState(ClockState newClockState) {

  // if (newClockState == clockState) {
  //   return;
  // }
  if (clockState == RADIO_MODE && newClockState != RADIO_MODE) {
    muteRadio();
    clearPixels();
  }

  if (clockState == GLITCH && newClockState != GLITCH) {
    playMem1.stop();
    stopPixelSequencer();
    display.clear();
    //doReboot();
  }

  // stopPixelSequencer();
  // display.clear();

  clockState = newClockState;

  String stateString;
  switch (clockState) {
    case SIMPLE_MODE:
      stateString = "Simple";
      break;
    case IDLE:
      startIdle();
      stateString = "Idle";
      break;
    case GLITCH:
      startGlitch();
      stateString = "Glitch";
      break;
    case SLEEP:
      startSleep();
      stateString = "Sleep";
      break;
    case WAKE:
      startWake();
      stateString = "Wake";
      break;
    case HOUR:
      startHour();
      stateString = "Hour";
      break;
    case MINUTE:
      startMinute();
      stateString = "Minute";
      break;
    case SNOOZ:
      startSnooz();
      stateString = "Snooz";
      break;
    case ON_MODE:
      startOnMode();
      stateString = "On";
      break;
    case OFF_MODE:
      startOffMode();
      stateString = "Off";
      break;
    case RADIO_MODE:
      startRadioMode();
      stateString = "Radio";
      break;
    case ALARM_MODE:
      startAlarmMode();
      stateString = "Alarm";
      break;
    case GORDON:
      startGordon();
      stateString = "Gordon";
      break;
    case FILE_NOT_FOUND:
    default:
      startFileNotFound();
      stateString = "404 - File Not Found";
      break;
  }
  DEBUG_PRINT("Clock State = ");
  DEBUG_PRINTLN(stateString);

  updateStateHistory();
  if (checkForGordonCodeMatch()) {
    setState(GORDON);
  }
  else if (checkForRebootCodeMatch()) {
    doReboot();
  }
}


void startIdle() {
  //mixer2.gain(2, 0.0);
  glitchTimer = 0;
  idleTimeoutTime = defaultIdleTimeoutTime;
  display.playIdleAnimation();
  muteRadio();
  stopPixelSequencer();
}


void startGlitch() {
  glitchTimer = 0;
  idleTimeoutTimer = 0;
  idleTimeoutTime = defaultIdleTimeoutTime;
  glitchTimeoutTime = defaultGlitchTimeoutTime;
  display.playGlitchAnimation();
  triggerGlitchFlash();
  mixer2.gain(1, 3); // Glitch Sample
  playMem1.play(GlitchSample);
}


void startSleep() {
  stopPixelSequencer();
  display.clear();

  display.playSleepAnimation();
}


void startWake() {
  stopPixelSequencer();
  display.clear();

  display.playWakeAnimation();
}

void startHour() {
  stopPixelSequencer();
  display.clear();

  display.playHourAnimation();
}


void startMinute() {
  stopPixelSequencer();
  display.clear();

  display.playMinuteAnimation();
  // mixer2.gain(2, .1);
  // tonesweep1.play(1, 100, 1000, 15);
}


void startSnooz() {
  stopPixelSequencer();
  display.clear();

  idleTimeoutTimer = 0;
  idleTimeoutTime = fiveSeconds;

  if (snoozDirectionToggle) {
    triggerReverseRedStreak();
    display.playReverseRedStreak();
  }
  else {
    triggerRedStreak();
    display.playRedStreak();
  }
  // if (snoozQueueIndex >= 4) snoozQueueIndex = 0;
  // display.playSnoozQueueAnimation(snoozQueueIndex++);

  snoozDirectionToggle = !snoozDirectionToggle;
}


void startOnMode() {
  idleTimeoutTimer = 0;
  idleTimeoutTime = fiveSeconds;
  //triggerGreenStrobe();
  triggerWhiteStrobe();
  display.playOnStrobeAnimation();
}


void startOffMode() {
  glitchTimer = 0;
  glitchTimeoutTime = fiveSeconds;
  stopPixelSequencer();
  display.clear();
  display.scrollString("   byE               ");
  //display.scrollString("drink reCIpe -   3 gender fluid     press 2 - hydro bang");
  // display.displayString("AT[]m");
  // display.displayString("A@#m");
}


void startRadioMode() {
  radioTimeoutTimer = 0;
  radioTimedOut = false;
  unmuteRadio();
  stopPixelSequencer();
}


void startAlarmMode() {
  glitchTimer = 0;
  glitchTimeoutTime = tenSeconds;

  triggerAlertLights();
  display.displayString("aLrt  ");
  display.blink(true);
}


void startGordon() {
  glitchTimer = 0;
  idleTimeoutTimer = 0;
  idleTimeoutTime = defaultIdleTimeoutTime;
  glitchTimeoutTime = tenSeconds;

  mixer2.gain(1, 1.0); // Gordon Sample
  playMem1.play(gordonLikesThatSample);
  triggerPoliceLights();
  display.scrollString("gOrdon LIKES tHAt");
}


void startFileNotFound() {
  glitchTimer = 0;
  idleTimeoutTimer = 0;
  idleTimeoutTime = defaultIdleTimeoutTime;
  glitchTimeoutTime = tenSeconds;
  display.displayInt(404);
  display.blink(true);
}


void updateStateHistory() {

  stateChangeCount++;

  for (int i = 9; i >= 1; --i) {
    stateHistory[i] = stateHistory[i-1];
  }

  stateHistory[0] = clockState;

  #ifdef DEBUG
  DEBUG_PRINT("StateHistory: ");

  for (int i = 0; i < 10; ++i) {
    DEBUG_PRINT(stateHistory[i]);

    if (i < 9) {
      DEBUG_PRINT(", ");
    }
  }

  DEBUG_PRINTLN("");
  #endif
}


// Returns true if a pattern matches
bool checkForGordonCodeMatch() {
  for (int i = 0; i < 8; ++i) {
    switch (i) {
      case 0: if(stateHistory[0] == SLEEP  ) { break; } else { return false; }
      case 1: if(stateHistory[1] == WAKE   ) { break; } else { return false; }
      case 2: if(stateHistory[2] == SLEEP  ) { break; } else { return false; }
      case 3: if(stateHistory[3] == SLEEP  ) { break; } else { return false; }
      case 4: if(stateHistory[4] == MINUTE ) { break; } else { return false; }
      case 5: if(stateHistory[5] == MINUTE ) { break; } else { return false; }
      case 6: if(stateHistory[6] == HOUR   ) { break; } else { return false; }
      case 7: if(stateHistory[7] == MINUTE ) { break; } else { return false; }
      default: 
        return false;
    }
  }
  DEBUG_PRINTLN("-- Gordon Likes That Match --");
  return true;
}


bool checkForRebootCodeMatch() {
  for (int i = 0; i < 6; ++i) {
    switch (i) {
      case 0: if(stateHistory[0] == OFF_MODE) { break; } else { return false; }
      case 1: if(stateHistory[1] == MINUTE  ) { break; } else { return false; }
      case 2: if(stateHistory[2] == HOUR    ) { break; } else { return false; }
      case 3: if(stateHistory[3] == WAKE    ) { break; } else { return false; }
      case 4: if(stateHistory[4] == SLEEP   ) { break; } else { return false; }
      case 5: if(stateHistory[5] == ON_MODE ) { break; } else { return false; }
      default: 
        return false;
    }
  }
  DEBUG_PRINTLN("-- Reboot Match --");
  return true;
}


void muteRadio() {
  fade1.fadeOut(200);
}


void unmuteRadio() {
  fade1.fadeIn(200);
}


void doReboot() {
  SCB_AIRCR = 0x05FA0004;
}
