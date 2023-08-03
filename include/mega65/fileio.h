/**
 * @file fileio.h
 * @brief File I/O functions
 *
 * If in C64 mode you must call `mega65_io_enable()` found in `memory.h`
 * before using any of the fileio functions.
 */

#ifndef __MEGA65_FILEIO_H
#define __MEGA65_FILEIO_H

#include <stdint.h>
#include <stddef.h>

void toggle_rom_write_protect(void);

/**
 * @brief Closes all open files
 */
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

/**
 * @brief Struct for holding version information of the hypervisor
 *
 * The HDOS in Hyppo is not related to the CBDOS inside the Kernal
 * or to the DOS in the disk drive units attached to the serial port.
 */
struct hyppo_version {
    uint8_t hdos_major;
    uint8_t hyppo_major;
    uint8_t hyppo_minor;
    uint8_t hdos_minor;
};

/**
 * @brief Get the version of the hypervisor
 * @param version Pointer to a `struct hyppo_version` to fill
 */
void gethyppoversion(struct hyppo_version* version);

#endif // __MEGA65_FILEIO_H
