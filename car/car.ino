#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>

#include "car.h"

SoftwareSerial BTSerial (10,11);

void bluetoothReceiveTask(void *pvParameters);
void carControlTask(void *pvParameters);

char gearStatus = ' ';
char oldMo

TaskHandle_t carControlTaskHandle;
void setup() {

  setupCar();
  BTSerial.begin(38400);
  xTaskCreate(carControlTask, "CAR_CONTROL_TASK", 128, NULL, 1, &carControlTaskHandle);
  xTaskCreate(bluetoothReceiveTask, "BLUETOOTH_RECEIVE_TASK", 128, NULL, 2, NULL);
}

void bluetoothReceiveTask(void *pvParamters) {
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
          break;          

      }
    }
  }
}

void carControlTask(void *pvParameters) {
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
