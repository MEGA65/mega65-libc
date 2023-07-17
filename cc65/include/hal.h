#ifndef __MEGA65_HAL_H
#define __MEGA65_HAL_H

#ifdef __CC65__
#include <sys/types.h>
#include <ctype.h>
#endif
#include <stdint.h>

void usleep(uint32_t micros);

#endif
