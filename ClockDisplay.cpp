#include "ClockDisplay.h"

ClockDisplay::ClockDisplay() {
	_matrix = Adafruit_7segment();

  _frameIndex = 0;
  _currentAnimation = animation7;
}

ClockDisplay::ClockDisplay(int hours, int minutes) {
  _hours = hours;
  _minutes = minutes;
}

void ClockDisplay::begin() {
  _matrix.begin(0x70);
}

void ClockDisplay::loop() {
  _currentMillis = millis();

  if (_currentMillis - _previousMillis > _currentAnimation[_frameIndex].holdTime ){//&& anmiationRepetitions < numRepeats) {

    _matrix.clear();

    for (int digitIndex = 0; digitIndex < 5; ++digitIndex) {
      _matrix.writeDigitRaw(digitIndex, _currentAnimation[_frameIndex].digitMasks[digitIndex] & 0b01111111);
      // if (anmiationRepetitions < 5) {
      //   matrix.writeDigitRaw(digitIndex, random(0, 256));
      // }
      // else if (anmiationRepetitions < 10) {
      //   matrix.writeDigitRaw(digitIndex, random(0, 256) & 0b11111110);
      // }
      // else if (anmiationRepetitions < 15) {
      //   matrix.writeDigitRaw(digitIndex, random(0, 256) & 0b11011100);
      // }
      // else if (anmiationRepetitions < 20) {
      //   matrix.writeDigitRaw(digitIndex, random(0, 256) & 0b10011100);
      // }
      // else if (anmiationRepetitions < 25) {
      //   matrix.writeDigitRaw(digitIndex, random(0, 256) & 0b10001000);
      // }
      // else if (anmiationRepetitions < 30) {
      //   matrix.writeDigitRaw(digitIndex, random(0, 256) & 0b10000000);
      // }
    }
    _matrix.setBrightness(_currentAnimation[_frameIndex].brightness);
    _matrix.writeDisplay();

    // Serial.print("Frame: ");
    // Serial.println(frameIndex);

    if ( (_currentAnimation[_frameIndex].controlBits & LAST_FRAME) != 0 ) { // if this is the last frame of the animation
      _frameIndex = 0;
      _anmiationRepetitions++;
      // Serial.print("Repetitions: ");
      // Serial.println(anmiationRepetitions);
    }
    else {
      _frameIndex++;
    }

    _previousMillis = _currentMillis;
  }
}

void ClockDisplay::playSleepAnimation() {
  _frameIndex = 0;
  _currentAnimation = animation7;
  Serial.println("sleep");
}

void ClockDisplay::playWakeAnimation() {
  _frameIndex = 0;
  _currentAnimation = animation2;
  Serial.println("wake");
}

void ClockDisplay::playHourAnimation() {
  _frameIndex = 0;
  _currentAnimation = animation3;
  Serial.println("houir");
}

void ClockDisplay::playMinuteAnimation() {
  _frameIndex = 0;
  _currentAnimation = animation4;
  Serial.println("minute");
}

void ClockDisplay::playSnoozAnimation() {
  _frameIndex = 0;
  _currentAnimation = animation6;
  Serial.println("snooz");
}


void ClockDisplay::setClock(int hours, int minutes) {
  _hours = hours;
  _minutes = minutes;
}


