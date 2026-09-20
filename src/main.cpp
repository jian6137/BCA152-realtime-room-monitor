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
        
        // send data to queue, do not block if full
        xQueueSend(sensorQueue, &data, 0);
        
        // diagnostic print (to be removed later if OLED is implemented, but only for testing)
        printf("Sent to Queue -> Temp: %.2f C, Hum: %.2f %%, Light: %d %%\n", 
               data.temperature, data.humidity, data.lightLevel);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

extern "C" void app_main() {
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    // initialize FreeRTOS objects
    rtos_objects_init();

    // create SensorTask (Priority 2 as per lab manual suggested priorities)
    xTaskCreate(sensor_task, "SensorTask", 2048, NULL, 2, NULL);
}