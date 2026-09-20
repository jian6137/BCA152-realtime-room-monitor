#include "motion.h"
#include "driver/gpio.h"

#define PIR_PIN GPIO_NUM_14

void motion_init() {
    gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIR_PIN, GPIO_PULLDOWN_ONLY);
}

bool motion_detect() {
    return gpio_get_level(PIR_PIN) == 1;
}

