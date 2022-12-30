#include <Arduino.h>

#include "common.h"
#include "joystick.h"

/**
 *
 * PlayStation One Analog Stick Test file
 * Author: @iainiscreative
 *
 * This is a program to check if the analog stick is working okay.
 * The analog stick has two potentiometers and a button.
 * This program checks the values of the potentiometers and prints it to serial,
 * while an LED is also placed to test the button.
 *
 * This code is open source for you to use and even modify.
 *
 */

// Store each pin in a variable

#define POTENTIOMETER_X A15
#define POTENTIOMETER_Y A14
#define JOYSTICK_BUTTON 41


int buttonStatus = LOW;

double POTENTIOMETER_MAX;
#define POTENTIOMETER_MIN 0.0

void setupJoystick() {
  // Setup joystick button
  pinMode(JOYSTICK_BUTTON, INPUT_PULLUP);
  POTENTIOMETER_MAX = analogRead(POTENTIOMETER_Y) * 2;
}

char getCurrentGearStatus(char oldGearStatus) {
  // Read each Potentiometer's value and store in a variable
  double sensorValueX = analogRead(POTENTIOMETER_X);
  double sensorValueY = (double)analogRead(POTENTIOMETER_Y) / (POTENTIOMETER_MAX - POTENTIOMETER_MIN);

  int currentButtonStatus = digitalRead(JOYSTICK_BUTTON);
  char newGearStatus = oldGearStatus;

  if (sensorValueY < 0.47) {
    newGearStatus = 'r';
  } else if (sensorValueY <= 0.53) {
    if (currentButtonStatus == LOW && currentButtonStatus != buttonStatus) {
      if (oldGearStatus == 'p') {
        newGearStatus = 'n';
      } else if (oldGearStatus = 'n') {
        newGearStatus = 'p';
      }
    } else {
      newGearStatus = oldGearStatus == 'p' || oldGearStatus == 'n' ? oldGearStatus : oldGearStatus == ' ' ? 'p'
                                                                                              : 'n';
    }
  } else if (sensorValueY <= 0.7) {
    newGearStatus = '1';
  } else if (sensorValueY <= 0.85) {
    newGearStatus = '2';
  } else {
    newGearStatus = '3';
  }
  buttonStatus = currentButtonStatus;
  return newGearStatus;
}