// Include project header files
#include "audio_tool_export.h"
#include "helper_functions.h"
#include "pin_definitions.h"
#include "potentiometer.h"
#include <IntervalTimer.h>

// Include arduino libraries
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SigmaDSP.h>
#include <ezLED.h>
#include "encoder.h"
#include "switch.h"
#include <EEPROM.h>

extern float tempmonGetTemp(void);

// Include generated parameter file
#include "SigmaDSP_parameters.h"

// Display definitions
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// SigmaDSP library definitions
SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, 44100.00f /*, DSP_RESET*/);
//DSPEEPROM ee(Wire, EEPROM_I2C_ADDRESS, 256, LED);

IntervalTimer timer;
IntervalTimer timer_temp;

// Global variables
int bypass_state = 1;
int speed_state = 0;  // Chorale vs Tremolo

char input = 0;
long lastPeriod = -1;

const float volume_minVal = 0.02;
const float volume_maxVal = 2.0;
Potentiometer volume(pot6, volume_minVal, volume_maxVal, true);

const float mix_minVal = 0.0;
const float mix_maxVal = 1.0;
Potentiometer mix(pot2, mix_minVal, mix_maxVal, false);

const float gain_minVal = 0.5;
const float gain_maxVal = 50.0;
Potentiometer gain(pot4, gain_minVal, gain_maxVal, true);

const float speed_minVal = 0.1;
const float speed_maxVal = 1.0;
Potentiometer speed(pot1, speed_minVal, speed_maxVal, false);

const float horn_minVal = 0.0;
const float horn_maxVal = 10.0;
Potentiometer horn(pot3, horn_minVal, horn_maxVal, false);

/* POT LAYOUT

SPEED     MIX     HORN

GAIN      ENC     VOLUME

BYPASS            TREM/CHORALE

*/

ezLED led(LED, CTRL_ANODE);
Switch encSw = Switch(SW1);
Switch button1 = Switch(FS1);
Switch button2 = Switch(FS2);

bool mixNeedsUpdate = false;

Encoder enc = Encoder(ENCA, ENCB);

struct menuItem {
  char name[20];
  volatile float value;
  float minValue;
  float maxValue;
  float stepSize;
};

#define NUM_MENU_ITEMS 7
#define reverbMix 0
#define reverbSize 1
#define reverbDamping 2
#define reverbCompansion 3
#define reverbFilter 4
#define inputPad 5
#define outputGain 6

struct menuItem menuItems[] = {
  { "Reverb Mix", 0.3, 0.0, 1.0, 0.1 },
  { "Reverb Size", 0.5, 0.0, 1.0, 0.1 },
  { "Reverb Damping", 1.0, 0.0, 1.0, 0.1 },
  { "Reverb Compansion", 1.0, 0.1, 1.0, 0.1 },
  { "Post-reverb LPF", 3000, 1000, 10000, 200 },
  { "Input Pad", 0, 0, 1, 1 },
  { "Output Boost", 0, 0, 1, 1 }
};

menuItem rd;
int currentMenuItem = 0;
volatile float currentValue = menuItems[currentMenuItem].value;
volatile bool isChangingValue = false;
volatile bool needsDisplayUpdate = false;
volatile bool needsDSPUpdate = false;

// Arduino Setup function
void setup() {
  AudioMemory(8);  // Allocate memory for the audio processing

  setMasterClock();
  setPinModes();
  analogWriteFrequency(LED, 375000);

  digitalWrite(DSP_SELFBOOT, LOW);

  digitalWrite(DSP_RESET, LOW);
  delay(100);
  digitalWrite(DSP_RESET, HIGH);

  delay(100);
  Serial.begin(9600);
  Serial.println(F("Hello world!"));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 16);             // Start at top-left corner
  printTextCenter("Locrius", 16);
  printTextCenter("DSP Pedal", 32);
  display.display();

  Wire.begin();
  dsp.begin();

  delay(2000);

  Serial.println(F("Pinging i2c bus...\n0 -> device is present\n2 -> device is not present"));
  Serial.print(F("DSP response: "));
  Serial.println(dsp.ping());
  // Use this step if no EEPROM is present
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(dsp);
  Serial.println(F("Done!\n"));

  Serial.println("Firmware version 1.0");

  // Init DSP Values
  getMenuItemsFromEEPROM();
  mixer1.gain(1, menuItems[reverbMix].value);      // reverb channel
  mixer1.gain(0, 1 - menuItems[reverbMix].value);  // dry channel
  reverb1.roomsize(menuItems[reverbSize].value);
  reverb1.damping(menuItems[reverbDamping].value);
  biquad1.setLowpass(0, menuItems[reverbFilter].value, 0.7);
  amp2.gain(menuItems[reverbCompansion].value);
  amp3.gain(1.f / menuItems[reverbCompansion].value);
  digitalWrite(OUTPUT_GAIN, !menuItems[outputGain].value);
  digitalWrite(INPUT_PAD, menuItems[inputPad].value);
  rotary.setParamValue("Volume", volume.getCurrentValue());
  rotary.setParamValue("Gain", gain.getCurrentValue());
  rotary.setParamValue("Speed", speed.getCurrentValue());

  led.blink(calculateLedOnTime(), calculateLedOnTime(), 0);

  rotary.setParamValue("Horn", horn.getCurrentValue());
  updateMixParam();

  // Un-bypass the DSP
  digitalWrite(BYPASS, bypass_state);

  // Clear display
  display.clearDisplay();
  updateDisplay();

  // Start control loop update timer
  timer.begin(timerCallback, 2500);  // 2500 microseconds = 2.5 milliseconds
  //timer_temp.begin(timertempCallback, 10000000);  // every 10 seconds
}

