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
        LOG_DBG("Starting application...");

#ifdef CONFIG_ADC
        LOG_DBG("Starting battery monitor...");
        rc = adc_init();
        if (rc == 0)
        {
                LOG_INF("ADC initialization success");
        }
        else
        {
                LOG_WRN("ADC initialization failed (%d)", rc);
        }
#endif

#ifdef CONFIG_LED
        LOG_DBG("Starting LEDs...");
        rc = led_init();
        if (rc == 0)
        {
                LOG_INF("LED initialization success");
        }
        else
        {
                LOG_WRN("LED initialization failed (%d)", rc);
        }
#endif

#ifdef CONFIG_BUTTON
        LOG_DBG("Starting button handler...");
        rc = button_init();
        if (rc == 0)
        {
                LOG_INF("Button initialization success");
        }
        else
        {
                LOG_WRN("Button initialization failed (%d)", rc);
        }
#endif

#ifdef CONFIG_BT
        LOG_DBG("Starting BLE...");
        rc = ble_init();
        if (rc == 0)
        {
                LOG_INF("BLE initialization success");
        }
        else
        {
                LOG_WRN("BLE initialization failed (%d)", rc);
        }
#endif

        return 0;
}
