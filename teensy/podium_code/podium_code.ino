#include "AnalogInputButton.h"

int noteNumber = 2; //change per teensy

int sensorPin = 15;

//adjust these values for the sensitivity and environment of your sensor
//the threshold should be slightly lower than the lowest change you want to cause a turn-on
//the hysteresis should be set at a high enough level to prevent unwanted turn-offs when when the sensor is on
//baseline_period_ticks and timeout_ticks are in units of your refresh period (e.g., you sample the sensor every 10 ms, timeout_ticks = 1000 means 10 seconds)
//noise should be set at how much noise, +/-, you get with the sensor off. The baseline level will update if the sensor value is within current value +/- noise.
//if baseline_always is on, noise is ignored, and the baseline will adjust regardless of the on/off state or current level
const AnalogInputButtonInit btn_init = {
  .threshold = 800,
  .hysteresis = 10,
  .baseline_period_ticks = 100,
  .baseline_always = true,
  .timeout_ticks = 1000,
  .noise = 40,
  .neg_reset_periods = 5,
  .debounce_ticks = 2,
};

AnalogInputButton cap_button(&btn_init);

void setup() {
  Serial.begin(9600);
}

void loop() {
  const int level = touchRead(sensorPin);
  
  const bool state = cap_button.Update(level);
  const bool changed = cap_button.Changed();

  if (changed) {
    const uint8_t vel = state ? 127 : 0;
    usbMIDI.sendNoteOn(noteNumber, vel, 1);
    Serial.println(vel);
  }
  
  Serial.print(level);
  Serial.print(" ");
  Serial.print(cap_button.Baseline());
  Serial.print(" ");
  Serial.println(state);

  delay(10);        // delay in between reads for stability
}
