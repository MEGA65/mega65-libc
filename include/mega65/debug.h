// Copyright 2023 The MEGA65 project
// Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
// See https://github.com/mega65/mega65-libc/blob/main/LICENSE for license
// information.

/**
 * @file debug.h
 * @brief Debugging functions for serial output
 */
#ifndef __MEGA65_DEBUG_H
#define __MEGA65_DEBUG_H

/**
 * @brief Write debug message to serial monitor
 * @param msg Text message to write
 */
void debug_msg(char* msg);

#endif // __MEGA65_DEBUG_H
