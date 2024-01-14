// Copyright 2023 The MEGA65 project
// Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
// See https://github.com/mega65/mega65-libc/blob/main/LICENSE for license
// information.

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
