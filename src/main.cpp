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

#include "input.h"
#include "driver/gpio.h"

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
        data.motionDetected = false; // dummy for now
        
        xQueueSend(sensorQueue, &data, 0);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/* InputTask implementation */
void input_task(void *pvParameters) {
    gpio_set_direction(GPIO_NUM_26, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_26, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_27, GPIO_PULLUP_ONLY);
    
    int lastClk = gpio_get_level(GPIO_NUM_26);
    
    for (;;) {
        int clk = gpio_get_level(GPIO_NUM_26);
        if (clk != lastClk && clk == 1) { // rising edge
            int dt = gpio_get_level(GPIO_NUM_27);
            if (dt != clk) {
                nextDisplayMode();
            } else {
                previousDisplayMode();
            }
        }
        lastClk = clk;
        vTaskDelay(pdMS_TO_TICKS(20));
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
    
    SensorData data = {0.0f, 0.0f, 0, false};
    DisplayMode lastMode = currentDisplayMode;
    bool needsUpdate = true;
    
    for (;;) {
        // wait up to 50ms for new sensor data
        if (xQueueReceive(sensorQueue, &data, pdMS_TO_TICKS(50)) == pdPASS) {
            needsUpdate = true;
        }
        
        // check if user turned the encoder
        if (lastMode != currentDisplayMode) {
            lastMode = currentDisplayMode;
            needsUpdate = true;
        }
        
        if (needsUpdate) {
            display_update(&data, get_display_mode_str(currentDisplayMode));
            needsUpdate = false;
        }
    }
}

extern "C" void app_main() {
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    rtos_objects_init();

    xTaskCreate(display_task, "DisplayTask", 4096, NULL, 1, NULL);
    xTaskCreate(sensor_task, "SensorTask", 4096, NULL, 2, NULL);
    xTaskCreate(input_task, "InputTask", 2048, NULL, 3, NULL);
}