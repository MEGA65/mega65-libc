/**
 * @file hal.h
 * @brief Timing functions ala unistd.h
 */
#ifndef __MEGA65_HAL_H
#define __MEGA65_HAL_H

#include <stdint.h>

/**
 * @brief Sleep for the given number of microseconds
 * @param micros Microseconds to sleep
 */
void usleep(uint32_t micros);

#endif // __MEGA65_HAL_H
