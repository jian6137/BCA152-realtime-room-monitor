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

/* SensorTask implementation */
void sensor_task(void *pvParameters) {
    dht22_init(GPIO_NUM_4);
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000);
    
    for (;;) {
        float temperature = 0.0f;
        float humidity = 0.0f;
        
        if (dht22_read(&temperature, &humidity)) {
            printf("Temperature: %.2f C\n", temperature);
            printf("Humidity: %.2f %%\n", humidity);
        } else {
            printf("Failed to read DHT22\n");
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

extern "C" void app_main() {
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    // create SensorTask (Priority 2 as per the lab manual suggested priorities to use)
    xTaskCreate(sensor_task, "SensorTask", 2048, NULL, 2, NULL);
}