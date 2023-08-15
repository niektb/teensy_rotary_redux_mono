#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

class Potentiometer {
private:
  int pin;
  float minVal;
  float maxVal;
  float curVal;
  float tolerance;
  float lastVal;
  const bool loga;

public:
  Potentiometer(int pin, float minVal, float maxVal, const bool loga);
  float readValue();
  bool hasValueChanged();
  float getCurrentValue();
  float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);
  float mapFloatLin(float value, float fromLow, float fromHigh, float toLow, float toHigh);
  float mapFloatLog(float value, float fromLow, float fromHigh, float toLow, float toHigh);
};

#endif
