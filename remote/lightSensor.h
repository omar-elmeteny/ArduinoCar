#ifndef LightSensor_h
#define LightSensor_h

void setupLightSensor();
float getLightIntensity();

#define LDR_SENSOR A13
#define LED 45
#define lightThreshold1 300
#define lightThreshold2 700

#endif