// Arduino loop function
void loop() {
  led.loop();

  // Switch.TaskFunction Returns:
  // -1 if nothing happened
  // 0 if switch pressed
  // 1 if switch is held
  // 2 if switch is released before being held
  switch (encSw.TaskFunction()) {
    case 0:
      encPress();
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      break;
  }

  switch (button1.TaskFunction()) {
    case 0:
      break;
    case 1:
      getMenuItemsFromEEPROM();
      break;
    case 2:
      bypass_state = !bypass_state;
      digitalWrite(BYPASS, bypass_state);
      needsDisplayUpdate = true;
      break;
    default:
      break;
  }

  switch (button2.TaskFunction()) {
    case 0:
      break;
    case 1:
      writeMenuItemsToEEPROM();
      break;
    case 2:
      speed_state = !speed_state;
      led.blink(calculateLedOnTime(), calculateLedOnTime(), 0);
      rotary.setParamValue("Chorale/Tremolo", speed_state);
      needsDisplayUpdate = true;
      break;
    default:
      break;
  }

  if (mixNeedsUpdate) {
    updateMixParam();
  }

  if (isChangingValue) {
    switch (enc.TaskFunction()) {
      case 0:  // CW
        currentValue = currentValue + menuItems[currentMenuItem].stepSize;
        currentValue = max(menuItems[currentMenuItem].minValue, min(menuItems[currentMenuItem].maxValue, currentValue));
        needsDisplayUpdate = true;
        break;
      case 1:  // CCW
        currentValue = currentValue - menuItems[currentMenuItem].stepSize;
        currentValue = max(menuItems[currentMenuItem].minValue, min(menuItems[currentMenuItem].maxValue, currentValue));
        needsDisplayUpdate = true;
        break;
      default:  // No change
        break;
    }
  } else {  // Changing menu item
    int newMenuItem = currentMenuItem;
    switch (enc.TaskFunction()) {
      case 0:  // CW
        newMenuItem++;
        if (newMenuItem < 0) {
          newMenuItem = NUM_MENU_ITEMS - 1;
        }
        if (newMenuItem >= NUM_MENU_ITEMS) {
          newMenuItem = 0;
        }
        currentMenuItem = newMenuItem;
        currentValue = menuItems[currentMenuItem].value;
        needsDisplayUpdate = true;
        break;
      case 1:  // CCW
        newMenuItem--;
        if (newMenuItem < 0) {
          newMenuItem = NUM_MENU_ITEMS - 1;
        }
        if (newMenuItem >= NUM_MENU_ITEMS) {
          newMenuItem = 0;
        }
        currentMenuItem = newMenuItem;
        currentValue = menuItems[currentMenuItem].value;
        needsDisplayUpdate = true;
        break;
      default:  // No change
        break;
    }
  }

  // Check if the display needs to be updated and perform the update
  if (needsDisplayUpdate) {
    updateDisplay();
    needsDisplayUpdate = false;
  }

  if (needsDSPUpdate) {
    mixer1.gain(1, menuItems[reverbMix].value);      // reverb channel
    mixer1.gain(0, 1 - menuItems[reverbMix].value);  // dry channel
    reverb1.roomsize(menuItems[reverbSize].value);
    reverb1.damping(menuItems[reverbDamping].value);
    biquad1.setLowpass(0, menuItems[reverbFilter].value, 0.7);
    amp2.gain(menuItems[reverbCompansion].value);
    amp3.gain(1.f / menuItems[reverbCompansion].value);
    digitalWrite(OUTPUT_GAIN, !menuItems[outputGain].value);
    digitalWrite(INPUT_PAD, menuItems[inputPad].value);
    needsDSPUpdate = false;
  }
}

