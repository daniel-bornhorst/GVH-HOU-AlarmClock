#include "ClockDisplay.h"



ClockDisplay::ClockDisplay() {

  Wire.setClock(1000000);

  _frameIndex = 0;
  _currentAnimation = animation4;
}

void ClockDisplay::begin() {
  // Setup display
  _matrix.begin();
  _matrix.displayOn();
  _matrix.brightness(DEFAULT_BRIGHTNESS);
  _matrix.displayClear();
  _matrix.blink(0);
  _matrix.cacheOn();
}

void ClockDisplay::loop() {
  _currentMillis = millis();

  if (_currentAnimation == NULL) { return; }
  if (_currentMillis - _previousMillis > _currentAnimation[_frameIndex].holdTime ){

    //_matrix.displayClear();
    //random(0, 256)

    _matrix.displayRaw( _currentAnimation[_frameIndex].digitMasks, 
                        _currentAnimation[_frameIndex].controlBits & DISPLAY_COLON);

    _matrix.brightness(_currentAnimation[_frameIndex].brightness);

    if ( (_currentAnimation[_frameIndex].controlBits & LAST_FRAME) != 0 ) { // if this is the last frame of the animation
      _frameIndex = 0;
      _anmiationRepetitions++;
    }
    else {
      _frameIndex++;
    }

    _previousMillis = _currentMillis;
  }
}

void ClockDisplay::playIdleAnimation() {
  _frameIndex = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation4;
  Serial.println("IDLE");
}

void ClockDisplay::playAtmAnimation() {
  _frameIndex = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation7;
  Serial.println("ATM");
}

void ClockDisplay::playVendeAnimation() {
  _frameIndex = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation2;
  Serial.println("VendE");
}

void ClockDisplay::playPianoAnimation() {
  _frameIndex = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation3;
  Serial.println("piano");
}

void ClockDisplay::playTurntableAnimation() {
  _frameIndex = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation5;
  Serial.println("turntable");
}

void ClockDisplay::playSnoozAnimation() {
  _frameIndex = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation6;
  Serial.println("snooz");
}


void ClockDisplay::setClock(int hours, int minutes) {
  _hours = hours;
  _minutes = minutes;
}


