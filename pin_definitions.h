const int LED = A12;
const int pot1 = A2;
const int pot2 = A1;
const int pot3 = A0;
const int pot4 = A10;
const int pot6 = A3;

const int ENCA = 28;
const int ENCB = 27;
const int SW1 = 22;

const int DSP_RESET = 2;
const int DSP_SELFBOOT = 3;

const int FS1 = 29;
const int FS2 = 9;

const int CS = 10;

const int OUTPUT_GAIN = 4;
const int INPUT_PAD = 5;
const int BYPASS = 6;

void setPinModes() {
  pinMode(LED, OUTPUT);
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(pot3, INPUT);
  pinMode(pot4, INPUT);
  pinMode(pot6, INPUT);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  pinMode(SW1, INPUT);
  pinMode(DSP_RESET, OUTPUT);
  pinMode(DSP_SELFBOOT, OUTPUT);
  pinMode(FS1, INPUT_PULLUP);
  pinMode(FS2, INPUT_PULLUP);
  pinMode(CS, OUTPUT);
  pinMode(OUTPUT_GAIN, OUTPUT);
  pinMode(INPUT_PAD, OUTPUT);
  pinMode(BYPASS, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(DSP_RESET, LOW);
  digitalWrite(DSP_SELFBOOT, LOW);
  digitalWrite(CS, LOW);
  //digitalWrite(OUTPUT_GAIN, HIGH);
  //digitalWrite(INPUT_PAD, HIGH);
  //digitalWrite(BYPASS, LOW);
}