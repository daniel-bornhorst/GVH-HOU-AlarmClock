/*
#include "ClockAudio.h"

ClockAudio::ClockAudio()  {
  
}

void ClockAudio::setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
  // AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
  AudioConnection c1(sound0, 0, headphones, 0);
  AudioConnection c2(sound0, 0, headphones, 1);
}

void ClockAudio::loop() {

}

void ClockAudio::play() {
  Serial.println("Hey Yall");
  sound0.play(AudioSampleCashregister);
  Serial.println(sound0.isPlaying());
}

void ClockAudio::playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(25);

  Serial.println(playWav1.isPlaying());
  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    // float vol = analogRead(15);
    // vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
}

*/