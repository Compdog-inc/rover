#ifndef STATUS_H
#define STATUS_H

#include "framework.h"

bool watchdog_reset();
bool brownout_reset();
bool external_reset();
bool poweron_reset();

#endif