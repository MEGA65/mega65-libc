#include <memory.h>
#include <targets.h>

unsigned char detect_target(void)
{
  // We use the different I2C device blocks to identify the various hardware targets
  
  if (lpeek(0xffd7000)||lpeek(0xffd7001)||
      lpeek(0xffd7002)||lpeek(0xffd7004))
    return TARGET_MEGAPHONER1;

  if (lpeek(0xffd7100)||lpeek(0xffd7101)||
      lpeek(0xffd7102)||lpeek(0xffd7104))
    return TARGET_MEGA65R2;

  return TARGET_UNKNOWN;
}
