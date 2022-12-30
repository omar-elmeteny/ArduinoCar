#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include <SoftwareSerial.h>
#include <limits.h>

#include "sevenSegment.h"
#include "joystick.h"
#include "mp3Player.h"
#include "lightSensor.h"

SoftwareSerial BTSerial(50, 51);

char gearStatus = ' ';
char direction = ' ';

void inputTask(void *pvParameters);
void gearSevenSegmentTask(void *pvParameters);
void mp3PlayerTask(void *pvParameters);
#ifdef DEBUG
void mp3PlayerStatusUpdatesTask(void *pvParameters);
#endif
void screenUpdateTask(void *pvParameters);

TaskHandle_t gearSevenSegmentTaskHandle;
TaskHandle_t mp3PlayerTaskHandle;
TaskHandle_t screenUpdateTaskHandle;

SemaphoreHandle_t screenPinsMutex;

void setup()
{
#ifdef DEBUG
  Serial.begin(38400);
#endif

  setupSevenSegment();
  setupJoystick();
  setupScreen();
  setupMp3Player();
  setupLightSensor();
  BTSerial.begin(38400);

  xTaskCreate(inputTask, "INPUT", 512, NULL, 1, NULL);
#ifdef DEBUG
  xTaskCreate(mp3PlayerStatusUpdatesTask, "MP3_STATUS_TASK", 512, NULL, 1, NULL);
#endif

  xTaskCreate(gearSevenSegmentTask, "GEAR_7SEGMENT_TASK", 512, NULL, 3, &gearSevenSegmentTaskHandle);
  xTaskCreate(mp3PlayerTask, "MP3_TASK", 512, NULL, 3, &mp3PlayerTaskHandle);

  xTaskCreate(screenUpdateTask, "SCREEN_UPDATE_TASK", 512, NULL, 2, &screenUpdateTaskHandle);

  screenPinsMutex = xSemaphoreCreateMutex();
}

void loop()
{
}

void screenUpdateTask(void *pvParameters)
{
  while (true)
  {
    uint32_t update;

    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &update, portMAX_DELAY) == pdTRUE && update)
    {
      if (xSemaphoreTake(screenPinsMutex, portMAX_DELAY) == pdTRUE)
      {
        updateScreen();
        xSemaphoreGive(screenPinsMutex);
      }
    }
  }
}
#ifdef DEBUG
void mp3PlayerStatusUpdatesTask(void *pvParameters)
{
  while (true)
  {
    printMp3PlayerUpdates();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
#endif

void mp3PlayerTask(void *pvParameters)
{
  uint32_t command;
  while (true)
  {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &command, portMAX_DELAY) && command)
    {
      switch (command)
      {
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

void gearSevenSegmentTask(void *pvParameters)
{
  uint32_t gearValue;
  while (true)
  {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &gearValue, portMAX_DELAY) && gearValue)
    {
      sevenSegPrint((char)gearValue);
    }
  }
}

void handleJoyStickInput()
{
  char newGearStatus = getCurrentGearStatus(gearStatus);
  if (newGearStatus != gearStatus)
  {
    gearStatus = newGearStatus;
    xTaskNotify(gearSevenSegmentTaskHandle, (uint32_t)gearStatus, eSetValueWithOverwrite);
    BTSerial.write(gearStatus);
  }

  char newDirection = getDirection();
  if (newDirection != direction) {
    direction = newDirection;
    BTSerial.write(gearStatus);
  }
}

void handleTouchScreenInput()
{
  if (xSemaphoreTake(screenPinsMutex, portMAX_DELAY) != pdTRUE)
  {
    return;
  }
  uint8_t button = getTappedButton();
  xSemaphoreGive(screenPinsMutex);

  if (button)
  {
    xTaskNotify(mp3PlayerTaskHandle, (uint32_t)button, eSetValueWithOverwrite);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void handleLightSensorInput()
{
  float lightIntensity = getLightIntensity();
  if (lightIntensity < LIGHT_THRESHOLD_BRIGHT)
  {
    analogWrite(LED, 0);
  }
  else if (lightIntensity < LIGHT_THRESHOLD_DIM)
  {
    analogWrite(LED, 128);
  }
  else
  {
    analogWrite(LED, 255);
  }
}

void inputTask(void *pvParameters)
{
  while (true)
  {
    handleJoyStickInput();
    handleTouchScreenInput();
    handleLightSensorInput();
  }
}
