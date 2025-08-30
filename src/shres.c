/**
 * @file shres.c
 * @brief Implementation of MEGA65 SYSPART Shared Resources Access API
 *
 * Provides functions to access shared resources stored in the MEGA65 system
 * partition (SYSPART), including file opening, reading, seeking, and directory
 * traversal. Resources are accessed via a custom trap interface to the SYSPART
 * filesystem.
 */

#include <stdio.h>
#include <string.h>

#include "mega65/shres.h"
#include "mega65/memory.h"

/// Magic string identifying the SYSPART shared resource area.
static const unsigned char magic_string[]
    = { 0x4D, 0x45, 0x47, 0x41, '6', '5',                       // "MEGA65"
          0x53, 0x48, 0x41, 0x52, 0x45, 0x44,                   // "SHARED"
          0x52, 0x45, 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x53, // "RESOURCES"
          0x00 };

/**
 * @brief Waits until the SD card is no longer busy.
 */
void sdcard_busy_wait(void)
{
    while (PEEK(0xD680) & 0x03) {
        continue;
    }
}

/**
 * @brief Executes the shared resource trap for a given sector number.
 *
 * @param arg The sector number to read (as a 32-bit address).
 * @return 0 on success, 1 if the SD card is busy or the trap fails.
 */
char do_shres_trap(unsigned long arg)
{
    if (PEEK(0xD680) & 0x03) {
        return 1;
    }

    shres_regs[0] = (arg >> 0) & 0xff;
    shres_regs[1] = (arg >> 8) & 0xff;
    shres_regs[2] = (arg >> 16) & 0xff;
    shres_regs[3] = (arg >> 24) & 0xff;
    shres_trap();

    // Success is indicated by bit 0 = 1 in shres_regs[4]
    return (shres_regs[4] & 0x01) ^ 0x01;
}

/**
 * @brief Opens a shared resource file by name and flags.
 *
 * @param resource_name The null-terminated name of the resource to open.
 * @param required_flags Bitmask of required flags the resource must have.
 * @param file_handle Pointer to the shared_resource struct to populate.
 * @return 0 on success, 1 if the resource was not found or an error occurred.
 */
char shopen(char* resource_name, unsigned long required_flags,
    struct shared_resource* file_handle)
{
    unsigned int d = shdopen();
    if (d == 0xffff) {
        return 1;
    }

    while (!shdread(required_flags, &d, file_handle)) {
        if (!strcmp(resource_name, file_handle->name)) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Reads bytes from an open shared resource.
 *
 * @param ptr Pointer to the output buffer to fill.
 * @param count Number of bytes to read.
 * @param f Pointer to an open shared_resource structure.
 * @return Number of bytes actually read.
 */
unsigned int shread(
    unsigned char* ptr, unsigned int count, struct shared_resource* f)
{
    unsigned int read_bytes = 0;

    if (!f || f->position >= f->length) {
        return 0;
    }

    if (count > (f->length - f->position)) {
        count = (f->length - f->position);
    }

    while (count > 0) {
        unsigned int bytes = 512 - (f->position & 511);
        if (bytes > count) {
            bytes = count;
        }

        // Load the appropriate sector
        if (do_shres_trap(f->first_sector + (f->position >> 9))) {
            break;
        }
	// And wait for the SD card to actually read it.
	sdcard_busy_wait();

	// Select SD card buffer, not FDC buffer
	POKE(0xD689L,PEEK(0xD689L)|0x80);
	
        // Copy bytes from the sector buffer to the output buffer
        lcopy(0xffd6e00L + (f->position & 511), (unsigned long)ptr, bytes);

        ptr += bytes;
        f->position += bytes;
        count -= bytes;
        read_bytes += bytes;
    }

    return read_bytes;
}

/**
 * @brief Seeks to a specific byte offset in an open shared resource.
 *
 * @param f Pointer to an open shared_resource structure.
 * @param offset Byte offset to seek.
 * @param whence One of SEEK_SET, SEEK_CUR, or SEEK_END.
 * @return 0 on success, 1 if seek was out of bounds or f is NULL.
 */
char shseek(struct shared_resource* f, long offset, unsigned char whence)
{
    if (!f) {
        return 1;
    }

    switch (whence) {
    case SEEK_CUR:
        f->position += offset;
        break;
    case SEEK_END:
        f->position = f->length + offset;
        break;
    case SEEK_SET:
    default:
        f->position = offset;
        break;
    }

    if ((long)f->position < 0) {
        f->position = 0;
        return 1;
    }
    if (f->position > f->length) {
        f->position = f->length;
        return 1;
    }

    return 0;
}

/**
 * @brief Opens the shared resource directory for reading entries.
 *
 * @return A directory handle (starting sector index) on success, or 0xffff on
 * failure.
 */
shared_resource_dir shdopen(void)
{
    char i;

    if (do_shres_trap(0)) {
        return 0xffff;
    }

    sdcard_busy_wait();

    // Verify magic string in sector 0
    for (i = 0; magic_string[i]; i++) {
        if (lpeek(0xffd6e00L + i) != magic_string[i]) {
            return 0xffff;
        }
    }

    // Directory starts at sector 1
    return 1;
}

/**
 * @brief Reads the next resource entry from the shared resource directory
 *        that matches the required flags.
 *
 * @param required_flags Bitmask of required flags the resource must have.
 * @param directory_handle Pointer to the current directory sector
 * (auto-increments).
 * @param dirent Pointer to a shared_resource struct to populate.
 * @return
 *   - 0 on success (matching entry read),
 *   - 1 if trap failed,
 *   - 2 if end of directory reached (no more entries).
 */
char shdread(unsigned long required_flags,
    shared_resource_dir* directory_handle, struct shared_resource* dirent)
{
    do {
        if (do_shres_trap(*directory_handle)) {
            return 1;
        }

        sdcard_busy_wait();

        // Check for end of directory (name_len == 0)
        if (!lpeek(0xffd6e10L)) {
            return 2;
        }

        // Copy entry from buffer into dirent
        lcopy(0xffd6e00L, (unsigned long)dirent, 256);
        dirent->position = 0L;

        (*directory_handle)++;
    } while ((dirent->flags & required_flags) != required_flags);

    return 0;
}
