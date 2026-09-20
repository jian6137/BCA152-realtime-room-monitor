#ifndef INPUT_H
#define INPUT_H

#include "frtos_objects.h"

/* Returns the string representation of a display mode */
const char* get_display_mode_str(DisplayMode mode);

/* Go to the next display mode (clockwise) */
void nextDisplayMode();

/* Go to the previous display mode (counterclockwise) */
void previousDisplayMode();

#endif // INPUT_H

