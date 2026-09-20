#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"

/* initialize the DHT22 sensor */
void dht22_init(gpio_num_t pin);

/* read temperature and humidity from DHT22 */
bool dht22_read(float *temperature, float *humidity);

/* initialize the LDR sensor */
void ldr_init();

/* read ambient light level from LDR as a percentage (0-100) */
int ldr_read_percentage();

#endif // SENSORS_H

