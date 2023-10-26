#include "ClockDisplay.h"
#include "ClockGlobals.h"

#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <Bounce.h>

#include "autonet.h"
#include <NativeEthernet.h>     // use if you have a Wiznet w5100 Ethernet shield
#include <NativeEthernetUdp.h>  // use if you have a Wiznet w5100 Ethernet shield

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11  // not actually used
#define SDCARD_SCK_PIN 13   // not actually used

// Network Stuff
//This device's unique IP and Mac Address
IPAddress ip(10, 32, 17, 62);
byte mac[] = { 0x81, 0x1C, 0xBD, 0xC3, 0x33, 0x67 };

//define outgoing IP and port to send OSC messages to a server
IPAddress remote_ip(10, 32, 16, 10);  //Cathy's temporary test server
const unsigned int remote_port = 6666;
//stuff we need if we're not on 10.42.16.*
byte ddns[] = { 10, 32, 17, 1 };
byte *gateway = ddns;
byte subnet[] = { 255, 255, 240, 0 };

char ard_name[50] = "d8b94bbb-3b48-421a-b449-51bf9d4dfb64";  //UUID
int localPort = 7777;                                        // autonet expects to see incoming OSC on port 7777
EthernetUDP Udp;

//create your Autonet object, pass in the IP and MAC address from above, plus custom stuff for our new show controller
Autonet autonet(ip, mac, ard_name, ddns, subnet, remote_ip, remote_port, 1);
int is_connected = 1;          // 0 = broken connection, 1 = connected
#define RETRY_TIME 5000        // how often to retry broken UDP connections
unsigned long retrytimer = 0;  // timer for the UDP retries

// Audio Stuff
AudioPlaySdWav playWav1;
AudioPlayMemory sound0;
//AudioAnalyzeRMS peak1;
AudioAnalyzePeak peak1;
AudioMixer4 mixer1;
AudioOutputI2S headphones;
AudioConnection c1(playWav1, 0, mixer1, 0);
AudioConnection c2(sound0, 0, mixer1, 1);
AudioConnection c3(mixer1, peak1);
AudioConnection c4(mixer1, 0, headphones, 0);
AudioConnection c5(mixer1, 0, headphones, 1);

// Peripheral Devies
ClockDisplay display;
Encoder tunerEncoder(tunerEncoderPin1, tunerEncoderPin2);

// State holders
ClockState clockState = IDLE;
ToggleSwitchState toggleSwitchState = OFF_SWITCH_STATE;

// Buttons
Bounce sleepButton = Bounce(SLEEP_BUTTON, 100);
Bounce wakeButton = Bounce(WAKE_BUTTON, 100);  // 5 ms debounce time
Bounce hourButton = Bounce(HOUR_BUTTON, 100);
Bounce minuteButton = Bounce(MINUTE_BUTTON, 100);
Bounce snoozButton = Bounce(SNOOZ_BUTTON, 100);

// Timers
elapsedMillis idleTimeoutTimer;
elapsedMillis vuMeterRefreshTimer;
elapsedMillis glitchTimer;
elapsedMillis ledToggleTimer;
elapsedMillis ledOnTimeTimer;

// Consts
const int defaultIdleTimeoutTime = 5000;
long unsigned int tunerModeTimeoutTime = 3000;
//long unsigned int glitchTimeoutTime = 600000; // 10 min
long unsigned int glitchTimeoutTime = 240000; // 4 min
//long unsigned int glitchTimeoutTime = 60000; // 1 min
//long unsigned int glitchTimeoutTime = 30000; // 30 sec
long unsigned const int ledOnTime = 10;
const int vuMeterRefreshRate = 24;

// Variables
long unsigned int idleTimeoutTime = defaultIdleTimeoutTime;
long tunerPosition  = 0;
long unsigned int ledToggleTime = 0;
bool ledToggle = false;


