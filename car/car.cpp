#include "car.h"
#include <Arduino.h>

#include "common.h"
#define MOTOR_A_ENABLE 5
#define MOTOR_B_ENABLE 3

#define MOTOR_A_FWD 7
#define MOTOR_A_RVS 6
#define MOTOR_B_FWD 8
#define MOTOR_B_RVS 9

#define STOP 255
#define SPEED_1 250
#define SPEED_2 225
#define SPEED_3 255


int8_t leftMotorValue = 0;
int8_t rightMotorValue = 0;

void setupCar() {
  pinMode(MOTOR_A_ENABLE, OUTPUT);
  pinMode(MOTOR_A_FWD, OUTPUT);
  pinMode(MOTOR_A_RVS, OUTPUT);

  pinMode(MOTOR_B_ENABLE, OUTPUT);
  pinMode(MOTOR_B_FWD, OUTPUT);
  pinMode(MOTOR_B_RVS, OUTPUT);

  digitalWrite(MOTOR_A_FWD, HIGH);
  digitalWrite(MOTOR_A_RVS, HIGH);
  digitalWrite(MOTOR_B_FWD, HIGH);
  digitalWrite(MOTOR_B_RVS, HIGH);

  analogWrite(MOTOR_B_ENABLE, 255);
  analogWrite(MOTOR_A_ENABLE, 255);
}

void setMotor(uint8_t enablePin, uint8_t fwdPin, uint8_t rvsPin, int8_t value) {
  if (value == 0) {
    digitalWrite(fwdPin, HIGH);
    digitalWrite(rvsPin, HIGH);
  } else if (value == 1) {
    digitalWrite(rvsPin, HIGH);
    digitalWrite(fwdPin, LOW);
    analogWrite(enablePin, SPEED_1);
  } else if (value == 2) {
    digitalWrite(rvsPin, HIGH);
    digitalWrite(fwdPin, LOW);
    analogWrite(enablePin, SPEED_2);
  } else if (value == 3) {
    digitalWrite(rvsPin, HIGH);
    digitalWrite(fwdPin, LOW);
    analogWrite(enablePin, SPEED_3);
  } else {
    digitalWrite(fwdPin, HIGH);
    digitalWrite(rvsPin, LOW);
    analogWrite(enablePin, SPEED_3);
  }
}

void setCarMovement(int8_t leftWheels, int8_t rightWheels) {
// #ifdef DEBUG
//   Serial.print("setting motor values ");
//   Serial.print(leftWheels);
//   Serial.print(", ");
//   Serial.print(rightWheels);
//   Serial.print(", ");
//   Serial.print(leftMotorValue);
//   Serial.print(", ");
//   Serial.println(rightMotorValue);
// #endif
  if (leftWheels != leftMotorValue) {
// #ifdef DEBUG
//     Serial.print("Setting left wheels to speed ");
//     Serial.println(leftWheels);
// #endif
    setMotor(MOTOR_A_ENABLE, MOTOR_A_FWD, MOTOR_A_RVS, leftWheels);
    leftMotorValue = leftWheels;
  }
  if (rightWheels != rightMotorValue) {
// #ifdef DEBUG
//     Serial.print("Setting right wheels to speed ");
//     Serial.println(rightWheels);
// #endif
    setMotor(MOTOR_B_ENABLE, MOTOR_B_FWD, MOTOR_B_RVS, rightWheels);
    rightMotorValue = rightWheels;
  }
}