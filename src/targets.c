#include <mega65/memory.h>
#include <mega65/targets.h>

uint8_t detect_target(void)
{
    // We use the different I2C device blocks to identify the various hardware
    // targets
    return lpeek(0xffd3629);
}
