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

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/// Open a directory
#ifdef __clang__
__attribute__((leaf))
#endif
unsigned char
opendir(void);

/// Read directory entry
#ifdef __clang__
__attribute__((leaf))
#endif
struct m65_dirent*
readdir(unsigned char);

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

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_DIRENT_H
