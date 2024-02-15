#include "framework.h"
#include "status.h"

bool watchdog_reset()
{
    return MCUSR & (1 << WDRF);
}

bool brownout_reset()
{
    return MCUSR & (1 << BORF);
}

bool external_reset()
{
    return MCUSR & (1 << EXTRF);
}

bool poweron_reset()
{
    return MCUSR & (1 << PORF);
}