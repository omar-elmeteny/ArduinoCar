#include "projdefs.h"
#include <stdint.h>
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "common.h"
#include "sevenSegment.h"

// SEVEN SEGMENT PINS
#define A 28
#define B 29
#define C 30
#define D 33
#define E 32
#define F 27
#define G 26
#define DP 31  // decimal
#define common_cathode 0
#define common_anode 1

bool segMode = common_anode;  // set this to your segment type, my segment is common_cathode


int seg[]{ A, B, C, D, E, F, G, DP };  // segment pins
uint8_t chars = 6;                     // max value in the array "Chars"

uint8_t Chars[6][9]{
  { '1', 0, 1, 1, 0, 0, 0, 0, 0 },  //1
  { '2', 1, 1, 0, 1, 1, 0, 1, 0 },  //2
  { '3', 1, 1, 1, 1, 0, 0, 1, 0 },  //3
  { 'n', 0, 0, 1, 0, 1, 0, 1, 0 },  //n
  { 'p', 1, 1, 0, 0, 1, 1, 1, 0 },  //p
  { 'r', 0, 0, 0, 0, 1, 0, 1, 0 },  //r
};

void setupSevenSegment() {
  // set segment pins as OUTPUT

  pinMode(seg[0], OUTPUT);
  pinMode(seg[1], OUTPUT);
  pinMode(seg[2], OUTPUT);
  pinMode(seg[3], OUTPUT);
  pinMode(seg[4], OUTPUT);
  pinMode(seg[5], OUTPUT);
  pinMode(seg[6], OUTPUT);
  pinMode(seg[7], OUTPUT);
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

  if (charNum == -1) {  // if the character not found

    for (int i = 0; i <= 6; i++) {
      digitalWrite(seg[i], HIGH);
      vTaskDelay(pdMS_TO_TICKS(100));
      digitalWrite(seg[i], LOW);
    }
    for (int i = 0; i <= 2; i++) {
      vTaskDelay(pdMS_TO_TICKS(100));
      setState(HIGH);
      vTaskDelay(pdMS_TO_TICKS(100));
      setState(LOW);
    }
  } else {  // else if the character found print it

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
