#ifndef __MEGA65_DIRENT_H
#define __MEGA65_DIRENT_H

#include <stdint.h>

unsigned char opendir(void);
struct m65_dirent* readdir(unsigned char);
void closedir(unsigned char);

#ifdef __clang__
struct __attribute__((__packed__)) m65_dirent {
#else
struct m65_dirent {
#endif
    uint32_t d_ino;
    uint16_t d_off;
    uint32_t d_reclen;
    uint16_t d_type;
    char d_name[256];
};

#endif // __MEGA65_DIRENT_H
