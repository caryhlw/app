#ifdef CONFIG_GPIO

#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "button.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_buttons, LOG_LEVEL);

#define DEVICE_NODE DT_ALIAS(btn_user)
static const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET(DEVICE_NODE, gpios);
static struct gpio_callback button_cb_data;

static int int_configure(const struct gpio_dt_spec* gpio);
static void int_callback(const struct device* dev, struct gpio_callback* cb, uint32_t pins);

int button_init(void)
{
    int rc;
    
    LOG_DBG("Finding button...");
    if (gpio_is_ready_dt(&gpio))
    {
        LOG_INF("Button found");
        LOG_DBG("Configuring button...");
        gpio_pin_configure_dt(&gpio, GPIO_INPUT);
        rc = int_configure(&gpio);
    }
    else
    {
        LOG_WRN("Failed to get button binding");
        rc = -ENODEV;
    }
    
    return rc;
}

static int int_configure(const struct gpio_dt_spec* gpio)
{
    int rc;

    LOG_DBG("Configuring interrupt mode...");
    rc = gpio_pin_interrupt_configure_dt(gpio, GPIO_INT_EDGE_TO_ACTIVE);
    if (rc == 0)
    {
        LOG_DBG("Initializing interrupt callback on pin...");
        gpio_init_callback(&button_cb_data, int_callback, BIT(gpio->pin));
        
        LOG_DBG("Adding interrupt callback...");
        rc = gpio_add_callback(gpio->port, &button_cb_data);
    }
    else
    {
        LOG_WRN("Failed to configure interrupt");
    }

    return rc;
}

static void int_callback(const struct device* dev, struct gpio_callback* cb, uint32_t pins)
{
    // ble_disable_notif in an event (do not do work directly!)
    return;
}

#endif