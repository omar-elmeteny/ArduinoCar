#include <Arduino.h>
#include <stdint.h>
#include <Arduino_FreeRTOS.h>

#include "distanceSensor.h"
#include "projdefs.h"

#define FRONT_LEFT_TRIG 4
#define FRONT_RIGHT_TRIG 12
#define BACK_LEFT_TRIG 2
#define BACK_RIGHT_TRIG 13

#define FRONT_LEFT_ECHO A4
#define FRONT_RIGHT_ECHO A3
#define BACK_LEFT_ECHO A5
#define BACK_RIGHT_ECHO A2

void setupDistanceSensors() {
  pinMode(FRONT_LEFT_TRIG, OUTPUT);
  pinMode(FRONT_LEFT_ECHO, INPUT);

  pinMode(FRONT_RIGHT_TRIG, OUTPUT);
  pinMode(FRONT_RIGHT_ECHO, INPUT);

  pinMode(BACK_LEFT_TRIG, OUTPUT);
  pinMode(BACK_LEFT_ECHO, INPUT);

  pinMode(BACK_RIGHT_TRIG, OUTPUT);
  pinMode(BACK_RIGHT_ECHO, INPUT);
}

void taskDelayMicroSeconds(uint32_t microSeconds) {
   vTaskDelay(pdMS_TO_TICKS(microSeconds) / 1000);
}

float readDistanceCentimeters(uint8_t sensor) {
  int trigPin;
  int echoPin;

  switch (sensor) {
    case FRONT_LEFT:
      trigPin = FRONT_LEFT_TRIG;
      echoPin = FRONT_LEFT_ECHO;
      break;
    case FRONT_RIGHT:
      trigPin = FRONT_RIGHT_TRIG;
      echoPin = FRONT_RIGHT_ECHO;
      break;
    case BACK_LEFT:
      trigPin = BACK_LEFT_TRIG;
      echoPin = BACK_LEFT_ECHO;
      break;
    case BACK_RIGHT:
      trigPin = BACK_RIGHT_TRIG;
      echoPin = BACK_RIGHT_ECHO;
      break;
    default: // We should never get here.
      return -1;
  }

  digitalWrite(trigPin, LOW);
  taskDelayMicroSeconds(2);
  //delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  taskDelayMicroSeconds(10);
  //delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float durationMicroseconds = pulseIn(echoPin, HIGH);
  return (float)durationMicroseconds * 0.0343f / 2;
}