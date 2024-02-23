#define _DEBUG

#include "ClockDisplay.h"
#include "ClockGlobals.h"
#include "elapsedMillis.h"
#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>
#include "GordonSample.h"

#ifdef ARDUINO_TEENSY41  //---------------------------------------------------------------------
#define ENCODER_DO_NOT_USE_INTERRUPTS

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
  uint8_t pixelData[8][4];  // Segment index 2 is the colon ':' on this display
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
AudioFilterStateVariable filter1;
AudioOutputI2S2 headphones;
AudioConnection c1(noise1, 0, mixer1, 0);
AudioConnection c2(playWav1, 0, mixer1, 1);
AudioConnection c3(playWav2, 0, mixer1, 2);
AudioConnection c4(playWav3, 0, mixer1, 3);
AudioConnection c5(mixer1, fade1);
AudioConnection c6(fade1, 0, mixer2, 0);
AudioConnection c7(playMem1, 0, mixer2, 1);
AudioConnection c8(mixer2, peak1);
AudioConnection c9(mixer2, 0, filter1, 0);
AudioConnection c10(filter1, 2, headphones, 0);
AudioConnection c11(filter1, 2, headphones, 1);

#endif  //---------------------------------------------------------------------


// Peripheral Devies
ClockDisplay display;


// State holders
ClockState clockState = IDLE;
ToggleSwitchState toggleSwitchState = OFF_SWITCH_STATE;


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


// Variables
long unsigned int idleTimeoutTime = defaultIdleTimeoutTime;
long tunerPosition = 0;
bool radioOn = false;
ClockState stateHistory[10];


void setup() {

  // Serial
  Serial.begin(9600);
  delay(1000);

  magneticSensor.begin();

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

  pinMode(tunerLedPinRight, INPUT_PULLUP);

  // SD Card Setup
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Unable to access the SD card");
    delay(500);
  }

  pixelSetup();

  delay(1000);
  
  // Audio Setup
  AudioMemory(10);

  mixer1.gain(0, 0.0);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0.0);
  mixer1.gain(3, 0.0);
  mixer2.gain(0, 1.0);
  mixer2.gain(1, 1.0);

  filter1.frequency(1000.0);

  fade1.fadeOut(10);

  playWav1.play("ETNL.WAV");
  delay(1000);
  
  playWav2.play("LUCIUS.WAV");
  delay(1000);

  playWav3.play("NUMBERS.WAV");
  delay(1000);

  noise1.amplitude(0.5);

#endif

  delay(1000);

  // Simple Mode Check
  // Start in simple mode unless these three buttons are held during boot
  // if ( !(sleepButton.isPressed() && minuteButton.isPressed() && snoozButton.isPressed()) ) {
  //   setState(SIMPLE_MODE);
  //   display.displayTime();
  //   Serial.println("WE IN SIMP MODE YO!");
  // }
}

void loop() {

  if (clockState != SIMPLE_MODE) {
    display.loop();
    inputPollingLoop();
    audioLoop();
    pixelLoop();
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
    case WAKE:
      break;
    case HOUR:
      break;
    case MINUTE:
      break;
    case SNOOZ:
      break;
    case RADIO:
      radioStateLoop();
      break;
    default:
      checkForIdleTimeout();
      break;
  }
}


void inputPollingLoop() {

  buttonLoop();
  modeSwitchLoop();


#ifdef ARDUINO_TEENSY41
  tunerSwitchLoop();
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
    buttonPressed(SLEEP_BUTTON);
  } else if (wakeButton.fell()) {
    buttonPressed(WAKE_BUTTON);
  } else if (hourButton.fell()) {
    buttonPressed(HOUR_BUTTON);
  } else if (minuteButton.fell()) {
    buttonPressed(MINUTE_BUTTON);
  } else if (snoozButton.fell()) {
    buttonPressed(SNOOZ_BUTTON);
  }
}


void modeSwitchLoop() {
  // Check The Mode Switch at an interval
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
}

// FOR TESTING PURPOSES
void tunerSwitchLoop() {
  if (digitalRead(tunerLedPinRight) == 0) {
      
      // glitchTimer = glitchTimeoutTime+1;
      // checkForGlitchTimeout();
      if (radioOn == true) {
        // Force the clock into idle mode
        // Which also turns radio off
        idleTimeoutTimer = idleTimeoutTime+1;
        checkForIdleTimeout();
      }

      radioOn = false;
      
  }
  else {
    if (radioOn == false) {
      // Force Radio on 
      tunerPosition -= 1;
      setState(RADIO);
    }
    radioOn = true;
  }
}


