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
unsigned char opendir(void) __attribute__((leaf));
#else
unsigned char opendir(void);
#endif

/// Read directory entry
#ifdef __clang__
struct m65_dirent* readdir(unsigned char) __attribute__((leaf));
#else
struct m65_dirent* readdir(unsigned char);
#endif

/// Close directory entry
#ifdef __clang__
void closedir(unsigned char) __attribute__((leaf));
#else
void closedir(unsigned char);
#endif

/// Structure describing an open directory
struct m65_dirent {
    uint32_t d_ino;    //!< File number of entry
    uint16_t d_off;    //!< Offset
    uint32_t d_reclen; //!< Length of this record
    uint16_t d_type;   //!< File type
    char d_name[256];  //!< Filename string of entry
};

#endif // __MEGA65_DIRENT_H
