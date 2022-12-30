#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "car.h"
#include "distanceSensor.h"
#include "buzzer.h"

SoftwareSerial BTSerial(10, 11);

void bluetoothReceiveTask(void *pvParameters);
void carControlTask(void *pvParameters);
void sensorTask(void *pvParameters);
void buzzerTask(void *pvParameters);

char gearStatus = ' ';

TaskHandle_t carControlTaskHandle;
TaskHandle_t buzzerTaskHandle;

void setup() {
  Serial.begin(38400);

  setupCar();
  setupBuzzer();
  setupDistanceSensors();
  BTSerial.begin(38400);

  xTaskCreate(sensorTask, "US", 128, NULL, 2, NULL);
  xTaskCreate(buzzerTask, "BUZ", 128, NULL, 3, &buzzerTaskHandle);

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
      // if (abs(newDistanceCentimeters - oldDistanceCentimeters) > DISTANCE_EPSILON_CM) {
      //   Serial.print("distance change sensor");
      //   Serial.print(i == 0 ? " FL " : i == 1 ? " FR "
      //                                : i == 2 ? " BL "
      //                                         : " BR ");
      //   Serial.println(newDistanceCentimeters);
      //   distancesCentimeters[i] = newDistanceCentimeters;
      // }
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
  Serial.println("Starting bluetooth receive.");
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
          Serial.print("Received car control: ");
          Serial.println(c);
          break;
      }
    }
  }
}

void carControlTask(void *pvParameters) {
  Serial.println("Starting car control task.");
  uint32_t carCommand;
  while (true) {
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &carCommand, portMAX_DELAY) && carCommand) {
      char cmd = (char)carCommand;
      int motorValue = 0;
      switch (cmd) {
        case 'r':
          gearStatus = cmd;
          motorValue = -1;
          break;
        case 'n':
        case 'p':
          gearStatus = cmd;
          motorValue = 0;
          break;
        case '1':
        case '2':
        case '3':
          gearStatus = cmd;
          motorValue = cmd - '0';
          break;
      }
      setCarMovement(motorValue, motorValue);
    }
  }
}

void loop() {
}
