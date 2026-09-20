#include "frtos_objects.h"

QueueHandle_t sensorQueue;
DisplayMode currentDisplayMode = DisplayMode::TEMPERATURE;
bool currentMotion = false;
EventGroupHandle_t systemEventGroup;
SemaphoreHandle_t serialMutex;

void rtos_objects_init() {
    // create queue to hold up to 10 SensorData items
    sensorQueue = xQueueCreate(10, sizeof(SensorData));
    
    // create event group and start in ACTIVE state
    systemEventGroup = xEventGroupCreate();
    xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE);
    
    // create mutex for serial port protection
    serialMutex = xSemaphoreCreateMutex();
}

