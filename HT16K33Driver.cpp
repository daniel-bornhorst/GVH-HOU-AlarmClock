//
//    FILE: HT16K33.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.3.9
//    DATE: 2019-02-07
// PURPOSE: Arduino Library for HT16K33 4x7segment display
//     URL: https://github.com/RobTillaart/HT16K33


#include "HT16K33Driver.h"


//  Commands
#define HT16K33_ON              0x21  //  0 = off   1 = on
#define HT16K33_STANDBY         0x20  //  bit xxxxxxx0


//  bit pattern 1000 0xxy
//  y    =  display on / off
//  xx   =  00=off     01=2Hz     10 = 1Hz     11 = 0.5Hz
#define HT16K33_DISPLAYON       0x81
#define HT16K33_DISPLAYOFF      0x80
#define HT16K33_BLINKON0_5HZ    0x87
#define HT16K33_BLINKON1HZ      0x85
#define HT16K33_BLINKON2HZ      0x83
#define HT16K33_BLINKOFF        0x81


//  bit pattern 1110 xxxx
//  xxxx    =  0000 .. 1111 (0 - F)
#define HT16K33_BRIGHTNESS      0xE0

//
//  HEX codes 7 segment
//
//          01
//      ---------
//     /         /
//  20 /         / 02
//     /         /
//     /--- 40 --/
//     /         /
//  10 /         / 04
//     /         /
//      ---------
//         08
//
static const uint8_t charmap[] = {  //  TODO PROGMEM = slower?

  0x3F,   //  0
  0x06,   //  1
  0x5B,   //  2
  0x4F,   //  3
  0x66,   //  4
  0x6D,   //  5
  0x7D,   //  6
  0x07,   //  7
  0x7F,   //  8
  0x6F,   //  9
  0x77,   //  A
  0x7C,   //  B
  0x39,   //  C
  0x5E,   //  D
  0x79,   //  E
  0x71,   //  F
  0x00,   //  space
  0x40,   //  minus
  0x61,   //  TOP_C
  0x63,   //  degree °
};


////////////////////////////////////////////////////
//
//  CONSTRUCTOR
//
#ifdef ARDUINO_TEENSY41
HT16K33Driver::HT16K33Driver(const uint8_t address)
{
  _address = address;
}
#else
HT16K33Driver::HT16K33Driver(const uint8_t address, TwoWire *wire)
{
  _address = address;
  _wire = wire;
}
#endif



#if defined (ESP8266) || defined(ESP32)
bool HT16K33Driver::begin(uint8_t sda, uint8_t scl)
{
  if ((sda < 255) && (scl < 255))
  {
    begin(sda, scl);
  } else {
    begin();
  }
  if (! isConnected()) return false;
  reset();
  return true;
}
#endif


bool HT16K33Driver::begin()
{
  #ifdef ARDUINO_TEENSY41
  master.begin(master_frequency);
  #else
  _wire->begin();
  #endif
  if (! isConnected()) return false;
  reset();
  return true;
}

#ifdef ARDUINO_TEENSY41
bool HT16K33Driver::isConnected()
{
  beginTransmission(_address);
  return (0 == endTransmission());
}
#else
bool HT16K33Driver::isConnected()
{
  _wire->beginTransmission(_address);
  return (0 == _wire->endTransmission());
}
#endif



void HT16K33Driver::reset()
{
  displayOn();
  displayClear();
  setDigits(1);
  clearCache();
  brightness(8);
}


////////////////////////////////////////////////////
//
//  CACHE
//
void HT16K33Driver::clearCache()
{
  for (uint8_t i = 0; i < 5; i++)
  {
    _displayCache[i] = HT16K33_NONE;
  }
}


void HT16K33Driver::cacheOn()
{
  _cache = true;
}



void HT16K33Driver::cacheOff()
{
  _cache = false;
}


void HT16K33Driver::refresh()
{
  _refresh();
}


////////////////////////////////////////////////////
//
//  DISPLAY
//
void HT16K33Driver::displayOn()
{
  writeCmd(HT16K33_ON);
  writeCmd(HT16K33_DISPLAYON);
  brightness(_bright);
}


