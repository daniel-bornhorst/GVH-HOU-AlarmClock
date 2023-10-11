#include "ClockDisplay.h"



ClockDisplay::ClockDisplay() {

  _frameTimer = 0;

  _frameIndex = 0;
  _currentAnimation = animation4;
  _scrollStepRate = 250;
}

void ClockDisplay::setup() {
  // Setup display
  _matrix.begin();
  _matrix.displayOn();
  _matrix.brightness(DEFAULT_BRIGHTNESS);
  _matrix.displayClear();
  _matrix.blink(0);
  _matrix.cacheOn();

  _frameTimer = 0;
  _stringScrollIndex = 0;
}

void ClockDisplay::loop() {

  switch (_displayState) {
    case DISPLAY_OFF:
      return;
    case SCRIPTED_ANIMATION:
      playScriptedAnimation();
      break;
    case BUFFER_SCROLL:
      scrollStringBuffer();
      break;
    case VU_METER:
      //setVuMeter();
      break;
  }
}

void ClockDisplay::playScriptedAnimation() {

  if (_currentAnimation == NULL) { return; }

  if (_frameTimer > _currentAnimation[_frameIndex].holdTime) {

    // Check for digit randomization mask
    if (_currentAnimation[_frameIndex].controlBits & RANDOMIZE_DISPLAY) {
      randomizeDisplayBuffer(_currentAnimation[_frameIndex].digitMasks);
    }
    else {
      loadDisplayBuffer(_currentAnimation[_frameIndex].digitMasks);
    }
    
    _matrix.displayRaw( _displayBuffer, 
                        _currentAnimation[_frameIndex].controlBits & DISPLAY_COLON);

    _matrix.brightness(_currentAnimation[_frameIndex].brightness);

    if ( (_currentAnimation[_frameIndex].controlBits & LAST_FRAME) != 0 ) { // if this is the last frame of the animation
      _frameIndex = 0;
      _anmiationRepetitions++;
    }
    else {
      _frameIndex++;
    }

    _frameTimer = 0;
  }
}

void ClockDisplay::scrollStringBuffer() {

  if (_frameTimer > _scrollStepRate) {

    char shitBuff[stringBuffer.length()];

    String subShit = stringBuffer.substring(_stringScrollIndex);
    subShit.toCharArray(shitBuff, stringBuffer.length());

    _matrix.displayChars(shitBuff);
    Serial.println(_stringScrollIndex);
    if (_stringScrollIndex >= stringBuffer.length()) {
      _stringScrollIndex = 0;
    }
    else {
      _stringScrollIndex++;
    }
    _frameTimer = 0;
  }

}

void ClockDisplay::playIdleAnimation() {
  _displayState = SCRIPTED_ANIMATION;
  _frameIndex = 0;
  _frameTimer = _currentAnimation[0].holdTime; // Jump straight into the IDLE animation (dont wait for the first frame timeout time)
  _anmiationRepetitions = 0;
  _currentAnimation = animation4;
  Serial.println("IDLE");
}

void ClockDisplay::playAtmAnimation() {
  _displayState = SCRIPTED_ANIMATION;
  _frameIndex = 0;
  _frameTimer = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation7;
  Serial.println("ATM");
}

void ClockDisplay::playVendeAnimation() {
  _displayState = SCRIPTED_ANIMATION;
  _frameIndex = 0;
  _frameTimer = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation2;
  Serial.println("VendE");
}

void ClockDisplay::playPianoAnimation() {
  _displayState = SCRIPTED_ANIMATION;
  _frameIndex = 0;
  _frameTimer = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation3;
  Serial.println("piano");
}

void ClockDisplay::playTurntableAnimation() {
  _displayState = SCRIPTED_ANIMATION;
  _frameIndex = 0;
  _frameTimer = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation9;
  // _displayState = BUFFER_SCROLL;
  // _frameTimer = _scrollStepRate;
  // _currentAnimation = NULL;
  //_stringScrollIndex = 0;
  Serial.println("turntable");
}

void ClockDisplay::playSnoozAnimation() {
  _displayState = SCRIPTED_ANIMATION;
  _frameIndex = 0;
  _frameTimer = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = animation6;
  Serial.println("snooz");
}


void ClockDisplay::setVuMeter(uint8_t level) {
  _displayState = VU_METER;
}


void ClockDisplay::setClock(int hours, int minutes) {
  _hours = hours;
  _minutes = minutes;
}

void ClockDisplay::randomizeDisplayBuffer(const uint8_t* frame) {
  for (int i = 0; i < 4; ++i) {
    _displayBuffer[i] = random(0, 256) | frame[i];
  }
}

void ClockDisplay::loadDisplayBuffer(const uint8_t* frame) {
  for (int i = 0; i < 4; ++i) {
    _displayBuffer[i] = frame[i];
  }
}


