// This is the teensy audio setup for Gordon's Alarm Clock
// You can import this into the Teensy Audio System Design Tool at https://www.pjrc.com/teensy/gui/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=71.33333969116211,309.33330726623535
AudioPlaySdWav           playSdWav1;     //xy=83.33332824707031,274.3333377838135
AudioPlayMemory          playMem1;       //xy=138.99998474121094,377
AudioSynthWaveform       waveform1;      //xy=240.99999237060547,428.99999809265137
AudioSynthToneSweep      tonesweep1;     //xy=244.33334350585938,515.3334226608276
AudioMixer4              mixer1;         //xy=248.3333511352539,287.33333587646484
AudioEffectMultiply      multiply1;      //xy=391,415
AudioSynthWaveformSineModulated sine_fm1;       //xy=393.33331298828125,515.3333740234375
AudioAnalyzePeak         peak1;          //xy=399.99999618530273,238.00003623962402
AudioEffectFade          fade1;          //xy=401.99999618530273,284.99999809265137
AudioSynthWaveformModulated waveformMod1;   //xy=401.33336639404297,769.3333778381348
AudioMixer4              mixer2;         //xy=591.0000076293945,506.9999933242798
AudioFilterStateVariable filter1;        //xy=725.3332328796387,513.3333330154419
AudioOutputI2S           i2s1;           //xy=882.3332672119141,524.3333005905151
AudioConnection          patchCord1(noise1, 0, mixer1, 1);
AudioConnection          patchCord2(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord3(playMem1, 0, multiply1, 0);
AudioConnection          patchCord4(waveform1, 0, multiply1, 1);
AudioConnection          patchCord5(tonesweep1, sine_fm1);
AudioConnection          patchCord6(mixer1, fade1);
AudioConnection          patchCord7(mixer1, peak1);
AudioConnection          patchCord8(multiply1, 0, mixer2, 1);
AudioConnection          patchCord9(sine_fm1, 0, mixer2, 2);
AudioConnection          patchCord10(fade1, 0, mixer2, 0);
AudioConnection          patchCord11(mixer2, 0, filter1, 0);
AudioConnection          patchCord12(filter1, 2, i2s1, 0);
AudioConnection          patchCord13(filter1, 2, i2s1, 1);
// GUItool: end automatically generated code
