/**
 * @file dirent.h
 * @brief Directory entry functions
 */

#ifndef __MEGA65_DIRENT_H
#define __MEGA65_DIRENT_H

#include <stdint.h>

/// Open a directory
unsigned char opendir(void);

/// Read directory entry
struct m65_dirent* readdir(unsigned char);

/// Close directory entry
void closedir(unsigned char);

/// Structure describing an open directory
#ifdef __clang__
struct __attribute__((__packed__)) m65_dirent {
#else
struct m65_dirent {
#endif
    uint32_t d_ino;    //!< File number of entry
    uint16_t d_off;    //!< Offset
    uint32_t d_reclen; //!< Length of this record
    uint16_t d_type;   //!< File type
    char d_name[256];  //!< Filename string of entry
};

#endif // __MEGA65_DIRENT_H
