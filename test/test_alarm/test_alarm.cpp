#include <unity.h>
#include "alarm.h"
#include "frtos_objects.h"

DisplayMode currentDisplayMode = DisplayMode::TEMPERATURE;
bool currentMotion = false;
QueueHandle_t sensorQueue = nullptr;
EventGroupHandle_t systemEventGroup = nullptr;
SemaphoreHandle_t serialMutex = nullptr;

void setUp(void) {
    // set up here
}

void tearDown(void) {
    // clean up here
}

void test_alarm_below_lower_threshold(void) {
    TEST_ASSERT_EQUAL((int)AlarmState::LOW_TEMPERATURE, (int)evaluateTemperature(17.9f));
}

void test_alarm_exactly_lower_threshold(void) {
    TEST_ASSERT_EQUAL((int)AlarmState::NORMAL, (int)evaluateTemperature(18.0f));
}

void test_alarm_normal_value(void) {
    TEST_ASSERT_EQUAL((int)AlarmState::NORMAL, (int)evaluateTemperature(24.0f));
}

void test_alarm_exactly_upper_threshold(void) {
    TEST_ASSERT_EQUAL((int)AlarmState::NORMAL, (int)evaluateTemperature(30.0f));
}

void test_alarm_above_upper_threshold(void) {
    TEST_ASSERT_EQUAL((int)AlarmState::HIGH_TEMPERATURE, (int)evaluateTemperature(30.1f));
}

#if defined(ESP_PLATFORM)
extern "C" void app_main() {
#else
int main() {
#endif
    UNITY_BEGIN();
    RUN_TEST(test_alarm_below_lower_threshold);
    RUN_TEST(test_alarm_exactly_lower_threshold);
    RUN_TEST(test_alarm_normal_value);
    RUN_TEST(test_alarm_exactly_upper_threshold);
    RUN_TEST(test_alarm_above_upper_threshold);
    UNITY_END();
#ifndef ESP_PLATFORM
    return 0;
#endif
}
