#ifndef ALARM_H
#define ALARM_H

#include "frtos_objects.h"

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

/* Initialize alarm hardware (buzzer) */
void alarm_init();

/* Set current alarm state */
void alarm_set_state(AlarmState state);

/* Get current alarm state */
AlarmState alarm_get_state();

/* Evaluates temperature and returns the appropriate alarm state */
AlarmState evaluateTemperature(float temperature);

#endif // ALARM_H

