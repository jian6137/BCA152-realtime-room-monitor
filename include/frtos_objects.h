#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

/* structure to hold all sensor readings */
struct SensorData {
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

/* display mode for UI navigation */
enum class DisplayMode {
    TEMPERATURE,
    HUMIDITY,
    LIGHT,
    MOTION
};

extern DisplayMode currentDisplayMode;
extern bool currentMotion;

/* queue for broadcasting sensor data to other tasks */
extern QueueHandle_t sensorQueue;

/* mutex to protect serial output */
extern SemaphoreHandle_t serialMutex;

/* event group for signaling system states */
extern EventGroupHandle_t systemEventGroup;
#define EVENT_ACTIVE BIT0
#define EVENT_MOTION BIT1
#define EVENT_ALARM  BIT2

/* initialize all FreeRTOS objects */
void rtos_objects_init();

#endif // RTOS_OBJECTS_H
