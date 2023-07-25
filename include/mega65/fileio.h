#ifndef __MEGA65_FILEIO_H
#define __MEGA65_FILEIO_H

#include <stdint.h>
#include <stddef.h>

void toggle_rom_write_protect(void);

/** Closes all open files */
void closeall(void);

/**
 * @brief Close a single file
 * @param fd File descriptor pointing to file to close
 */
void close(uint8_t fd);

/**
 * @brief Open file
 * @param filename to open
 * @return File descriptor or `0xff` if error
 */
uint8_t open(char* filename);

/**
 * @brief Read up to 512 bytes from file
 * @param buffer Input buffer
 * @return Number of bytes read
 */
size_t read512(uint8_t* buffer);

/**
 * @brief Change working directory
 * @param filename Directory name
 * @return Error code (currently unused)
 * @note Only accepts one directory segment at a time
 */
uint8_t chdir(char* filename);

/**
 * @brief Change working directory to the root directory
 * @return Error code (currently unused)
 */
uint8_t chdirroot(void);

#endif // __MEGA65_FILEIO_H
