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

#define POTENTIOMETER_X A1
#define POTENTIOMETER_Y A0

// Set last values for each potentiometer
int lastValueX = 0;
int lastValueY = 0;

// Begin Setup
void setup() {
  Serial.begin(9600);
}

// Begin Loop
void loop() {
  // Add a tiny delay to the loop
  delay(1000);
  

  // Read each Potentiometer's value and store in a variable
  int sensorValueX = analogRead(POTENTIOMETER_X);
  int sensorValueY = analogRead(POTENTIOMETER_Y);

  /**
   *
   * If the value's of each potentionmeter do not match their respective last
   * value, update the values and update to serial.
   *
   */

  if(sensorValueX != lastValueX) {
    Serial.print("X:");
    Serial.println(sensorValueX);
    lastValueX = sensorValueX;
  }

  if(sensorValueY != lastValueY) {
    Serial.print("Y:");
    Serial.println(sensorValueY);
    lastValueY = sensorValueY;
    // Apply a slight delay to the loop
  }
}