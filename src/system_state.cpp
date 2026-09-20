#include "system_state.h"

static SystemState currentState = SystemState::ACTIVE;

SystemState get_system_state() {
    return currentState;
}

void set_system_state(SystemState state) {
    currentState = state;
}

SystemState evaluateSystemState(bool motionDetected, uint32_t currentTicks, uint32_t* lastMotionTicks, uint32_t timeoutTicks) {
    if (motionDetected) {
        *lastMotionTicks = currentTicks;
        return SystemState::ACTIVE;
    }
    
    if ((currentTicks - *lastMotionTicks) > timeoutTicks) {
        return SystemState::INACTIVE;
    }
    
    return SystemState::ACTIVE; // default fallback if neither motion or timeout has happened yet
}

