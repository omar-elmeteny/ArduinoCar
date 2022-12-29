#ifndef DistanceSensor_h
#define DistanceSensor_h

#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define BACK_LEFT 2
#define BACK_RIGHT 3

#define SENSOR_COUNT 4

float readDistanceCentimeters(uint8_t sensor);
void setupDistanceSensors();

#define DISTANCE_MEDIUM_CM 15.0f
#define DISTANCE_CLOSE_CM 10.0f
#define DISTANCE_VERY_CLOSE_CM 5.0f

#define DISTANCE_FAR 1
#define DISTANCE_MEDIUM 2
#define DISTANCE_CLOSE  3
#define DISTANCE_VERY_CLOSE 4

#define DISTANCE_EPSILON_CM 0.5f
#endif
