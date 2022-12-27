
#include <Arduino_FreeRTOS.h>

#define X (4 + 5)

void Task1(void *pvParameters);
void Task1(void *pvParameters);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  xTaskCreate(Task1, "Task1", 128, NULL, 2, NULL);
  xTaskCreate(Task2, "Task2", 128, NULL, 1, NULL);
  int y = X * 7;

}

void loop() {

}

void Task1(void *pvParameters) {
  TickType_t xLastWakeupTime;
  const TickType_t xDelay1s = pdMS_TO_TICKS(1000);
  xLastWakeupTime = xTaskGetTickCount();
  
  while(1) {
    Serial.println("Task 1");
    vTaskDelayUntil(&xLastWakeupTime, xDelay1s);
  }
}

void Task2(void *pvParameters) {
  TickType_t xLastWakeupTime;
  const TickType_t xDelayHalfs = pdMS_TO_TICKS(500);
  xLastWakeupTime = xTaskGetTickCount();
  while(1) {
    Serial.println("Task 2");
    vTaskDelayUntil(&xLastWakeupTime, xDelayHalfs);

  }
}