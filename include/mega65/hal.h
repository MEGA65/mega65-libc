/**
 * @file hal.h
 * @brief Timing functions ala unistd.h
 */
#ifndef __MEGA65_HAL_H
#define __MEGA65_HAL_H

#include <stdint.h>

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/**
 * @brief Sleep for the given number of microseconds
 * @param micros Microseconds to sleep
 */
void usleep(uint32_t micros);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_HAL_H
