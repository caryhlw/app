#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "adc.h"
#include "led.h"
#include "button.h"
#include "ble.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_main, LOG_LEVEL);

int main(void)
{
        int rc;
        LOG_INF("Application started");

#ifdef CONFIG_ADC
        rc = adc_init();
        LOG_DBG("ADC returned %d", rc);
#endif

#ifdef CONFIG_LED
        rc = led_init();
        LOG_DBG("LED returned %d", rc);
#endif

#ifdef CONFIG_BUTTON
        rc = button_init();
        LOG_DBG("Button returned %d", rc);
#endif

#ifdef CONFIG_BT
        rc = ble_init();
        LOG_DBG("BLE returned %d", rc);
#endif

        return 0;
}