void HT16K33Driver::displayOff()
{
  writeCmd(HT16K33_DISPLAYOFF);
  writeCmd(HT16K33_STANDBY);
}


void HT16K33Driver::blink(uint8_t value)
{
  if (value > 0x03) value = 0x00;
  writeCmd(HT16K33_BLINKOFF | (value << 1) );
}


void HT16K33Driver::brightness(uint8_t value)
{
  if (value == _bright) return;
  _bright = value;
  if (_bright > 0x0F) _bright = 0x0F;
  writeCmd(HT16K33_BRIGHTNESS | _bright);
}


uint8_t HT16K33Driver::getBrightness()
{
  return _bright;
}


void HT16K33Driver::setDigits(uint8_t value)
{
  _digits = value > 4 ? 4 : value;
}


void HT16K33Driver::suppressLeadingZeroPlaces(uint8_t value)
{
  _digits = value > 4 ? 0 : 4 - value;
}


//////////////////////////////////////////
//
//  display functions
//
void HT16K33Driver::displayClear()
{
  uint8_t x[4] = {HT16K33_SPACE, HT16K33_SPACE, HT16K33_SPACE, HT16K33_SPACE};
  display(x);
  displayColon(false);
}


//  DIV10 & DIV100 optimize?
bool HT16K33Driver::displayInt(int n)
{
  bool inRange = ((-1000 < n) && (n < 10000));
  uint8_t x[4], h, l;
  bool neg = (n < 0);
  if (neg) n = -n;
  h = n / 100;
  l = n - h * 100;
  x[0] = h / 10;
  x[1] = h - x[0] * 10;
  x[2] = l / 10;
  x[3] = l - x[2] * 10;

  if (neg)
  {
    if (_digits >= 3)
    {
      x[0] = HT16K33_MINUS;
    }
    else
    {
      int i = 0;
      for (i = 0; i < (4 - _digits); i++)
      {
        if (x[i] != 0) break;
        x[i] = HT16K33_SPACE;
      }
      x[i-1] = HT16K33_MINUS;
    }
  }
  display(x);
  return inRange;
}


//  0000..FFFF
bool HT16K33Driver::displayHex(uint16_t n)
{
  uint8_t x[4], h, l;
  h = n >> 8;
  l = n & 0xFF;
  x[3] = l & 0x0F;
  x[2] = l >> 4;
  x[1] = h & 0x0F;;
  x[0] = h >> 4;
  display(x);
  return true;
}


//  00.00 .. 99.99
bool HT16K33Driver::displayDate(uint8_t left, uint8_t right, bool lz)
{
  bool inRange = ((left < 100) && (right < 100));
  uint8_t x[4];
  x[0] = left / 10;
  x[1] = left - x[0] * 10;
  if (!lz && (x[0] == 0)) x[0] = HT16K33_SPACE;

  x[2] = right / 10;
  x[3] = right - x[2] * 10;
  display(x, 1);
  displayColon(false);
  return inRange;
}


//  00:00 .. 99:99
bool HT16K33Driver::displayTime(uint8_t left, uint8_t right, bool colon, bool lz)
{
  bool inRange = ((left < 100) && (right < 100));
  uint8_t x[4];
  x[0] = left / 10;
  x[1] = left - x[0] * 10;
  if (!lz && (x[0] == 0)) x[0] = HT16K33_SPACE;

  x[2] = right / 10;
  x[3] = right - x[2] * 10;
  display(x);
  displayColon(colon);
  return inRange;
}


//  seconds / minutes max 6039 == 99:99
bool HT16K33Driver::displaySeconds(uint16_t seconds, bool colon, bool lz)
{
  uint8_t left = seconds / 60;
  uint8_t right = seconds - left * 60;
  return displayTime(left, right, colon, lz);
}


