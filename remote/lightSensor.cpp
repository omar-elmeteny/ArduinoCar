#include <Arduino.h>
#include "lightSensor.h"

void setupLightSensor() {
  pinMode(45, OUTPUT);
  pinMode(A13, INPUT);
}

float getLightIntensity() {
  float value = analogRead(LDR_SENSOR);         //Reads the Value of LDR(light).
  Serial.print("LDR value is :");  //Prints the value of LDR to Serial Monitor.
  Serial.println(value);
  return value;
}