void radioTuningWheelLoop() {

  if (radioOn == false) {
    //muteRadio();         //TESTING
    fade1.fadeOut(500);
    return;
  }
  else {
    //mixer1.gain(1, 0.5); //TESTING
    fade1.fadeIn(500);
  }

  // Only change state if value has changed by a determined amount
  long newTunerPosition;
  magneticSensor.updateData();
  delay(10);
  rawY = magneticSensor.getAzimuth();
  cookedY = 0.85 * cookedY + 0.15 * rawY;
  newTunerPosition = round((cookedY*10)+25);
  // Serial.println(newTunerPosition);

  // FOR TESTING
  return;
  // -----------

  if (newTunerPosition != tunerPosition) {

    //setState(RADIO);
    idleTimeoutTime = tunerModeTimeoutTime;
    idleTimeoutTimer = 0;

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
      gainNoise = (map(tunerPosition-17, 0, 8, 100, 10)/100.0);
      gainRadio = (map(tunerPosition-17, 0, 8, 0, 90)/100.0);

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
      gainNoise = (map(tunerPosition-33, 0, 8, 100, 10)/100.0);
      gainRadio = (map(tunerPosition-33, 0, 8, 0, 90)/100.0);

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
}


void audioLoop() {
#ifdef ARDUINO_TEENSY41
  // Restart Audio Loops if they have reached the end of the file
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
#endif
}


void radioStateLoop() {
#ifdef ARDUINO_TEENSY41
  if (clockState == RADIO) {
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
    display.playSleepAnimation();
    triggerBlueNightRider();
    setState(SLEEP);
  } 
  else if (pressedButton == WAKE_BUTTON) {
    display.playWakeAnimation();
    setState(WAKE);
  } 
  else if (pressedButton == HOUR_BUTTON) {
    display.playHourAnimation();
    triggerRedStreak();
    setState(HOUR);
  } 
  else if (pressedButton == MINUTE_BUTTON) {
    //display.scrollString("JARED IS ALIVE AND WELL NOTHING TO SEE HERE");
    //display.scrollString("yo");
    display.playMinuteAnimation();
    setState(MINUTE);
  } 
  else if (pressedButton == SNOOZ_BUTTON) {
    //display.playSnoozAnimation();
    display.scrollString("drink reCIpe -   3 gender fluid     press 2 - hydro bang");
    // display.displayString("AT[]m");
    // display.displayString("A@#m");
    setState(SNOOZ);
  }
}


void checkForIdleTimeout() {
  if (idleTimeoutTimer >= idleTimeoutTime) {
    setState(IDLE);
    glitchTimer = 0;
    idleTimeoutTime = defaultIdleTimeoutTime;
    display.playIdleAnimation();

    muteRadio();
    stopPixelSequencer();
  }
}


void checkForGlitchTimeout() {
  if (glitchTimer >= glitchTimeoutTime) {
    setState(GLITCH);
    glitchTimer = 0;
    idleTimeoutTimer = 0;
    idleTimeoutTime = defaultIdleTimeoutTime;
    display.playGlitchAnimation();
  }
}


void setState(ClockState newClockState) {
  clockState = newClockState;

  String stateString;
  switch (clockState) {
    case SIMPLE_MODE:
      stateString = "Simple";
      break;
    case IDLE:
      stateString = "Idle";
      break;
    case GLITCH:
      stateString = "Glitch";
      break;
    case SLEEP:
      stateString = "Sleep";
      break;
    case WAKE:
      stateString = "Wake";
      break;
    case HOUR:
      stateString = "Hour";
      break;
    case MINUTE:
      stateString = "Minute";
      break;
    case SNOOZ:
      stateString = "Snooz";
      break;
    case RADIO:
      stateString = "Radio";
      break;
    default:
      break;
  }
  Serial.print("Clock State = ");
  Serial.print(stateString);
  Serial.println();
  updateStateHistory();
  if (checkForStateMatch()) {
    triggerPoliceLights();
  }
}


void updateStateHistory() {
  for (int i = 9; i >= 1; --i) {
    stateHistory[i] = stateHistory[i-1];
  }

  stateHistory[0] = clockState;

  Serial.print("StateHistory: ");

  for (int i = 0; i < 10; ++i) {
    Serial.print(stateHistory[i]);

    if (i < 9) {
      Serial.print(", ");
    }
  }

  Serial.println();
}


// Returns true if a pattern matches
bool checkForStateMatch() {
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
  playMem1.play(gordonLikesThatSample);
  Serial.println("MATCH!!");
  return true;
}


void muteRadio() {
  fade1.fadeOut(500);
  // #ifdef ARDUINO_TEENSY41
  //   mixer1.gain(0, 0.0);
  //   mixer1.gain(1, 0.0);
  //   mixer1.gain(2, 0.0);
  //   mixer1.gain(3, 0.0);
  // #endif
}
