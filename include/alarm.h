#ifndef ALARM_H
#define ALARM_H

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

/* Initialize alarm hardware (buzzer) */
void alarm_init();

/* Set current alarm state */
void alarm_set_state(AlarmState state);

/* Evaluates temperature and returns the appropriate alarm state */
AlarmState evaluateTemperature(float temperature);

#endif // ALARM_H

