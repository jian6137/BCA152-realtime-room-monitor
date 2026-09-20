#ifndef DISPLAY_H
#define DISPLAY_H

#include "frtos_objects.h"

/* initialize I2C and OLED display */
void display_init();

/* update the display based on current menu and sensor data */
void display_update(const SensorData* data, const char* activeMenu);

#endif // DISPLAY_H

