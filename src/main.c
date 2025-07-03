#include <stdio.h>
#include <stdint.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include "say_hello.h"

// Settings
static const int32_t sleep_time_ms = 100;
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

static int read(uint8_t devaddr, uint8_t regaddr, uint8_t *regval)
{
	int ret;

	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));

	if (!device_is_ready(dev))
	{
		// LOG_ERR("Device not ready");
		return -ENODEV;
	}

	ret = i2c_write_read(dev, devaddr, &regaddr, 1, regval, 1);
	if (ret)
	{
		// LOG_ERR("Call `i2c_write_read` failed: %d", ret);
		return ret;
	}

	return 0;
}

static int write(uint8_t devaddr, uint8_t regaddr, uint8_t regval)
{
	int ret;

	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));

	if (!device_is_ready(dev))
	{
		// LOG_ERR("Device not ready");
		return -ENODEV;
	}

	uint8_t buf[2] = {regaddr, regval};

	ret = i2c_write(dev, buf, 2, devaddr);
	if (ret)
	{
		// LOG_ERR("Call `i2c_write` failed: %d", ret);
		return ret;
	}

	return 0;
}

int main(void)
{
	int ret;
	int state = 0;
	int button_state = 0, last_button_state = 0;
	uint32_t rnd;
	double dbl;

	// Make sure that the GPIO was initialized
	if (!gpio_is_ready_dt(&led))
	{
		return 0;
	}

	// Make sure that the button was initialized
	if (!gpio_is_ready_dt(&btn))
	{
		printk("ERROR: button not ready\r\n");
		return 0;
	}

	// Set the button as input (apply extra flags if needed)
	ret = gpio_pin_configure_dt(&btn, GPIO_INPUT);
	if (ret < 0)
	{
		return 0;
	}

	// Set the GPIO as output
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0)
	{
		return 0;
	}

	printk("Up and running I think!");

	// Do forever
	while (1)
	{

		// Change the state of the pin and print
		state = !state;
		rnd = sys_rand32_get();
		dbl = (double)rnd / UINT32_MAX;

		// printk("LED state: %d\r\n", state);
		// printk("Random value: %u\r\n", rnd);
		// printk("Random double value: %f\r\n", dbl);

		// say_hello();

		// Set pin state
		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0)
		{
			return 0;
		}

		// Poll button state
		button_state = gpio_pin_get_dt(&btn);
		if (button_state < 0)
		{
			printk("Error %d: failed to read button pin\r\n", button_state);
		}
		else if (button_state != last_button_state)
		{
			// printk("Button state: %d\r\n", button_state);

			if (button_state == 0)
			{
				// Button pressed
				write(0x60, 0x10, 0x0c); // Turn on clock1
				write(0x60, 0x11, 0x0c); // Turn on clock1
			}
			else
			{
				// Button released
				write(0x60, 0x10, 0x8c); // Turn off clock1
				write(0x60, 0x11, 0x8c); // Turn off clock1
			}

			last_button_state = button_state;
		}

		// Sleep
		k_msleep(sleep_time_ms);
	}

	return 0;
}
