#include "input.h"
#include "driver/gpio.h"

const char* get_display_mode_str(DisplayMode mode) {
    switch(mode) {
        case DisplayMode::TEMPERATURE: return "TEMPERATURE";
        case DisplayMode::HUMIDITY:    return "HUMIDITY";
        case DisplayMode::LIGHT:       return "LIGHT";
        case DisplayMode::MOTION:      return "MOTION";
        default: return "UNKNOWN";
    }
}

void nextDisplayMode() {
    int mode = (int)currentDisplayMode;
    mode++;
    if (mode > (int)DisplayMode::MOTION) {
        mode = (int)DisplayMode::TEMPERATURE;
    }
    currentDisplayMode = (DisplayMode)mode;
}

void previousDisplayMode() {
    int mode = (int)currentDisplayMode;
    mode--;
    if (mode < (int)DisplayMode::TEMPERATURE) {
        mode = (int)DisplayMode::MOTION;
    }
    currentDisplayMode = (DisplayMode)mode;
}

