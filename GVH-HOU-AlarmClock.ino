#include <Bounce.h>

#include "ClockDisplay.h"
#include "ClockGlobals.h"

#include "autonet.h"
#include <NativeEthernet.h> // use if you have a Wiznet w5100 Ethernet shield 
#include <NativeEthernetUdp.h> // use if you have a Wiznet w5100 Ethernet shield 

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// Network Stuff
//This device's unique IP and Mac Address
IPAddress ip(10,32,17,62);
byte mac[] = {0x81, 0x1C, 0xBD, 0xC3, 0x33, 0x67};

//define outgoing IP and port to send OSC messages to a server
IPAddress remote_ip(10, 32, 16, 10); //Cathy's temporary test server
const unsigned int remote_port = 6666;
//stuff we need if we're not on 10.42.16.*
byte ddns[] =               { 10, 32, 17, 1 };
byte *gateway =             ddns;
byte subnet[] =             { 255, 255, 240, 0 };

char ard_name[50] = "d8b94bbb-3b48-421a-b449-51bf9d4dfb64"; //UUID
int localPort = 7777; // autonet expects to see incoming OSC on port 7777
EthernetUDP Udp;

//create your Autonet object, pass in the IP and MAC address from above, plus custom stuff for our new show controller  
Autonet autonet(ip, mac, ard_name, ddns, subnet, remote_ip, remote_port, 1);
int is_connected = 1; // 0 = broken connection, 1 = connected
#define RETRY_TIME 5000  // how often to retry broken UDP connections
unsigned long retrytimer = 0; // timer for the UDP retries

// Audio Stuff
AudioPlaySdWav playWav1;
AudioPlayMemory sound0;
//AudioAnalyzeRMS peak1;
AudioAnalyzePeak peak1;        
AudioMixer4 mixer1;
AudioOutputI2S2 headphones;
AudioConnection c1(playWav1, 0, mixer1, 0);
AudioConnection c2(sound0, 0, mixer1, 1);
AudioConnection c3(mixer1, peak1);
AudioConnection c4(mixer1, 0, headphones, 0);
AudioConnection c5(mixer1, 0, headphones, 1);

// Peripheral Devies
ClockDisplay display;

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
elapsedMillis vuMeterRefreshTimer;

// Consts
const int idleTimeoutTime = 5000;
const int vuMeterRefreshRate = 24;


void setup() {

  Serial.begin(9600);
  delay(1000);

  AudioMemory(10);

  pinMode(ATM_BUTTON, INPUT_PULLUP);
  pinMode(VENDE_BUTTON, INPUT_PULLUP);
  pinMode(PIANO_BUTTON, INPUT_PULLUP);
  pinMode(TURNTABLE_BUTTON, INPUT_PULLUP);
  pinMode(SNOOZ_BUTTON, INPUT_PULLUP);

  Ethernet.begin(mac, ip, ddns, gateway, subnet);
  //Ethernet.begin(mac, ip); // use this if you don't need gateway and subnet
  Udp.begin(localPort);
  autonet.setup(Udp);

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

  mixer1.gain(0, 0.1);
  mixer1.gain(1, 0.1);
}


void loop() {

  autonet.loop();
  display.loop();
  buttonLoop();

  switch (clockState) {
    case IDLE:
      break;
    case MUSIC:
      musicStateLoop();
      break;
    default:
      checkForIdleTimeout();
      break;
  }
}


void checkForIdleTimeout() {
  if (idleTimeout >= idleTimeoutTime) {
      clockState = IDLE;
      display.playIdleAnimation();
  }
}


void musicStateLoop() {
  if (clockState == MUSIC) {
    if (vuMeterRefreshTimer >= vuMeterRefreshRate) {
      // float peakVal = peak1.read(); // float from 0.0 to 1.0
      // int peakScaled = peak1.read() * 8.0;
      // display.setVuMeter(peakScaled);

      vuMeterRefreshTimer = 0;

      if (peak1.available()) {
        uint8_t peakScaled = peak1.read() * 70.0;
        display.setVuMeter(peakScaled);
        // Serial.print("Peak Val: ");
        // Serial.print(peakScaled);
        // Serial.println();
        // vuMeterRefreshTimer = 0;
        // int monoPeak = peak1.read() * 30.0;
        // Serial.print("|");
        // for (int cnt=0; cnt<monoPeak; cnt++) {
        //   Serial.print(">");
        // }
        // Serial.println();
      }
    }
  }
}


void networkLoop() {

}


void buttonLoop() {
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
}


void buttonPressed(ClockInput pressedButton) {
  
  idleTimeout = 0;

  if (pressedButton == ATM_BUTTON) {
    clockState = ATM;
    //playWav1.play("GLADIATORS.WAV");
    display.playAtmAnimation();
  }
  else if (pressedButton == VENDE_BUTTON) {
    clockState = VENDE;
    //clockState = MUSIC;
    vuMeterRefreshTimer = 0;
    //playWav1.play("DEMNTEDCIRCUS.WAV");
    //playWav1.play("LONGDJENT.WAV");
    //playWav1.play("ALARM2.WAV");
    display.playVendeAnimation();
  }
  else if (pressedButton == PIANO_BUTTON) {
    clockState = PIANO;
    //playWav1.play("3SECSAWSWEEP.WAV");
    display.playPianoAnimation();
  }
  else if (pressedButton == TURNTABLE_BUTTON) {
    clockState = TURNTABLE;
    //playWav1.play("3SECSQUARESWEEP.WAV");
    display.playTurntableAnimation();
  }
  else if (pressedButton == SNOOZ_BUTTON) {
    clockState = SNOOZ;
    //playWav1.stop();
    display.playSnoozAnimation();
  }

  Serial.print("audio usage:  ");
  Serial.println(AudioProcessorUsage());
}
