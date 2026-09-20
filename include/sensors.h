#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"

/* initialize the DHT22 sensor */
void dht22_init(gpio_num_t pin);

/* read temperature and humidity from DHT22 */
bool dht22_read(float *temperature, float *humidity);

#endif // SENSORS_H

