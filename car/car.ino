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
void setup() {
#ifdef DEBUG
  Serial.begin(38400);
#endif

  setupCar();
  setupBuzzer();
  setupDistanceSensors();
  BTSerial.begin(38400);
  BTSerial.listen();

  xTaskCreate(buzzerTask, "BUZ", 128, NULL, 5, &buzzerTaskHandle);
  xTaskCreate(sensorTask, "SENSOR", 128, NULL, 2, NULL);
  xTaskCreate(carControlTask, "CAR", 128, NULL, 5, &carControlTaskHandle);

  xTaskCreate(bluetoothReceiveTask, "BT_RCV", 128, NULL, 1, NULL);
}

void waitUntilObstacleOrTime(uint32_t ms, bool front, float distance) {
  TickType_t entryTime = xTaskGetTickCount();
  TickType_t exitTime = entryTime + pdMS_TO_TICKS(ms);
  uint8_t leftSensor = front ? FRONT_LEFT : BACK_LEFT;
  uint8_t rightSensor = front ? FRONT_RIGHT : BACK_RIGHT;


  while (readDistanceCentimeters(leftSensor) > distance && readDistanceCentimeters(rightSensor) > distance
         && xTaskGetTickCount() < exitTime) {
  }
}

void stopCar() {
#ifdef DEBUG
  Serial.println("Stopping car");
#endif

  xTaskNotify(carControlTaskHandle, 'n', eSetValueWithoutOverwrite);
  xTaskNotify(carControlTaskHandle, 's', eSetValueWithoutOverwrite);

#ifdef DEBUG
  vTaskDelay(pdMS_TO_TICKS(5000));
#else
  vTaskDelay(pdMS_TO_TICKS(500));
#endif
}

void reverseCar(uint32_t ms, bool checkSensor) {
#ifdef DEBUG
  Serial.println("Reversing car");
#endif

  xTaskNotify(carControlTaskHandle, 's', eSetValueWithoutOverwrite);
  xTaskNotify(carControlTaskHandle, 'r', eSetValueWithoutOverwrite);

  if (checkSensor) {
    waitUntilObstacleOrTime(ms, false, 5);
  } else {
    vTaskDelay(pdMS_TO_TICKS(ms));
  }

  stopCar();
}

void rotateCar(bool reverse, bool rightSide) {
#ifdef DEBUG
  Serial.println("Rotating car");
#endif
  xTaskNotify(carControlTaskHandle, rightSide ? 'a' : 'd', eSetValueWithoutOverwrite);
  xTaskNotify(carControlTaskHandle, reverse ? 'r' : '3', eSetValueWithoutOverwrite);
  waitUntilObstacleOrTime(1500, !reverse, 5);

  stopCar();
}

void parkCar(bool rightSide) {
#ifdef DEBUG
  Serial.println(rightSide ? "Parking right side" : "Parking left side");
#endif

  reverseCar(500, false);
  rotateCar(true, rightSide);
  reverseCar(1000, false);
  rotateCar(false, rightSide);

#ifdef DEBUG
  Serial.println("Car parked");
#endif
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
      // #if DEBUG
      // if (abs(newDistanceCentimeters - oldDistanceCentimeters) > DISTANCE_EPSILON_CM) {
      //   Serial.print("distance change sensor");
      //   Serial.print(i == 0 ? " FL " : i == 1 ? " FR "
      //                                : i == 2 ? " BL "
      //                                         : " BR ");
      //   Serial.println(newDistanceCentimeters);
      //   distancesCentimeters[i] = newDistanceCentimeters;
      // }
      // #endif
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
        case 'a':
        case 's':
        case 'd':
          xTaskNotify(carControlTaskHandle, (uint32_t)c, eSetValueWithOverwrite);
#if DEBUG
          Serial.print("Received car control: ");
          Serial.println(c);
#endif
          break;
        case 'p':
          parkCar(true);
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
      // #ifdef DEBUG
      //       Serial.print("Direction = ");
      //       Serial.print(direction);
      //       Serial.print(", MotorValue = ");
      //       Serial.println(motorValue);
      // #endif
      if (direction == 0) {
        setCarMovement(motorValue, motorValue);
      } else if (direction == 1) {
        setCarMovement(0, motorValue);
      } else {
        setCarMovement(motorValue, 0);
      }
    }
  }
}

void loop() {
}
