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

/* Task A implementation */
void task_a(void *pvParameters) {
    for (;;) {
        printf("Task A running\n");
        // block for 1000 ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Task B implementation */
void task_b(void *pvParameters) {
    for (;;) {
        printf("Task B running\n");
        // block for 1500 ms
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

extern "C" void app_main() {
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    // create Task A
    xTaskCreate(task_a, "TaskA", 2048, NULL, 1, NULL);

    // create Task B
    xTaskCreate(task_b, "TaskB", 2048, NULL, 1, NULL);
}