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

#ifdef __clang__
void toggle_rom_write_protect(void) __attribute__((leaf));
#else
void toggle_rom_write_protect(void);
#endif

/**
 * @brief Closes all open files
 */
#ifdef __clang__
void closeall(void) __attribute__((leaf));
#else
void closeall(void);
#endif

/**
 * @brief Close a single file
 * @param fd File descriptor pointing to file to close
 */
#ifdef __clang__
void close(uint8_t fd) __attribute__((leaf));
#else
void close(uint8_t fd);
#endif

/**
 * @brief Open file
 * @param filename to open
 * @return File descriptor or `0xff` if error
 */
#ifdef __clang__
uint8_t open(char* filename) __attribute__((leaf));
#else
uint8_t open(char* filename);
#endif

/**
 * @brief Read up to 512 bytes from file
 * @param buffer Input buffer
 * @return Number of bytes read
 */
#ifdef __clang__
size_t read512(uint8_t* buffer) __attribute__((leaf));
#else
size_t read512(uint8_t* buffer);
#endif

/**
 * @brief Change working directory
 * @param filename Directory name
 * @return Error code (currently unused)
 * @note Only accepts one directory segment at a time
 */
#ifdef __clang__
uint8_t chdir(char* filename) __attribute__((leaf));
#else
uint8_t chdir(char* filename);
#endif

/**
 * @brief Change working directory to the root directory
 * @return Error code (currently unused)
 */
#ifdef __clang__
uint8_t chdirroot(void) __attribute__((leaf));
#else
uint8_t chdirroot(void);
#endif

/**
 * @brief Struct for holding version information of the hypervisor
 *
 * The HDOS in Hyppo is not related to the CBDOS inside the Kernal
 * or to the DOS in the disk drive units attached to the serial port.
 */
struct hyppo_version {
    uint8_t hyppo_major;
    uint8_t hyppo_minor;
    uint8_t hdos_major;
    uint8_t hdos_minor;
};

/**
 * @brief Get the version of the hypervisor
 * @param version Pointer to a `struct hyppo_version` to fill
 */
#ifdef __clang__
void gethyppoversion(struct hyppo_version* version) __attribute__((leaf));
#else
void gethyppoversion(struct hyppo_version* version);
#endif

#endif // __MEGA65_FILEIO_H
