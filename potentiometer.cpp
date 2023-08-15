#include "Potentiometer.h"
#include "Arduino.h"

Potentiometer::Potentiometer(int pin, float minVal, float maxVal, const bool loga)
  : loga(loga) {
  this->pin = pin;
  this->minVal = minVal;
  this->maxVal = maxVal;
  curVal = analogRead(pin);
  curVal = mapFloat(curVal, 0, 1023, minVal, maxVal);
  tolerance = 0.01 * (maxVal - minVal);  // 1% of the full value range
  lastVal = curVal;
}

float Potentiometer::readValue() {
  float val = analogRead(pin);
  val = mapFloat(val, 0, 1023, minVal, maxVal);
  curVal = val;
  return curVal;
}

bool Potentiometer::hasValueChanged() {
  float val = readValue();
  float diff = abs(val - lastVal);
  if (diff > tolerance) {
    lastVal = val;
    return true;
  }
  return false;
}

float Potentiometer::getCurrentValue() {
  return curVal;
}

float Potentiometer::mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  if (this->loga)
    return mapFloatLog(value, fromLow, fromHigh, toLow, toHigh);
  else
    return mapFloatLin(value, fromLow, fromHigh, toLow, toHigh);
}

float Potentiometer::mapFloatLin(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

float Potentiometer::mapFloatLog(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return toLow + (toHigh - toLow) * (pow(value - fromLow, 2) / pow(fromHigh - fromLow, 2));
}