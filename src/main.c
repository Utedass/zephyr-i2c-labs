#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include "say_hello.h"
#include <stdint.h>

// Settings
static const int32_t sleep_time_ms = 1000;
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

int main(void)
{
	int ret;
	int state = 0;
	uint32_t rnd;
	double dbl;

	// Make sure that the GPIO was initialized
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	// Set the GPIO as output
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}

	// Do forever
	while (1) {

		// Change the state of the pin and print
		state = !state;
		rnd = sys_rand32_get();
		dbl = (double)rnd / UINT32_MAX;

		printk("LED state: %d\r\n", state);
		printk("Random value: %u\r\n", rnd);
		printk("Random double value: %f\r\n", dbl);

		say_hello();

		// Set pin state
		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			return 0;
		}

		// Sleep
		k_msleep(sleep_time_ms);
	}

	return 0;
}
