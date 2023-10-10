/*
#ifndef ClockAudio_h
#define ClockAudio_h

#include "Arduino.h"
#include "ClockGlobals.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "AudioSampleCashregister.h"

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used


class ClockAudio {
public:
    // Constructors
  ClockAudio(); // Default constructor

  void setup();
  void loop();

  void play();
   
  void playFile(const char* filename);
private:

  AudioPlaySdWav           playWav1;
  // // Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
  // AudioOutputI2S2          audioOutput;
  // //AudioOutputSPDIF       audioOutput;
  // //AudioOutputAnalog      audioOutput;
  // //On Teensy LC, use this for the Teensy Audio Shield:
  // //AudioOutputI2Sslave    audioOutput;

  AudioPlayMemory sound0;
  AudioOutputI2S2 headphones;

  AudioConnection c1(sound0, 0, headphones, 0);
  AudioConnection c2(sound0, 0, headphones, 1);

  // AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
  // AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);

  //void playFile(const char* filename);
    
};

#endif

*/