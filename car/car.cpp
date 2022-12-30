#include "car.h"
#include <Arduino.h>

#define MOTOR_A_ENABLE 5
#define MOTOR_B_ENABLE 3

#define MOTOR_A_FWD 6
#define MOTOR_A_RVS 7
#define MOTOR_B_FWD 9
#define MOTOR_B_RVS 8 

int8_t leftMotorValue = 0;
int8_t rightMotorValue = 0;

void setupCar() {
  pinMode(MOTOR_A_ENABLE, OUTPUT);
  pinMode(MOTOR_A_FWD, OUTPUT);
  pinMode(MOTOR_A_RVS, OUTPUT);

  pinMode(MOTOR_B_ENABLE, OUTPUT);
  pinMode(MOTOR_B_FWD, OUTPUT);
  pinMode(MOTOR_B_RVS, OUTPUT);
}

void setMotor(uint8_t enablePin, uint8_t fwdPin, uint8_t rvsPin, int8_t value) {
  if (value == 0) {
    analogWrite(enablePin, 0);
    digitalWrite(fwdPin, LOW);
    digitalWrite(rvsPin, LOW);
    return;
  } else if (value < 0) {
    digitalWrite(fwdPin, LOW);
    digitalWrite(rvsPin, HIGH);
    value = 3;
  } else {
    digitalWrite(fwdPin, HIGH);
    digitalWrite(rvsPin, LOW);
  }
  if (value == 1) {
    analogWrite(enablePin, 200);
  } else if (value == 2) {
    analogWrite(enablePin, 225);
  } else {
    analogWrite(enablePin, 255);
  }
}

void setCarMovement(int8_t leftWheels, int8_t rightWheels) {
  if (leftWheels != leftMotorValue) {
    setMotor(MOTOR_A_ENABLE, MOTOR_A_FWD, MOTOR_A_RVS, leftWheels);
    leftMotorValue = leftWheels;
  }
  if (rightWheels != rightMotorValue) {
    setMotor(MOTOR_B_ENABLE, MOTOR_B_FWD, MOTOR_B_RVS, rightWheels);
    rightMotorValue = rightWheels;
  }
}