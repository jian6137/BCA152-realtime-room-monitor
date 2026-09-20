#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/* structure to hold all sensor readings */
struct SensorData {
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

/* queue for broadcasting sensor data to other tasks */
extern QueueHandle_t sensorQueue;

/* initialize all FreeRTOS objects */
void rtos_objects_init();

#endif // RTOS_OBJECTS_H