void updateMixParam() {
  dsp.volume(MOD_2XMIXER1_ALG0_STAGE0_VOLONE_ADDR, 20 * log(cos(HALF_PI * mix.getCurrentValue())));
  dsp.volume(MOD_2XMIXER1_ALG0_STAGE0_VOLTWO_ADDR, 20 * log(cos(HALF_PI * (1 - mix.getCurrentValue()))));
  mixNeedsUpdate = false;
}

void timerCallback() {
  // Code to execute every 2.5ms
  if (volume.hasValueChanged()) {
    rotary.setParamValue("Volume", volume.getCurrentValue());
  }
  if (gain.hasValueChanged()) {
    rotary.setParamValue("Gain", gain.getCurrentValue());
  }
  if (speed.hasValueChanged()) {
    rotary.setParamValue("Speed", speed.getCurrentValue());
    led.blink(calculateLedOnTime(), calculateLedOnTime(), 0);
  }
  if (horn.hasValueChanged()) {
    rotary.setParamValue("Horn", horn.getCurrentValue());
  }
  if (mix.hasValueChanged()) {
    mixNeedsUpdate = true;
  }
}

void timertempCallback() {
  Serial.print(tempmonGetTemp());
  Serial.println(F("°C"));
  Serial.print(F("MAX CPU: "));
  Serial.print(AudioProcessorUsageMax());
  Serial.println(F("%"));
  Serial.print(F("CPU: "));
  Serial.print(AudioProcessorUsage());
  Serial.println(F("%\n"));
}

int calculateLedOnTime() {
  int val = 500 / speed.getCurrentValue();
  if (speed_state) {
    val = val / 8;
  }
  return val;
}

void encPress() {
  static unsigned long lastDebounceTime = 0;
  static unsigned long debounceDelay = 50;
  unsigned long currentMillis = millis();

  if (currentMillis - lastDebounceTime < debounceDelay) {
    return;
  }

  if (!isChangingValue) {
    isChangingValue = true;
  } else {
    // for some reason I need both the first line and the switch statement
    menuItems[currentMenuItem].value = currentValue;
    isChangingValue = false;
    needsDSPUpdate = true;
  }
  needsDisplayUpdate = true;
  lastDebounceTime = currentMillis;
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(menuItems[currentMenuItem].name);
  display.setTextSize(2);
  display.setCursor(0, 12);
  display.print(currentValue);

  if (isChangingValue) {
    display.drawLine(display.width() - 1, 12, display.width() - 1, 25, SSD1306_WHITE);
  } else {
    display.drawLine(display.width() - 1, 0, display.width() - 1, 7, SSD1306_WHITE);
  }

  display.setCursor(0, 48);
  if (bypass_state)
    display.println("   ");
  else
    display.println("BYP");

  display.setCursor(80, 48);
  if (speed_state)
    display.println("TREM");
  else
    display.println("CHOR");

  display.display();
}

void printTextCenter(const char* buf, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);  //calc width of new string
  display.setCursor((display.width() - w) / 2, y);
  display.print(buf);
}

void writeMenuItemsToEEPROM() {
  for (int i = 0; i < NUM_MENU_ITEMS; i++) {
    EEPROM.put(i * sizeof(menuItem), menuItems[i]);
  }
  Serial.println("SAVED");
  printTextCenter("SAVED", 30);
  display.display();
}

void getMenuItemsFromEEPROM() {
  for (int i = 0; i < NUM_MENU_ITEMS; i++) {
    EEPROM.get(i * sizeof(menuItem), rd);

    printMenuItem(rd);
    bool skip = false;
    if (strncmp(rd.name, menuItems[i].name, sizeof(rd.name)) != 0) {
      Serial.println("Name not equal");
      skip = true;
    }

    if (rd.value < menuItems[i].minValue || rd.value > menuItems[i].maxValue) {
      Serial.println("Value out of bounds");
      skip = true;
    }

    if (rd.stepSize > (rd.maxValue - rd.minValue)) {
      Serial.println("StepSize too big");
      skip = true;
    }

    if (skip) {
      Serial.print("Skip: ");
      Serial.println(rd.name);
      continue;
    }

    memcpy(&menuItems[i], &rd, sizeof(menuItem));
  }
  Serial.println("LOADED");
  printTextCenter("LOADED", 30);
  display.display();
  currentValue = menuItems[currentMenuItem].value;
  needsDSPUpdate = true;
}

void printMenuItem(menuItem item) {
  Serial.print("Name: ");
  Serial.println(item.name);
  Serial.print("Value: ");
  Serial.println(item.value);
  Serial.print("MinValue: ");
  Serial.println(item.minValue);
  Serial.print("MaxValue: ");
  Serial.println(item.maxValue);
  Serial.print("StepSize: ");
  Serial.println(item.stepSize);
  Serial.print("\n");
}