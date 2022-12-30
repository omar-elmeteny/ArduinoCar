#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include <SoftwareSerial.h>
#include <limits.h>

#include "sevenSegment.h"
#include "joystick.h"
#include "mp3Player.h"

SoftwareSerial BTSerial(50, 51);

char gearStatus = ' ';

void inputTask(void *pvParameters);
void gearSevenSegmentTask(void *pvParameters);
//void bluetoothSendTask(void *pvParameters);
void mp3PlayerTask(void *pvParameters);
void mp3PlayerStatusUpdatesTask(void *pvParameters);
void screenUpdateTask(void *pvParameters);

TaskHandle_t gearSevenSegmentTaskHandle;
//TaskHandle_t bluetoothSendTaskHandle;
TaskHandle_t mp3PlayerTaskHandle;
TaskHandle_t screenUpdateTaskHandle;

SemaphoreHandle_t screenPinsMutex;

void setup() {
  // uncomment to enable Serial.print
  Serial.begin(38400);

  setupSevenSegment();
  setupJoystick();
  setupScreen();
  setupMp3Player();
  BTSerial.begin(38400);

  xTaskCreate(inputTask, "INPUT", 512, NULL, 1, NULL);
  //xTaskCreate(joystickTask, "JOYSTICK_TASK", 128, NULL, 2, NULL);
  xTaskCreate(mp3PlayerStatusUpdatesTask, "MP3_STATUS_TASK", 512, NULL, 1, NULL);


  xTaskCreate(gearSevenSegmentTask, "GEAR_7SEGMENT_TASK", 512, NULL, 3, &gearSevenSegmentTaskHandle);
  //xTaskCreate(bluetoothSendTask, "BLUETOOTH_SEND", 512, NULL, 3, &bluetoothSendTaskHandle);
  xTaskCreate(mp3PlayerTask, "MP3_TASK", 512, NULL, 3, &mp3PlayerTaskHandle);

  xTaskCreate(screenUpdateTask, "SCREEN_UPDATE_TASK", 512, NULL, 2, &screenUpdateTaskHandle);

  screenPinsMutex = xSemaphoreCreateMutex();
}

void loop() {
}

void screenUpdateTask(void *pvParameters) {
  while (true) {
    uint32_t update;

    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &update, portMAX_DELAY) == pdTRUE && update) {
      if (xSemaphoreTake(screenPinsMutex, portMAX_DELAY) == pdTRUE) {
        updateScreen();
        xSemaphoreGive(screenPinsMutex);
      }
    }
  }
}

void mp3PlayerStatusUpdatesTask(void *pvParameters) {
  while (true) {
    printMp3PlayerUpdates();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void mp3PlayerTask(void *pvParameters) {
  uint32_t command;
  while (true) {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &command, portMAX_DELAY) && command) {
      switch (command) {
        case PREVIOUS_BUTTON:
          previousSong();
          break;
        case NEXT_BUTTON:
          nextSong();
          break;
        case PAUSE_PLAY_BUTTON:
          playPause();
          break;
      }
      xTaskNotify(screenUpdateTaskHandle, 1, eSetValueWithOverwrite);
    }
  }
}

// void bluetoothSendTask(void *pvParameters) {
//   uint32_t command;
//   while (true) {
//     if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &command, portMAX_DELAY) && command) {
//       BTSerial.write((char)command);
//     }
//   }
// }

void gearSevenSegmentTask(void *pvParameters) {
  uint32_t gearValue;
  while (true) {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &gearValue, portMAX_DELAY) && gearValue) {
      sevenSegPrint((char)gearValue);
    }
  }
}

void inputTask(void *pvParameters) {
  while (true) {
    char newGearStatus = getCurrentGearStatus(gearStatus);
    if (newGearStatus != gearStatus) {
      gearStatus = newGearStatus;
      xTaskNotify(gearSevenSegmentTaskHandle, (uint32_t)gearStatus, eSetValueWithOverwrite);
      //xTaskNotify(bluetoothSendTaskHandle, (uint32_t)gearStatus, eSetValueWithOverwrite);
      BTSerial.write(gearStatus);
    }

    if (xSemaphoreTake(screenPinsMutex, portMAX_DELAY) != pdTRUE) {
      continue;
    }
    uint8_t button = getTappedButton();
    xSemaphoreGive(screenPinsMutex);

    if (button) {
      xTaskNotify(mp3PlayerTaskHandle, (uint32_t)button, eSetValueWithOverwrite);
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}
