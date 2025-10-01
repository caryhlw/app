#pragma once

#include "events.h"

int led_init(void);
int led_event_dispatch(enum event event);
