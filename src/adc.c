#ifdef CONFIG_ADC

#include <errno.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <nrfx_saadc.h>

#include "adc.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_adc, LOG_LEVEL);

#define APP DT_PATH(app)
#define SAMPLE_INTERVAL_MS DT_PROP(APP, sample_interval_ms)

#define ADC_DEVICE DT_ALIAS(adc)
static const struct device *dev;
static struct adc_channel_cfg config;

#define TIMER_START_DELAY_S 0
static struct k_timer adc_timer;
static void adc_timer_handler();

static struct k_work_delayable dwork;
static void work(struct k_work *work);

static uint8_t voltage_mv;
static int sample_interval_ms = SAMPLE_INTERVAL_MS;

static int configure();
static int sample();

int adc_init(void)
{
    int rc;

    LOG_DBG("Finding ADCs...");
    dev = DEVICE_DT_GET(ADC_DEVICE);
    if (dev == NULL)
    {
        LOG_WRN("Failed to get ADC bindings");
        rc = -ENXIO;
    }
    else
    {
        LOG_INF("ADC found");
        rc = configure();
        if (rc == 0)
        {
            LOG_INF("ADC configured");
            LOG_DBG("Initilizing timer and worker...");
            k_timer_init(&adc_timer, adc_timer_handler, NULL);
            k_work_init_delayable(&dwork, (k_work_handler_t)work);
        }
        else
        {
            LOG_WRN("ADC configuration failed (%d)", rc);
        }
    }

    if (rc == 0)
    {
        LOG_INF("ADC initilization complete");
        LOG_DBG("Starting timer with %ds delay...", TIMER_START_DELAY_S);
        k_timer_start(&adc_timer, K_SECONDS(TIMER_START_DELAY_S), K_MSEC(sample_interval_ms));
    }
    
    return rc;
}

int adc_voltage_get(void)
{
    return voltage_mv;
}

int adc_sample_interval_get(void)
{
    return sample_interval_ms;
}

int configure()
{
    int rc;
        LOG_DBG("Configuring ADCs...");
        config.gain = ADC_GAIN_1_6;
        config.reference = ADC_REF_INTERNAL;
        config.acquisition_time = ADC_ACQ_TIME_DEFAULT;
        config.channel_id = 0;
        config.differential = NRF_SAADC_MODE_SINGLE_ENDED;
#if CONFIG_ADC_CONFIGURABLE_INPUTS
        config.input_positive = NRF_SAADC_INPUT_AIN0;
#endif
        rc = adc_channel_setup(dev, &config);
        return rc;
}

static int sample()
{
    int rc;

    // struct adc_sequence *sp = &adc_chat_ptr->adc_seq;
    // rc = adc_read(adc_chan_ptr->adc, sp);

    if(rc == 0)
    {
        // voltage_mv = adc_chan_ptr->raw;
    }
    else
    {
        // Mock for now
        voltage_mv = 2200;

    }

    return rc;
}

static void adc_timer_handler()
{
    LOG_DBG("Dispatching work...");
    k_work_submit(&dwork);
}

static void work(struct k_work *work)
{
    int rc;
    LOG_DBG("Sampling VDD...");
    sample();

    if (rc == 0)
    {
        LOG_DBG("Sample success...notifying...");
        ble_voltage_update(voltage_mv);
    }
    else
    {
        LOG_ERR("Failed to get sample");
    }
}

#endif