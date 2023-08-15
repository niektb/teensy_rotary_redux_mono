// Set the master clock frequency to 11.2896 MHz
void setMasterClock() {
  uint32_t register_value = ((F_CPU / 11289600) / 2) - 1;
  *(volatile uint32_t *)0x400E4490 = 0x00080000 | (register_value << 6);
}

float mapFloat(int value, int fromLow, int fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
