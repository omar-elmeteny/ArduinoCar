/*
  Showing numbers, chars and phrases
                             A (seg[0] in this project)
                            ---
F (seg[5] in this project) |   | B (seg[1] in this project)
                           |   | 
                            --- G (seg[6] in this project)
E (seg[4] in this project) |   | 
                           |   | C (seg[2] in this project)
                            ---  . dot or dicimal (seg[7] in this project)
                             D (seg[3] in this project)

 */
#define A 8
#define B 9
#define C 6
#define D 5
#define E 4
#define F 3
#define G 2
#define DP 7  // decimal
#define common_cathode 0
#define common_anode 1
bool segMode = common_anode;  // set this to your segment type, my segment is common_cathode

int seg[]{ A, B, C, D, E, F, G, DP };  // segment pins
byte chars = 6;                        // max value in the array "Chars"

byte Chars[6][9]{
  { '1', 0, 1, 1, 0, 0, 0, 0, 0 },  //1
  { '2', 1, 1, 0, 1, 1, 0, 1, 0 },  //2
  { '3', 1, 1, 1, 1, 0, 0, 1, 0 },  //3
  { 'n', 0, 0, 1, 0, 1, 0, 1, 0 },  //n
  { 'p', 1, 1, 0, 0, 1, 1, 1, 0 },  //p
  { 'r', 0, 0, 0, 0, 1, 0, 1, 0 },  //r
};

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
#define JOYSTICK_BUTTON 12

// Set last values for each potentiometer
int lastValueX = 0;
int lastValueY = 0;
char gearStatus = ' ';
int buttonStatus = LOW;

double POTENTIOMETER_MAX;
#define POTENTIOMETER_MIN 0.0

void setup() {
  // uncomment to enable Serial.print
  // Serial.begin(9600);

  // set segment pins as OUTPUT

  pinMode(seg[0], OUTPUT);
  pinMode(seg[1], OUTPUT);
  pinMode(seg[2], OUTPUT);
  pinMode(seg[3], OUTPUT);
  pinMode(seg[4], OUTPUT);
  pinMode(seg[5], OUTPUT);
  pinMode(seg[6], OUTPUT);
  pinMode(seg[7], OUTPUT);

  // Setup joystick button
  pinMode(JOYSTICK_BUTTON, INPUT_PULLUP);
  POTENTIOMETER_MAX = analogRead(POTENTIOMETER_Y) * 2;
}

void setState(bool mode)  //sets the hole segment state to "mode"
{
  for (int i = 0; i <= 6; i++) {
    digitalWrite(seg[i], mode);
  }
}

void sevenSegPrint(char Char)  // print any character on the segment ( Note : you can't use capital characters )
{
  int charNum = -1;   // set search resault to -1
  setState(segMode);  //turn off the segment

  for (int i = 0; i < chars; i++) {  //search for the enterd character
    if (Char == Chars[i][0]) {       //if the character found
      charNum = i;                   //set the resault number into charNum ( because this function prints the character using it's number in the array )
    }
  }

  if (charNum == -1)  // if the character not found
  {
    for (int i = 0; i <= 6; i++) {
      digitalWrite(seg[i], HIGH);
      delay(100);
      digitalWrite(seg[i], LOW);
    }
    for (int i = 0; i <= 2; i++) {
      delay(100);
      setState(HIGH);
      delay(100);
      setState(LOW);
    }
  } else  // else if the character found print it
  {
    if (segMode == 0) {  //for segment mode
      for (int i = 0; i < 8; i++) {
        digitalWrite(seg[i], Chars[charNum][i + 1]);
      }
    } else {
      for (int i = 0; i < 8; i++) {
        digitalWrite(seg[i], !Chars[charNum][i + 1]);
      }
    }
  }
}

void loop() {

  // Read each Potentiometer's value and store in a variable
  double sensorValueX = analogRead(POTENTIOMETER_X);
  double sensorValueY = (double)analogRead(POTENTIOMETER_Y) / (POTENTIOMETER_MAX - POTENTIOMETER_MIN);

  int currentButtonStatus = digitalRead(JOYSTICK_BUTTON);
  char newGearStatus = gearStatus;

  // if (buttonStatus != currentButtonStatus) {
  //   Serial.print("button status:");
  //   Serial.println(currentButtonStatus);
  // }


  if (sensorValueY < 0.47) {
    newGearStatus = 'r';
  } else if (sensorValueY <= 0.53) {
    if (currentButtonStatus == LOW && currentButtonStatus != buttonStatus) {
      if (gearStatus == 'p') {
        newGearStatus = 'n';
      } else if (gearStatus = 'n') {
        newGearStatus = 'p';
      }
    } else {
      newGearStatus = gearStatus == 'p' || gearStatus == 'n' ? gearStatus : gearStatus == ' ' ? 'p' : 'n';
    }
  } else if (sensorValueY <= 0.7) {
    newGearStatus = '1';
  } else if (sensorValueY <= 0.85) {
    newGearStatus = '2';
  } else {
    newGearStatus = '3';
  }
  buttonStatus = currentButtonStatus;

  if (newGearStatus != gearStatus) {
    gearStatus = newGearStatus;
    sevenSegPrint(gearStatus);
  }
}
