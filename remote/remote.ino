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
#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include "sevenSegment.h"
#include "joystick.h"

SoftwareSerial BTSerial(50, 51);

char gearStatus = ' ';

void joystickTask(void *pvParameters);
void gearSevenSegmentTask(void *pvParameters);

TaskHandle_t gearSevenSegmentTaskHandle;

void setup() {
  // uncomment to enable Serial.print
  // Serial.begin(9600);

  setupSevenSegment();
  setupJoystick();

  xTaskCreate(joystickTask, "JOYSTICK_TASK", 128, NULL, 1, NULL);
  xTaskCreate(gearSevenSegmentTask, "GEAR_7SEGMENT_TASK", 128, NULL, 2, &gearSevenSegmentTaskHandle);
}

void loop() {
}

void gearSevenSegmentTask(void *pvParameters) {
  uint32_t gearValue;
  while (true) {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &gearValue, portMAX_DELAY) && gearValue) {
      sevenSegPrint((char)gearValue);
    }
  }
}

void joystickTask(void *pvParameters) {
  while (true) {
    char newGearStatus = getCurrentGearStatus(gearStatus);

    if (newGearStatus != gearStatus) {
      gearStatus = newGearStatus;
      xTaskNotify(gearSevenSegmentTaskHandle, (uint32_t)gearStatus, eSetValueWithOverwrite);
    }
  }
}
