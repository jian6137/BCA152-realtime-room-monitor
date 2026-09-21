#ifndef MOCK_GPIO_H
#define MOCK_GPIO_H

#define GPIO_NUM_13 13
#define GPIO_MODE_OUTPUT 1

#ifdef __cplusplus
extern "C" {
#endif

inline void gpio_set_direction(int pin, int mode) {}
inline void gpio_set_level(int pin, int level) {}

#ifdef __cplusplus
}
#endif

#endif

