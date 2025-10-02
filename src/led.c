
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
#else
#warning "LED PWM not configured; using simulated LEDs"
#endif

#define THREAD_SIZE 512
#define THREAD_PRIORITY 7
static struct k_thread thread;
static void led_thread(void* p1, void* p2, void* p3);

#define SEM_COUNT_INIT 0
#define SEM_COUNT_LIMIT 1
#define SEM_TIMEOUT_MS 100
static struct k_sem blink_sem;

#define BLINK_IDLE_FREQUENCY_HZ 1
#define BLINK_SAMPLING_FREQUENCY_HZ 2
#define BLINK_ERROR_FREQUENCY_HZ 3
static struct k_timer blink_timer;
static int blink(int frequency_hz);
static void blink_timer_handler(struct k_timer* timer_id);

enum states {
    STATE_IDLE,
    STATE_SAMPLING,
    STATE_ERROR
};
static enum states state;

K_THREAD_STACK_DEFINE(led_stack, THREAD_SIZE);

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

    // Start state machine regardless of device readiness for simulation
    LOG_DBG("Initializing semaphores, timers, thread, etc...");
    k_sem_init(&blink_sem, SEM_COUNT_INIT, SEM_COUNT_LIMIT);
    k_timer_init(&blink_timer, blink_timer_handler, NULL);
    k_thread_create(&thread, led_stack, THREAD_SIZE, led_thread, NULL, NULL, NULL, THREAD_PRIORITY, 0, K_NO_WAIT);
    rc = led_event_dispatch(EVENT_IDLE);
    
    return rc;
}

int led_event_dispatch(enum event event)
{
    int rc;

    LOG_DBG("Handling event %d...", event);
    switch(event)
    {
        case EVENT_IDLE:
            rc = blink(BLINK_IDLE_FREQUENCY_HZ);
            state = STATE_IDLE;
            break;
        case EVENT_SAMPLE:
            rc = blink(BLINK_SAMPLING_FREQUENCY_HZ);
            state = STATE_SAMPLING;
            break;
        case EVENT_ERROR:
            rc = blink(BLINK_ERROR_FREQUENCY_HZ);
            state = STATE_ERROR;
            break;
        default:
            rc = -EINVAL;
            break;
    }
    return rc;
}

static void led_thread(void* p1, void* p2, void* p3)
{
    static bool led_on = false;
    while(1)
    {
#ifdef CONFIG_PWM
        if (k_sem_take(&blink_sem, K_FOREVER) == 0)
        {
            switch(state)
            {
                case STATE_IDLE:
                    pwm_set_pulse_dt(&led_pwm, 1000);
                    break;
                case STATE_SAMPLING:
                    pwm_set_pulse_dt(&led_pwm, 1000);
                    k_sleep(1);
                    led_event_dispatch(EVENT_IDLE);
                    break;
                case STATE_ERROR:
                    pwm_set_pulse_dt(&led_pwm, 1000);
                    break;
                default:
                    rc = -EINVAL;
            }
        }
#else
        // Manually drive blink using timer if PWM not available
        if (k_sem_take(&blink_sem, K_MSEC(SEM_TIMEOUT_MS)) == 0)
        {
            if(led_on)
            {
                LOG_DBG("Turning LED off...");
            }
            else
            {
                LOG_DBG("Turning LED on...");
            }
            led_on = !led_on;
        }
#endif
    }
}
static int blink(int frequency_hz)
{
    int rc;

#ifdef CONFIG_PWM
    LOG_DBG("Deferring PWM configuration to thread...");
    k_sem_give(&blink_sem);
#else
    // Manually drive blink using timer if PWM not available
    k_timer_stop(&blink_timer);
    k_sem_reset(&blink_sem);
    LOG_DBG("Starting blink timer...");
    k_timer_start(&blink_timer, K_NO_WAIT, K_SECONDS(1/frequency_hz));
    rc = 0;
#endif
    return rc;
}

static void blink_timer_handler(struct k_timer *timer_id)
{
    LOG_DBG("Deferring LED toggle to thread...");
    k_sem_give(&blink_sem);
}
