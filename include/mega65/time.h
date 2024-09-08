// Copyright 2023 The MEGA65 project
// Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
// See https://github.com/mega65/mega65-libc/blob/main/LICENSE for license
// information.

/**
 * @file time.h
 * @brief Real time clock support
 *
 * The MEGA65 really only has a Real-Time Clock (RTC), so we just have handy
 * functions for that.
 *
 * The RTC varies between revisions of the MEGA65, though, so we have to take
 * that into account.
 */
#ifndef __MEGA65_TIME_H
#define __MEGA65_TIME_H

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/**
 * @brief Real-time clock structure
 */
#ifdef __clang__
struct __attribute__((__packed__)) m65_tm {
#else
struct m65_tm {
#endif
    unsigned char tm_sec;   //!< Seconds (0-60)
    unsigned char tm_min;   //!< Minutes (0-59)
    unsigned char tm_hour;  //!< Hours (0-23)
    unsigned char tm_mday;  //!< Day of the month (1-31)
    unsigned char tm_mon;   //!< Month (0-11)
    unsigned short tm_year; //!< Year - 1900 (in practice, never < 2000)
    unsigned char tm_wday;  //!< Day of the week (0-6, Sunday = 0)
    int tm_yday;            //!< Day in the year (0-365, 1 Jan = 0)
    unsigned char tm_isdst; //!< Daylight saving time
};

/// Get real-time clock
void getrtc(struct m65_tm* tm);
/// Set real-time clock
void setrtc(struct m65_tm* tm);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_TIME_H