bool HT16K33Driver::displayFloat(float f, uint8_t decimals)
{
  bool inRange = ((-999.5 < f) && (f < 9999.5));

  bool neg = (f < 0);
  if (neg) f = -f;

  if (decimals == 2) f = round(f * 100) * 0.01;
  if (decimals == 1) f = round(f * 10) * 0.1;
  if (decimals == 0) f = round(f);

  int whole = f;
  int point = 3;
  if (whole < 1000) point = 2;
  if (whole < 100) point = 1;
  if (whole < 10) point = 0;

  if (f >= 1)
  {
    while (f < 1000) f *= 10;
    whole = round(f);
  }
  else
  {
    whole = round(f * 1000);
  }

  uint8_t x[4], h, l;
  h = whole / 100;
  l = whole - h * 100;
  x[0] = h / 10;
  x[1] = h - x[0] * 10;
  x[2] = l / 10;
  x[3] = l - x[2] * 10;
  if (neg)    //  corrections for neg => all shift one position
  {
    x[3] = x[2];
    x[2] = x[1];
    x[1] = x[0];
    x[0] = HT16K33_MINUS;
    point++;
  }
  //  add leading spaces
  while (point + decimals < 3)
  {
    x[3] = x[2];
    x[2] = x[1];
    x[1] = x[0];
    x[0] = HT16K33_SPACE;
    point++;
  }

  display(x, point);

  return inRange;
}


bool HT16K33Driver::displayUnit(float f, uint8_t decimals, uint8_t unitChar)
{
  bool inRange = ((-99.5 < f) && (f < 999.5));

  bool neg = (f < 0);
  if (neg) f = -f;

  if (decimals == 2) f = round(f * 100) * 0.01;
  if (decimals == 1) f = round(f * 10) * 0.1;
  if (decimals == 0) f = round(f);

  int whole = f;
  int point = 2;
  if (whole < 100) point = 1;
  if (whole < 10) point = 0;

  if (f >= 1)
  {
    while (f < 100) f *= 10;
    whole = round(f);
  }
  else
  {
    whole = round(f * 100);
  }

  uint8_t x[4];
  x[0] = whole / 100;
  whole = whole - x[0] * 100;
  x[1] = whole / 10;
  x[2] = whole % 10;
  x[3] = unitChar;
  if (neg)    //  corrections for neg => all shift one position
  {
    x[3] = unitChar;
    x[2] = x[1];
    x[1] = x[0];
    x[0] = HT16K33_MINUS;
    point++;
  }
  //  add leading spaces
  while (point + decimals < 2)
  {
    x[3] = unitChar;
    x[2] = x[1];
    x[1] = x[0];
    x[0] = HT16K33_SPACE;
    point++;
  }

  display(x, point);

  return inRange;
}


/////////////////////////////////////////////////////////////////////
//
//  EXPERIMENTAL
//
bool HT16K33Driver::displayFixedPoint0(float f)
{
  bool inRange = ((-999.5 < f) && (f < 9999.5));
  displayFloat(f, 0);
  return inRange;
}

bool HT16K33Driver::displayFixedPoint1(float f)
{
  bool inRange = ((-99.5 < f) && (f < 999.95));
  displayFloat(f, 1);
  return inRange;
}

bool HT16K33Driver::displayFixedPoint2(float f)
{
  bool inRange = ((-9.95 < f) && (f < 99.995));
  displayFloat(f, 2);
  return inRange;
}

bool HT16K33Driver::displayFixedPoint3(float f)
{
  bool inRange = ((0 < f) && (f < 9.9995));
  displayFloat(f, 3);
  return inRange;
}

/////////////////////////////////////////////////////////////////////

void HT16K33Driver::displayTest(uint8_t del)
{
  for (int i = 0; i < 256; i++)
  {
    writePos(0, i);
    writePos(1, i);
    writePos(2, i);
    writePos(3, i);
    writePos(4, i);
    delay(del);
  }
}


void HT16K33Driver::displayRaw(uint8_t *array, bool colon)
{
  writePos(0, array[0]);
  writePos(1, array[1]);
  writePos(3, array[2]);
  writePos(4, array[3]);
  writePos(2, colon ? 255 : 0);
}


bool HT16K33Driver::displayVULeft(uint8_t value)
{
  bool inRange = (value < 9);     //  can display 0..8  bars
  uint8_t ar[4];
  for (int idx = 3; idx >=0; idx--)
  {
    if (value >= 2)
    {
      ar[idx] = 0x36;        //   ||
      value -= 2;
    }
    else if (value == 1)
    {
      ar[idx] = 0x06;        //   _|
      value = 0;
    }
    else ar[idx] = 0x00;     //   __
  }
  displayRaw(ar);
  return inRange;
}