void setup() {

  // Serial
  Serial.begin(9600);
  delay(1000);

  // Button Setup
  pinMode(SLEEP_BUTTON, INPUT_PULLUP);
  pinMode(WAKE_BUTTON, INPUT_PULLUP);
  pinMode(HOUR_BUTTON, INPUT_PULLUP);
  pinMode(MINUTE_BUTTON, INPUT_PULLUP);
  pinMode(SNOOZ_BUTTON, INPUT_PULLUP);

  pinMode(tunerLedPinLeft, OUTPUT);
  pinMode(tunerLedPinRight, OUTPUT);

  // Ethernet Setup
  Ethernet.begin(mac, ip, ddns, gateway, subnet);
  //Ethernet.begin(mac, ip); // use this if you don't need gateway and subnet
  Udp.begin(localPort);
  autonet.setup(Udp);

  // Display Setup
  display.setup();
  display.setTime(12, 43);
  display.playIdleAnimation();

  // SD Card Setup
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // Audio Setup
  AudioMemory(10);
  mixer1.gain(0, 0.1);
  mixer1.gain(1, 0.1);
}


void loop() {

  autonet.loop();
  display.loop();
  inputPollingLoop();
  networkLoop();

  switch (clockState) {
    case IDLE:
      checkForGlitchTimeout();
      break;
    case GLITCH:
      checkForIdleTimeout();
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
}


void networkLoop() {
}


void inputPollingLoop() {
  // Check for button presses
  sleepButton.update();
  wakeButton.update();
  hourButton.update();
  minuteButton.update();
  snoozButton.update();

  if (sleepButton.fallingEdge()) {
    buttonPressed(SLEEP_BUTTON);
  } else if (wakeButton.fallingEdge()) {
    buttonPressed(WAKE_BUTTON);
  } else if (hourButton.fallingEdge()) {
    buttonPressed(HOUR_BUTTON);
  } else if (minuteButton.fallingEdge()) {
    buttonPressed(MINUTE_BUTTON);
  } else if (snoozButton.fallingEdge()) {
    buttonPressed(SNOOZ_BUTTON);
  }

  // Poll tuning encoder
  // Only change state if value has changed by a determined amount
  long newTunerPosition = tunerEncoder.read();
  if (newTunerPosition != tunerPosition) {
    
    clockState = TUNER;
    idleTimeoutTime = tunerModeTimeoutTime;
    idleTimeoutTimer = 0;

    tunerPosition = newTunerPosition;
    Serial.println(newTunerPosition);
  }
}


void tunerLoop() {
  display.displayInt(tunerPosition);


}


void musicStateLoop() {
  if (clockState == MUSIC) {
    if (vuMeterRefreshTimer >= vuMeterRefreshRate) {

      vuMeterRefreshTimer = 0;

      if (peak1.available()) {
        uint8_t peakScaled = peak1.read() * 70.0;
        display.setVuMeter(peakScaled);
      }
    }
  }
}


void buttonPressed(ClockInput pressedButton) {

  idleTimeoutTimer = 0;

  if (pressedButton == SLEEP_BUTTON) {
    clockState = SLEEP;
    playWav1.play("GLADIATORS.WAV");
    display.playSleepAnimation();
  } 
  else if (pressedButton == WAKE_BUTTON) {
    //clockState = WAKE;
    clockState = MUSIC;
    vuMeterRefreshTimer = 0;
    //playWav1.play("DEMNTEDCIRCUS.WAV");
    playWav1.play("LONGDJENT.WAV");
    //playWav1.play("ALARM2.WAV");
    display.playWakeAnimation();
  } 
  else if (pressedButton == HOUR_BUTTON) {
    clockState = HOUR;
    playWav1.play("3SECSAWSWEEP.WAV");
    display.playHourAnimation();
  } 
  else if (pressedButton == MINUTE_BUTTON) {
    clockState = MINUTE;
    playWav1.play("3SECSINESWEEP.WAV");
    idleTimeoutTime = 20000;
    //display.scrollString("Gordon KILLED JARED");
    display.scrollString("yo");
  } 
  else if (pressedButton == SNOOZ_BUTTON) {
    clockState = SNOOZ;
    playWav1.stop();
    display.playSnoozAnimation();
  }

  Serial.print("audio usage:  ");
  Serial.println(AudioProcessorUsage());
}


void checkForIdleTimeout() {
  if (idleTimeoutTimer >= idleTimeoutTime) {
    clockState = IDLE;
    glitchTimer = 0;
    idleTimeoutTime = defaultIdleTimeoutTime;
    display.playIdleAnimation();
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
