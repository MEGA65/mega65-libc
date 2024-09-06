/**
 * @file debug.h
 * @brief Debugging functions for serial output
 */
#ifndef __MEGA65_DEBUG_H
#define __MEGA65_DEBUG_H

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/**
 * @brief Write debug message to serial monitor
 * @param msg Text message to write
 */
void debug_msg(char* msg);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_DEBUG_H