bool HT16K33Driver::displayVURight(uint8_t value)
{
  bool inRange = (value < 9);
  uint8_t ar[4];
  for (uint8_t idx = 0; idx < 4; idx++)
  {
    if (value >= 2)
    {
      ar[idx] = 0x36;        //   ||
      value -= 2;
    }
    else if (value == 1)
    {
      ar[idx] = 0x30;        //   |_
      value = 0;
    }
    else ar[idx] = 0x00;     //   __
  }
  displayRaw(ar);
  return inRange;
}


void HT16K33Driver::display(uint8_t *array)
{
  for (uint8_t i = 0; i < (4 - _digits); i++)
  {
    if (array[i] != 0) break;
    array[i] = HT16K33_SPACE;
  }
  writePos(0, charmap[array[0]]);
  writePos(1, charmap[array[1]]);
  writePos(3, charmap[array[2]]);
  writePos(4, charmap[array[3]]);

  //  debug to Serial
  //  dumpSerial(array, 0);
}

void HT16K33Driver::displayChars(char *array)
{
  for (uint8_t i = 0; i < (4 - _digits); i++)
  {
    if (array[i] != 0) break;
    array[i] = HT16K33_SPACE;
  }
  writePos(0, getBinaryChar(array[0]));
  writePos(1, getBinaryChar(array[1]));
  writePos(2, 0); // no colon
  writePos(3, getBinaryChar(array[2]));
  writePos(4, getBinaryChar(array[3]));

  //  debug to Serial
  //  dumpSerial(array, 0);
}


void HT16K33Driver::display(uint8_t *array, uint8_t point)
{
  //  debug to Serial
  //  dumpSerial(array, point);
  //  dumpSerial();

  writePos(0, charmap[array[0]], point == 0);
  writePos(1, charmap[array[1]], point == 1);
  writePos(3, charmap[array[2]], point == 2);
  writePos(4, charmap[array[3]], point == 3);
}



void HT16K33Driver::displayColon(uint8_t on)
{
  writePos(2, on ? 2 : 0);
}


void HT16K33Driver::displayExtraLeds(uint8_t value)
{
  if (value > 30)
  {
    return;  //  no leds.
  }
  writePos(2, value);
}


void HT16K33Driver::dumpSerial(uint8_t *array, uint8_t point)
{
  //  to debug without display
  for (int i = 0; i < 4; i++)
  {
    if (array[i] == HT16K33_SPACE) Serial.print("_");
    else if (array[i] == HT16K33_MINUS) Serial.print("-");
    else Serial.print(array[i]);
    if (i == point) Serial.print(".");
  }
  Serial.print(" (");
  Serial.print(point);
  Serial.println(")");
}


