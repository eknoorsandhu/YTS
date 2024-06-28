#include <Arduino.h>

const int emgPin = 34;
int emgValue = 0;

void setup() {
Serial.begin(115200);

analogReadResolution(12);

analogSetAttenuation(ADC_11db);
}

void loop() {
  // put your main code here, to run repeatedly:
  emgValue = analogRead(emgPin);
  Serial.println(emgValue);
  delay(100);
}
