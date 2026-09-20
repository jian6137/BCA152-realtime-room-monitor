#ifndef MOTION_H
#define MOTION_H

/* Initialize PIR sensor */
void motion_init();

/* Returns true if motion is currently detected */
bool motion_detect();

#endif // MOTION_H

