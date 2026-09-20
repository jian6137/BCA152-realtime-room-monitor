#include "alarm.h"
#include "driver/gpio.h"

#define BUZZER_PIN GPIO_NUM_13

static AlarmState currentAlarmState = AlarmState::NORMAL;

void alarm_init() {
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_PIN, 0);
}

void alarm_set_state(AlarmState state) {
    currentAlarmState = state;
}

AlarmState alarm_get_state() {
    return currentAlarmState;
}

AlarmState evaluateTemperature(float temperature) {
    if (temperature < 18.0f) {
        return AlarmState::LOW_TEMPERATURE;
    } else if (temperature > 30.0f) {
        return AlarmState::HIGH_TEMPERATURE;
    }
    return AlarmState::NORMAL;
}

