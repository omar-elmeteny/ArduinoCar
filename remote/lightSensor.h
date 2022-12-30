#ifndef LightSensor_h
#define LightSensor_h

void setupLightSensor();
float getLightIntensity();

#define LDR_SENSOR A13
#define LED 45
#define LIGHT_THRESHOLD_BRIGHT 300
#define LIGHT_THRESHOLD_DIM 700

#endif