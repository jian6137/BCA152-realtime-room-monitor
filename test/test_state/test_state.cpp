#include <unity.h>
#include "system_state.h"
#include "frtos_objects.h"

DisplayMode currentDisplayMode = DisplayMode::TEMPERATURE;
bool currentMotion = false;
QueueHandle_t sensorQueue = nullptr;
EventGroupHandle_t systemEventGroup = nullptr;
SemaphoreHandle_t serialMutex = nullptr;

void setUp(void) {}
void tearDown(void) {}

void test_active_no_timeout(void) {
    uint32_t lastMotion = 1000;
    // 5000 ticks passed, timeout is 15000. No motion.
    SystemState result = evaluateSystemState(false, 6000, &lastMotion, 15000);
    TEST_ASSERT_EQUAL((int)SystemState::ACTIVE, (int)result);
    TEST_ASSERT_EQUAL(1000, lastMotion); // Should not update lastMotion
}

void test_active_timeout(void) {
    uint32_t lastMotion = 1000;
    // 16001 ticks passed, timeout is 15000. No motion.
    SystemState result = evaluateSystemState(false, 17000, &lastMotion, 15000);
    TEST_ASSERT_EQUAL((int)SystemState::INACTIVE, (int)result);
}

void test_inactive_no_motion(void) {
    uint32_t lastMotion = 1000;
    // System is inactive because we pass a time > timeout. No motion.
    SystemState result = evaluateSystemState(false, 20000, &lastMotion, 15000);
    TEST_ASSERT_EQUAL((int)SystemState::INACTIVE, (int)result);
}

void test_inactive_motion(void) {
    uint32_t lastMotion = 1000;
    // System was inactive, but now motion is detected
    SystemState result = evaluateSystemState(true, 25000, &lastMotion, 15000);
    TEST_ASSERT_EQUAL((int)SystemState::ACTIVE, (int)result);
    // lastMotion should be updated to current time
    TEST_ASSERT_EQUAL(25000, lastMotion);
}

#if defined(ESP_PLATFORM)
extern "C" void app_main() {
#else
int main() {
#endif
    UNITY_BEGIN();
    RUN_TEST(test_active_no_timeout);
    RUN_TEST(test_active_timeout);
    RUN_TEST(test_inactive_no_motion);
    RUN_TEST(test_inactive_motion);
    UNITY_END();
#ifndef ESP_PLATFORM
    return 0;
#endif
}

