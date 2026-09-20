#include "sensors.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"

static gpio_num_t dht_pin;

void dht22_init(gpio_num_t pin) {
    dht_pin = pin;
    // configure as open drain so we can both read and write without changing direction
    gpio_set_direction(dht_pin, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(dht_pin, GPIO_PULLUP_ONLY);
    gpio_set_level(dht_pin, 1); // idle high
}

/* helper to wait for a specific pin state */
static int wait_for_state(int state, int timeout_us) {
    int count = 0;
    while (gpio_get_level(dht_pin) != state) {
        if (count++ > timeout_us) {
            return -1;
        }
        ets_delay_us(1);
    }
    return count;
}

bool dht22_read(float *temperature, float *humidity) {
    uint8_t data[5] = {0};

    // send start signal
    gpio_set_level(dht_pin, 0);
    ets_delay_us(2000); // 2ms start signal
    gpio_set_level(dht_pin, 1);
    ets_delay_us(30);

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&mux);

    // wait for response (low ~80us, then high ~80us)
    if (wait_for_state(0, 80) == -1) { taskEXIT_CRITICAL(&mux); return false; }
    if (wait_for_state(1, 80) == -1) { taskEXIT_CRITICAL(&mux); return false; }
    if (wait_for_state(0, 80) == -1) { taskEXIT_CRITICAL(&mux); return false; }

    // read 40 bits
    for (int i = 0; i < 40; i++) {
        if (wait_for_state(1, 80) == -1) { taskEXIT_CRITICAL(&mux); return false; }
        int dur = wait_for_state(0, 80);
        if (dur == -1) { taskEXIT_CRITICAL(&mux); return false; }
        
        data[i / 8] <<= 1;
        // high duration > 40us means 1, otherwise 0
        if (dur > 40) {
            data[i / 8] |= 1;
        }
    }

    taskEXIT_CRITICAL(&mux);

    // verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        return false;
    }

    // parse the data
    uint16_t raw_hum = (data[0] << 8) | data[1];
    uint16_t raw_temp = (data[2] << 8) | data[3];

    *humidity = raw_hum * 0.1f;
    
    if (raw_temp & 0x8000) {
        *temperature = -(raw_temp & 0x7FFF) * 0.1f;
    } else {
        *temperature = raw_temp * 0.1f;
    }

    return true;
}

