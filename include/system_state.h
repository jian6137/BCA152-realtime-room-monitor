#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>

enum class SystemState {
    ACTIVE,
    INACTIVE
};

/* Get current system state */
SystemState get_system_state();

/* Set current system state */
void set_system_state(SystemState state);

/* Evaluates system state transitions based on motion timeout */
SystemState evaluateSystemState(bool motionDetected, uint32_t currentTicks, uint32_t* lastMotionTicks, uint32_t timeoutTicks);

#endif // SYSTEM_STATE_H

