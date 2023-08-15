#include <Audio.h>
#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
#include "rotary_redux_mono.h"

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=657.0000114440918,655.0000705718994
rotary_redux_mono rotary;
AudioAmplifier           amp2;           //xy=949.75,789.75
AudioEffectFreeverb      reverb1;        //xy=1083.750015258789,789.7500123977661
AudioAmplifier           amp3;           //xy=1217.750015258789,787.7500114440918
AudioFilterBiquad        biquad1;        //xy=1347.7500190734863,787.750072479248
AudioMixer4              mixer1;         //xy=1374.2500190734863,663.7500095367432
AudioOutputI2S           i2s1;           //xy=1539,670
AudioConnection          patchCord1(i2s2, 0, rotary, 0);
AudioConnection          patchCord2(rotary, 0, mixer1, 0);
AudioConnection          patchCord3(rotary, amp2);
AudioConnection          patchCord4(amp2, reverb1);
AudioConnection          patchCord5(reverb1, amp3);
AudioConnection          patchCord6(amp3, biquad1);
AudioConnection          patchCord7(biquad1, 0, mixer1, 1);
AudioConnection          patchCord8(amp3, 0, mixer1, 1);
AudioConnection          patchCord9(mixer1, 0, i2s1, 0);
// GUItool: end automatically generated code
