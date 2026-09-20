#ifndef DISPLAY_H
#define DISPLAY_H

#include "frtos_objects.h"

/* initialize I2C and OLED display */
void display_init();

/* Clear the OLED display */
void display_clear();

/* Update the OLED with sensor data and menu state */
void display_update(const SensorData* data, const char* activeMenu);

#endif // DISPLAY_H