void HT16K33Driver::dumpSerial()
{
  //  to debug without display
  for (int i = 0; i < 4; i++)
  {
    if (_displayCache[i] < 0x10) Serial.print("0");
    Serial.print(_displayCache[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}


uint8_t HT16K33Driver::getAddress()
{
  return _address;
}








//////////////////////////////////////////////////////////
//
// PRIVATE
//
void HT16K33Driver::_refresh()
{
  for (uint8_t pos = 0; pos < 4; pos++)
  {
    #ifdef ARDUINO_TEENSY41
    beginTransmission(_address);
    write(pos * 2);
    write(_displayCache[pos]);
    endTransmission();
    #else
    _wire->beginTransmission(_address);
    _wire->write(pos * 2);
    _wire->write(_displayCache[pos]);
    _wire->endTransmission();
    #endif
  }
}

void HT16K33Driver::writeCmd(uint8_t cmd)
{
  #ifdef ARDUINO_TEENSY41
  beginTransmission(_address);
  write(cmd);
  endTransmission();
  #else
  _wire->beginTransmission(_address);
  _wire->write(cmd);
  _wire->endTransmission();
  #endif
}


void HT16K33Driver::writePos(uint8_t pos, uint8_t mask)
{
  #ifdef ARDUINO_TEENSY41
  if (_cache && (_displayCache[pos] == mask)) return;
  beginTransmission(_address);
  write(pos * 2);
  write(mask);
  endTransmission();
  _displayCache[pos] = _cache ? mask : HT16K33_NONE;
  #else
  if (_cache && (_displayCache[pos] == mask)) return;
  _wire->beginTransmission(_address);
  _wire->write(pos * 2);
  _wire->write(mask);
  _wire->endTransmission();
  _displayCache[pos] = _cache ? mask : HT16K33_NONE;
  #endif
}


void HT16K33Driver::writePos(uint8_t pos, uint8_t mask, bool point)
{
  if (point) mask |= 0x80;
  else mask &= 0x7F;
  writePos(pos, mask);
}

uint8_t HT16K33Driver::getBinaryChar(uint8_t character)
{

  switch(character) {
    case 'a':
      return 0b01011111;
    case 'A':
      return 0b01110111;
    case 'b':
    case 'B':
      return 0b01111100;
    case 'c':
      return 0b01011000;
    case 'C':
      return 0b00111001;
    case 'd':
    case 'D':
      return 0b01011110;
    case 'e':
    case 'E':
      return 0b01111001;
    case 'f':
    case 'F':
      return 0b01110001;
    case 'g':
    case 'G':
      return 0b00111101;
    case 'h':
      return 0b01110100;
    case 'H':
      return 0b01110110;
    case 'i':
      return 0b00000100;
    case 'I':
      return 0b00000110;
    case 'j':
    case 'J':
      return 0b00011110;
    case 'k':
    case 'K':
      return 0b01110101;
    case 'l':
      return 0b00000110;
    case 'L':
      return 0b00111000;
    case 'm':
    case 'M':
      return 0b00010101;
    case 'n':
      return 0b01010100;
    case 'N':
      return 0b00110111;
    case 'o':
      return 0b01011100;
    case 'O':
      return 0b00111111;
    case 'p':
    case 'P':
      return 0b01110011;
    case 'q':
    case 'Q':
      return 0b01100111;
    case 'r':
    case 'R':
      return 0b01010000;
    case 's':
    case 'S':
      return 0b01101101;
    case 't':
    case 'T':
      return 0b01111000;
    case 'u':
      return 0b00011100;
    case 'U':
    case 'v':
    case 'V':
      return 0b00111110;
    case 'w':
    case 'W':
      return 0b01111110;
    case 'x':
    case 'X':
      return 0b01110110;
    case 'y':
    case 'Y':
      return 0b01101110;
    case 'z':
    case 'Z':
      return 0b01011011;
      
    case '0':
      return 0b00111111;
    case '1':
      return 0b00000110;
    case '2':
      return 0b01011011;
    case '3':
      return 0b01001111;
    case '4':
      return 0b01100110;
    case '5':
      return 0b01101101;
    case '6':
      return 0b01111101;
    case '7':
      return 0b00000111;
    case '8':
      return 0b01111111;
    case '9':
      return 0b01100111;
    case ' ':
      return 0b00000000;
    case '!':
      return 0b10000110;
    case '-':
      return 0b01000000;
    case '_':
      return 0b00001000;
    case '.':
      return 0b10000000;
    case ']':
      return 0b00100111;
    case '[':
      return 0b00110011;
    case '|':
      return 0b11010100;
    case '@':
      return 0b01110000;
    case '#':
      return 0b01111001;
  }

  return 0;
}

#ifdef ARDUINO_TEENSY41
void HT16K33Driver::beginTransmission(int address) {
    write_address = (uint8_t)address;
    tx_next_byte_to_write = 0;
}

size_t HT16K33Driver::write(uint8_t data) {
    if (tx_next_byte_to_write < tx_buffer_length) {
        tx_buffer[tx_next_byte_to_write++] = data;
        return 1;
    }
    return 0;
}

uint8_t HT16K33Driver::endTransmission(int stop) {
    master.write_async(write_address, tx_buffer, tx_next_byte_to_write, stop);
    finish();
    return 0;
}

void HT16K33Driver::finish() {
    elapsedMillis timeout;
    while (timeout < timeout_millis) {
        if (master.finished()) {
            return;
        }
    }
    Serial.println("Timed out waiting for transfer to finish.");
}
#endif
//  -- END OF FILE --
