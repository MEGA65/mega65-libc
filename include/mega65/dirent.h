// Copyright 2023 The MEGA65 project
// Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
// See https://github.com/mega65/mega65-libc/blob/main/LICENSE for license
// information.

/**
 * @file dirent.h
 * @brief Directory entry functions
 *
 * If in C64 mode, you must call `mega65_io_enable()` before using these
 * functions.
 */

#ifndef __MEGA65_DIRENT_H
#define __MEGA65_DIRENT_H

#include <stdint.h>

/// Open a directory
#ifdef __clang__
__attribute__((leaf))
#endif
unsigned char opendir(void);

/// Read directory entry
#ifdef __clang__
__attribute__((leaf))
#endif
struct m65_dirent* readdir(unsigned char);

/// Close directory entry
#ifdef __clang__
__attribute__((leaf))
#endif
void closedir(unsigned char);

/// Structure describing an open directory
struct m65_dirent {
    uint32_t d_ino;    //!< File number of entry
    uint16_t d_off;    //!< Offset
    uint32_t d_reclen; //!< Length of this record
    uint16_t d_type;   //!< File type
    char d_name[256];  //!< Filename string of entry
};

#endif // __MEGA65_DIRENT_H
