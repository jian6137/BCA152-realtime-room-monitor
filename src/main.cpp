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
#include "alarm.h"
#include "motion.h"
#include "system_state.h"
#include "driver/gpio.h"

/* SensorTask implementation */
void sensor_task(void *pvParameters) {
    dht22_init(GPIO_NUM_4);
    ldr_init();
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000);
    
    SensorData data = {0.0f, 0.0f, 0, false}; // Cache previous readings
    
    for (;;) {
        if (xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) {
            if (!dht22_read(&data.temperature, &data.humidity)) {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                printf("Failed to read DHT22\n");
                xSemaphoreGive(serialMutex);
            }
            
            data.lightLevel = ldr_read_percentage();
            data.motionDetected = currentMotion;
            
            // Evaluate alarm conditions only if active
            AlarmState newState = evaluateTemperature(data.temperature);
            alarm_set_state(newState);
            
            if (newState == AlarmState::HIGH_TEMPERATURE || newState == AlarmState::LOW_TEMPERATURE) {
                xEventGroupSetBits(systemEventGroup, EVENT_ALARM);
            } else {
                xEventGroupClearBits(systemEventGroup, EVENT_ALARM);
            }
        } else {
            data.motionDetected = currentMotion;
            alarm_set_state(AlarmState::NORMAL);
            xEventGroupClearBits(systemEventGroup, EVENT_ALARM);
        }
        
        xQueueSend(sensorQueue, &data, 0);
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/* MotionTask implementation */
void motion_task(void *pvParameters) {
    motion_init();
    
    for (;;) {
        currentMotion = motion_detect();
        if (currentMotion) {
            xEventGroupSetBits(systemEventGroup, EVENT_MOTION);
        } else {
            xEventGroupClearBits(systemEventGroup, EVENT_MOTION);
        }
        
        // Poll at 10Hz or 10 cycles per second
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* StateTask implementation */
void state_task(void *pvParameters) {
    uint32_t lastMotionTicks = xTaskGetTickCount();
    const uint32_t timeoutTicks = pdMS_TO_TICKS(15000); // 15 seconds inactivity
    
    for (;;) {
        EventBits_t bits = xEventGroupWaitBits(
            systemEventGroup, 
            EVENT_MOTION, 
            pdFALSE, // Do not auto-clear
            pdFALSE, 
            timeoutTicks
        );
        
        bool motionNow = (bits & EVENT_MOTION) != 0;
        
        SystemState newState = evaluateSystemState(motionNow, xTaskGetTickCount(), &lastMotionTicks, timeoutTicks);
        set_system_state(newState);
        
        if (newState == SystemState::ACTIVE) {
            xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE);
        } else {
            xEventGroupClearBits(systemEventGroup, EVENT_ACTIVE);
        }
        
        if (motionNow) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

/* AlarmTask implementation */
void alarm_task(void *pvParameters) {
    alarm_init();
    
    for (;;) {
        xEventGroupWaitBits(
            systemEventGroup, 
            EVENT_ALARM | EVENT_ACTIVE, 
            pdFALSE, 
            pdTRUE, // Wait for ALL bits
            portMAX_DELAY
        );
        
        if ((xEventGroupGetBits(systemEventGroup) & (EVENT_ALARM | EVENT_ACTIVE)) == (EVENT_ALARM | EVENT_ACTIVE)) {
            gpio_set_level(GPIO_NUM_13, 1);
            vTaskDelay(pdMS_TO_TICKS(200));
            gpio_set_level(GPIO_NUM_13, 0);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
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
        if (xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) {
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
        }
        
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
    SystemState lastSystemState = SystemState::ACTIVE;
    bool needsUpdate = true;
    
    for (;;) {
        // Determine logical state from event group
        bool isActive = (xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) != 0;
        SystemState currentState = isActive ? SystemState::ACTIVE : SystemState::INACTIVE;
        
        if (xQueueReceive(sensorQueue, &data, pdMS_TO_TICKS(50)) == pdPASS) {
            needsUpdate = true;
        }
        
        if (lastMode != currentDisplayMode) {
            lastMode = currentDisplayMode;
            needsUpdate = true;
        }

        // Controls the OLED Display Activity (ACTIVE / INACTIVE)
        if (lastSystemState != currentState) {
            lastSystemState = currentState;
            if (currentState == SystemState::INACTIVE) {
                display_clear();
            } else {
                needsUpdate = true;
            }
        }
        
        if (needsUpdate && currentState == SystemState::ACTIVE) {
            display_update(&data, get_display_mode_str(currentDisplayMode));
            needsUpdate = false;
        }
    }
}

extern "C" void app_main() {
    rtos_objects_init();

    xSemaphoreTake(serialMutex, portMAX_DELAY);
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");
    xSemaphoreGive(serialMutex);

    xTaskCreate(display_task, "DisplayTask", 4096, NULL, 1, NULL);
    xTaskCreate(sensor_task, "SensorTask", 4096, NULL, 2, NULL);
    xTaskCreate(alarm_task, "AlarmTask", 2048, NULL, 2, NULL);
    xTaskCreate(input_task, "InputTask", 2048, NULL, 3, NULL);
    xTaskCreate(motion_task, "MotionTask", 2048, NULL, 3, NULL);
    xTaskCreate(state_task, "StateTask", 2048, NULL, 4, NULL);
}