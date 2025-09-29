#ifdef CONFIG_ADC

#include <errno.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include <zephyr/drivers/adc.h>
#include <nrfx_saadc.h>

#include "adc.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_adc, LOG_LEVEL);

#define ADC_DEVICE DT_ALIAS(adc)
static const struct device *dev;

#define ADC_GAIN NRF_SAADC_GAIN1_3
#define ADC_ACQ_TIME_US 20

static struct adc_channel_cfg config;

int adc_init(void)
{
    int rc;

    LOG_DBG("Initializing ADC");

    dev = DEVICE_DT_GET(ADC_DEVICE);
    if (dev == NULL)
    {
        LOG_WRN("Failed to get ADC bindings");
        rc = -ENXIO;
    }
    else
    {
        config.gain = ADC_GAIN;
        config.reference = ADC_REF_INTERNAL;
        config.acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, ADC_ACQ_TIME_US);
        config.channel_id = 1;
        config.differential = NRF_SAADC_MODE_SINGLE_ENDED;
        config.input_positive = NRF_SAADC_INPUT_AIN0;
        LOG_DBG("Setting up ADC");
        rc = adc_channel_setup(dev, &config);
        LOG_DBG("ADC channel setup returned %d", rc);
    }

    return rc;
}

#endif