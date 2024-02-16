#ifndef STATUS_H
#define STATUS_H

#include "framework.h"

/// @brief Returns true if reset by watchdog
bool watchdog_reset();

/// @brief Returns true if reset by brownout
bool brownout_reset();

/// @brief Returns true if reset externally
bool external_reset();

/// @brief Returns true if reset by power on
bool poweron_reset();

#endif