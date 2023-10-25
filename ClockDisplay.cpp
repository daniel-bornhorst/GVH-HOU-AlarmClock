#include "ClockDisplay.h"



ClockDisplay::ClockDisplay() {

  _frameTimer = 0;

  _frameIndex = 0;
  _currentAnimation = sevenOhSixThrob;
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
      scriptedAnimationLoop();
      break;
    case BUFFER_SCROLL:
      scrollLoop();
      break;
    case VU_METER:
      break;
    case MANUAL_STATIC:
      break;
  }
}


void ClockDisplay::playIdleAnimation() {
  loadAnimation(sevenOhSixThrob);
  Serial.println("IDLE");
}


void ClockDisplay::playGlitchAnimation() {
  loadAnimation(sevenOhSixGlitch);
  Serial.println("gLiTcH");
}


void ClockDisplay::playAtmAnimation() {
  loadAnimation(sevenOhSixGlitch);
  Serial.println("ATM");
}


void ClockDisplay::playVendeAnimation() {
  loadAnimation(circleAnimation1);
  Serial.println("VendE");
}


void ClockDisplay::playPianoAnimation() {
  loadAnimation(circleAnimation1);
  Serial.println("piano");
}


void ClockDisplay::playTurntableAnimation() {
  loadAnimation(noiseAnimation1);
  Serial.println("turntable");
}


void ClockDisplay::playSnoozAnimation() {
  loadAnimation(circleAnimation1);
  Serial.println("snooz");
}


void ClockDisplay::scrollString(String buff) {
  setStringBuffer(buff);
  setDisplayState(BUFFER_SCROLL);
}


void ClockDisplay::displayInt(int displayVal) {
  setDisplayState(MANUAL_STATIC);
  _matrix.displayColon(0);
  _matrix.displayInt(displayVal);
}


void ClockDisplay::displayString(String displayString) {
  setDisplayState(MANUAL_STATIC);

  char chars[displayString.length()];
  displayString.toCharArray(chars, displayString.length());

  _matrix.displayChars(chars);
}


void ClockDisplay::setTime(int hours, int minutes) {
  _hours = hours;
  _minutes = minutes;
}


void ClockDisplay::setStringBuffer(String buff) {
  _stringBuffer = buff;
}


void ClockDisplay::setStringBuffer(int intValBuff) {
  _stringBuffer = String(intValBuff);
}


void ClockDisplay::setDisplayState(DisplayState newState) {
  if (_displayState != newState) {
    _matrix.brightness(DEFAULT_BRIGHTNESS);
    _displayState = newState;
  }
}


void ClockDisplay::clear() {
  _matrix.displayClear();
}


void ClockDisplay::randomizeDisplayBuffer(const uint8_t* frame) {
  for (int i = 0; i < 4; ++i) {
    _displayBuffer[i] = random(0, 256) & frame[i];
  }
}


void ClockDisplay::loadDisplayBuffer(const uint8_t* frame) {
  for (int i = 0; i < 4; ++i) {
    _displayBuffer[i] = frame[i];
  }
}


void ClockDisplay::loadAnimation(const AnimationFrame* newAnimation) {
  setDisplayState(SCRIPTED_ANIMATION);
  _frameIndex = 0;
  _frameTimer = 0;
  _anmiationRepetitions = 0;
  _currentAnimation = newAnimation;
  scriptedAnimationLoop(true); // true indicates that we've loaded a new animation
}


void ClockDisplay::scriptedAnimationLoop(bool newAnimation) {

  if (_currentAnimation == NULL) { return; }

  if (_frameAdvanced || newAnimation) {
    // Check for digit randomization mask
    if (_currentAnimation[_frameIndex].controlBits & RANDOMIZE_DISPLAY) {
      randomizeDisplayBuffer(_currentAnimation[_frameIndex].digitMasks);
    } else {
      loadDisplayBuffer(_currentAnimation[_frameIndex].digitMasks);
    }

    _matrix.displayRaw(_displayBuffer,
                       _currentAnimation[_frameIndex].controlBits & DISPLAY_COLON);

    _matrix.brightness(_currentAnimation[_frameIndex].brightness);
  }

  if (_frameTimer > _currentAnimation[_frameIndex].holdTime) {
    if ((_currentAnimation[_frameIndex].controlBits & LAST_FRAME) != 0) {  // if this is the last frame of the animation
      _frameIndex = 0;
      _anmiationRepetitions++;
    } else {
      _frameIndex++;
    }

    _frameAdvanced = true;
    _frameTimer = 0;
  }
  else {
    _frameAdvanced = false;
  }
}

void ClockDisplay::scrollLoop() {

  if (_frameTimer > _scrollStepRate) {

    char charBuff[_stringBuffer.length()];

    String subString = _stringBuffer.substring(_stringScrollIndex);
    subString.toCharArray(charBuff, _stringBuffer.length());

    _matrix.displayChars(charBuff);
    if (_stringScrollIndex >= _stringBuffer.length()) {
      _stringScrollIndex = 0;
    } else {
      _stringScrollIndex++;
    }
    _frameTimer = 0;
  }
}


void ClockDisplay::setVuMeter(uint8_t level) {
  setDisplayState(VU_METER);
  loadDisplayBuffer(vuLevelDisplayStates[level]);
  _matrix.displayRaw(_displayBuffer);
}