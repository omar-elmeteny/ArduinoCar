#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include "sevenSegment.h"
#include "joystick.h"

SoftwareSerial BTSerial(50, 51);

char gearStatus = ' ';

void joystickTask(void *pvParameters);
void gearSevenSegmentTask(void *pvParameters);
void bluetoothSendTask(void *pvParameters);

TaskHandle_t gearSevenSegmentTaskHandle;
TaskHandle_t bluetoothSendTaskHandle;

void setup() {
  // uncomment to enable Serial.print
  // Serial.begin(9600);

  setupSevenSegment();
  setupJoystick();
  BTSerial.begin(38400);

  xTaskCreate(joystickTask, "JOYSTICK_TASK", 128, NULL, 1, NULL);
  xTaskCreate(gearSevenSegmentTask, "GEAR_7SEGMENT_TASK", 128, NULL, 2, &gearSevenSegmentTaskHandle);
  xTaskCreate(bluetoothSendTask, "BLUETOOTH_SEND", 128, NULL, 2, &bluetoothSendTaskHandle);
}

void loop() {
}

void bluetoothSendTask(void *pvParameters) {
  uint32_t command;
  while (true) {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &command, portMAX_DELAY) && command) {
      BTSerial.write((char)command);
    }
  }  
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
      xTaskNotify(bluetoothSendTaskHandle, (uint32_t)gearStatus, eSetValueWithOverwrite);
    }
  }
}
