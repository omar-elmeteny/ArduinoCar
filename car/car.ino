#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "common.h"

#include "car.h"
#include "distanceSensor.h"
#include "buzzer.h"

SoftwareSerial BTSerial(10, 11);

void bluetoothReceiveTask(void *pvParameters);
void carControlTask(void *pvParameters);
void sensorTask(void *pvParameters);
void buzzerTask(void *pvParameters);
void parkingTask(void *pvParameters);

TaskHandle_t carControlTaskHandle;
TaskHandle_t buzzerTaskHandle;
TaskHandle_t parkingTaskHandle;
void setup() {
  #ifdef DEBUG
  Serial.begin(38400);
  #endif

  setupCar();
  setupBuzzer();
  setupDistanceSensors();
  BTSerial.begin(38400);

  xTaskCreate(sensorTask, "US", 128, NULL, 2, NULL);
  xTaskCreate(buzzerTask, "BUZ", 128, NULL, 3, &buzzerTaskHandle);
  xTaskCreate(parkingTask, "PARK", 128, NULL, 3, &parkingTaskHandle);

  xTaskCreate(carControlTask, "CAR", 128, NULL, 1, &carControlTaskHandle);
  xTaskCreate(bluetoothReceiveTask, "BT_RCV", 128, NULL, 1, NULL);
}

void buzzerTask(void *pvParamters) {
  uint32_t distance = DISTANCE_FAR;
  bool isBuzzing = false;
  while (true) {
    TickType_t delayTicks = distance == DISTANCE_FAR      ? portMAX_DELAY
                            : distance == DISTANCE_MEDIUM ? pdMS_TO_TICKS(DELAY_MEDIUM)
                            : distance == DISTANCE_CLOSE  ? pdMS_TO_TICKS(DELAY_CLOSE)
                                                          : pdMS_TO_TICKS(DELAY_VERY_CLOSE);
    uint32_t distanceNotification;
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &distanceNotification, delayTicks) && distanceNotification) {
      distance = distanceNotification;
    }
    if (distance == DISTANCE_FAR && isBuzzing) {
      noTone(BUZZER_PIN);
    } else if (distance != DISTANCE_FAR) {
      if (isBuzzing) {
        noTone(BUZZER_PIN);
      } else {
        tone(BUZZER_PIN, BUZZER_FREQUENCY);
      }
      isBuzzing = !isBuzzing;
    }
  }
}

void sensorTask(void *pvParameters) {
  float distancesCentimeters[SENSOR_COUNT];
  for (int i = 0; i < SENSOR_COUNT; i++) {
    distancesCentimeters[i] = -1;
  }
  uint32_t minDistance = DISTANCE_FAR;
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(100));
    float minDistaneCm = FLT_MAX;
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
      float oldDistanceCentimeters = distancesCentimeters[i];
      double newDistanceCentimeters = readDistanceCentimeters(i);
      #if DEBUG
      if (abs(newDistanceCentimeters - oldDistanceCentimeters) > DISTANCE_EPSILON_CM) {
        Serial.print("distance change sensor");
        Serial.print(i == 0 ? " FL " : i == 1 ? " FR "
                                     : i == 2 ? " BL "
                                              : " BR ");
        Serial.println(newDistanceCentimeters);
        distancesCentimeters[i] = newDistanceCentimeters;
      }
      #endif
      if (newDistanceCentimeters < minDistaneCm) {
        minDistaneCm = newDistanceCentimeters;
      }
    }

    uint32_t newDistance = minDistaneCm > DISTANCE_MEDIUM_CM       ? DISTANCE_FAR
                           : minDistaneCm > DISTANCE_CLOSE_CM      ? DISTANCE_MEDIUM
                           : minDistaneCm > DISTANCE_VERY_CLOSE_CM ? DISTANCE_CLOSE
                                                                   : DISTANCE_VERY_CLOSE;

    if (newDistance != minDistance) {
      xTaskNotify(buzzerTaskHandle, (uint32_t)newDistance, eSetValueWithOverwrite);
      minDistance = newDistance;
    }
  }
}

void bluetoothReceiveTask(void *pvParamters) {
  #if DEBUG
  Serial.println("Starting bluetooth receive.");
  #endif
  while (true) {
    if (BTSerial.available()) {
      char c = BTSerial.read();
      switch (c) {
        case 'r':
        case 'n':
        case '1':
        case '2':
        case '3':
        case 'p':
          xTaskNotify(carControlTaskHandle, (uint32_t)c, eSetValueWithOverwrite);
          #if DEBUG
          Serial.print("Received car control: ");
          Serial.println(c);
          #endif
          break;
      }
    }
  }
}

void carControlTask(void *pvParameters) {
  #if DEBUG
  Serial.println("Starting car control task.");
  #endif
  uint32_t carCommand;
  int8_t motorValue = 0;
  int8_t direction = 0;

  while (true) {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &carCommand, portMAX_DELAY) && carCommand) {
      char cmd = (char)carCommand;
      switch (cmd) {
        case 'r':
          motorValue = -1;
          break;
        case 'n':
        case 'p':
          motorValue = 0;
          break;
        case '1':
        case '2':
        case '3':
          motorValue = cmd - '0';
          break;
        case 's':
          direction = 0;
          break;
        case 'a':
          direction = -1;
          break;
        case 'd':
          direction = 1;
          break;
      }
      if (direction == 0) {
        setCarMovement(motorValue, motorValue);
      } else if (direction == -1) {
        setCarMovement(0, motorValue);
      } else if (direction == 1) {
        setCarMovement(motorValue, 0);
      }
    }
  }
}

void loop() {
}
