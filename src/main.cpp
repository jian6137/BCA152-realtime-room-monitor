/* 
* Copyright 2026, Jiandale Ambal
*
* A BCA152 Laboratory Project
* 
* This is the main entry point for the firmware
*
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sensors.h"
#include "frtos_objects.h"
#include "display.h"

/* SensorTask implementation */
void sensor_task(void *pvParameters) {
    dht22_init(GPIO_NUM_4);
    ldr_init();
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000);
    
    for (;;) {
        SensorData data = {0.0f, 0.0f, 0, false};
        
        if (!dht22_read(&data.temperature, &data.humidity)) {
            printf("Failed to read DHT22\n");
        }
        
        data.lightLevel = ldr_read_percentage();
        data.motionDetected = false; // dummy & temp data for now until PIR reading is implemented
        
        xQueueSend(sensorQueue, &data, 0);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/* DisplayTask implementation */
void display_task(void *pvParameters) {
    display_init();
    
    // show my custom bootscreen for 3 seconds

    // NOTE: The purpose of this is TO MAKE SURE THAT THE WHOLE SYSTEM IS STABILIZED BEFORE DOING ANYTHING
    // This includes electrical voltages in the MCU.
    // This is just how I IMPLEMENT MY PROJECTS that uses an OPERATING SYSTEM
    display_update(NULL, "BOOT");
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    SensorData data;
    const char* currentMenu = "TEMPERATURE";
    
    for (;;) {
        if (xQueueReceive(sensorQueue, &data, portMAX_DELAY) == pdPASS) {
            display_update(&data, currentMenu);
        }
    }
}

extern "C" void app_main() {
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    rtos_objects_init();

    xTaskCreate(display_task, "DisplayTask", 4096, NULL, 1, NULL);
    xTaskCreate(sensor_task, "SensorTask", 4096, NULL, 2, NULL);
}