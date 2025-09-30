#ifdef CONFIG_GPIO

#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include "button.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_buttons, LOG_LEVEL);

#define DEVICE_NODE DT_ALIAS(btn_user)
static const struct device *dev;

int button_init(void)
{
    int rc;

    LOG_DBG("Initializing button");

    dev = DEVICE_DT_GET(DEVICE_NODE);
    if (dev == NULL)
    {
        LOG_WRN("Failed to get button binding");
        rc = -ENXIO;
    }
    else
    {
        LOG_DBG("Configuring button");
    }

    return rc;
}

#endif