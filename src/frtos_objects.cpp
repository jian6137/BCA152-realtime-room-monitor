#include "rtos_objects.h"

QueueHandle_t sensorQueue;

void rtos_objects_init() {
    // create queue to hold up to 10 SensorData items
    sensorQueue = xQueueCreate(10, sizeof(SensorData));
}

