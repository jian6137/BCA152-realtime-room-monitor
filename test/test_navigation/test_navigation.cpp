#include <unity.h>
#include "input.h"

// Mock the globals used by input.h so we don't need to link frtos_objects.cpp natively
DisplayMode currentDisplayMode = DisplayMode::TEMPERATURE;
bool currentMotion = false;
QueueHandle_t sensorQueue = nullptr;
EventGroupHandle_t systemEventGroup = nullptr;
SemaphoreHandle_t serialMutex = nullptr;

void setUp(void) {}
void tearDown(void) {}

void test_next_transitions(void) {
    currentDisplayMode = DisplayMode::TEMPERATURE;
    nextDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::HUMIDITY, (int)currentDisplayMode);
    nextDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::LIGHT, (int)currentDisplayMode);
    nextDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::MOTION, (int)currentDisplayMode);
}

void test_next_wraparound(void) {
    currentDisplayMode = DisplayMode::MOTION;
    nextDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::TEMPERATURE, (int)currentDisplayMode);
}

void test_prev_transitions(void) {
    currentDisplayMode = DisplayMode::MOTION;
    previousDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::LIGHT, (int)currentDisplayMode);
    previousDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::HUMIDITY, (int)currentDisplayMode);
    previousDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::TEMPERATURE, (int)currentDisplayMode);
}

void test_prev_wraparound(void) {
    currentDisplayMode = DisplayMode::TEMPERATURE;
    previousDisplayMode();
    TEST_ASSERT_EQUAL((int)DisplayMode::MOTION, (int)currentDisplayMode);
}

#if defined(ESP_PLATFORM)
extern "C" void app_main() {
#else
int main() {
#endif
    UNITY_BEGIN();
    RUN_TEST(test_next_transitions);
    RUN_TEST(test_next_wraparound);
    RUN_TEST(test_prev_transitions);
    RUN_TEST(test_prev_wraparound);
    UNITY_END();
#ifndef ESP_PLATFORM
    return 0;
#endif
}

