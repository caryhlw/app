#ifdef CONFIG_LED

#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include "led.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_led, LOG_LEVEL);

#define DEVICE_NODE DT_ALIAS(led_status)
static const struct device *dev;

int led_init(void)
{
    int rc;

    dev = DEVICE_DT_GET(DEVICE_NODE);
    if (dev == NULL)
    {
        LOG_WRN("Failed to get LED binding");
        rc = -ENXIO;
    }
    else
    {
        LOG_DBG("Configuring LED");
    }

    return rc;
}

#endif
