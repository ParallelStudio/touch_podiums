int noteNumber = 1; //change per teensy

int sensorPin = 15;
bool pinState;
bool noteState;
unsigned long lastDebounce;
unsigned long debounceDelay = 20;
int targetRange = 3000; // Capacitance threshold == touched


void setup() {
  Serial.begin(9600);
}

void loop() {
//  Serial.println(touchRead(sensorPin));
  bool tripped = touchRead(sensorPin) > targetRange;
  if (pinState != tripped) {
    lastDebounce = millis();
    pinState = tripped;
  }
  else if (noteState != pinState && (millis() - lastDebounce) > debounceDelay) {
    usbMIDI.sendNoteOn(noteNumber, pinState ? 127 : 0, 1);
    Serial.println(pinState);
    noteState = pinState;
  }
  delay(10);        // delay in between reads for stability
}
