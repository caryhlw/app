
#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "led.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_led, LOG_LEVEL);

#ifdef CONFIG_PWM
#define DEVICE_NODE DT_ALIAS(led_status)
static const struct pwm_dt_spec led_pwm = PWM_DT_SPEC_GET(DEVICE_NODE);
#endif

int led_init(void)
{
    int rc;
    
    LOG_DBG("Finding LEDs...");
#ifdef CONFIG_PWM
    if (pwm_is_ready_dt(&led_pwm))
    {
    }
    else
    {
        LOG_WRN("Failed to get LED binding");
        rc = -ENODEV;
    }
#endif
    
    return rc;
}
