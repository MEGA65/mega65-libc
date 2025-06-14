/**
 * @file shres.h
 * @brief MEGA65 SYSPART Shared Resources Access API
 *
 * This API provides access to shared resources stored in the MEGA65 system partition (SYSPART),
 * including files such as fonts, icons, or other binary assets.
 * 
 * Resources are accessed via a special trap instruction and a sector-based directory format.
 * This API allows enumeration, opening, reading, and seeking within those shared resources.
 */

#ifndef SHRES_H
#define SHRES_H

/// Triggers the low-level SYSPART shared resource trap.
extern void shres_trap(void);

/// Registers used for communicating with the shared resource trap.
extern unsigned char shres_regs[5];

/// Flag indicating that the resource is a font.
#define SHRES_FLAG_FONT 1

/// Indicates the font is 16x16 pixels (requires SHRES_FLAG_FONT).
#define SHRES_FLAG_16x16 2

/// Indicates the font supports Unicode (requires SHRES_FLAG_FONT).
#define SHRES_FLAG_UNICODE 4

/// Maximum length of a resource name (not including null terminator).
#define MAX_RES_NAME_LEN 240

/**
 * @struct shared_resource
 * @brief Represents a file or other data resource in the SYSPART directory.
 *
 * This structure includes metadata stored on disk (such as name, flags, and sector info),
 * as well as an in-memory field to track read position for streaming access.
 */
struct shared_resource {
    /// First sector of the resource.
    unsigned long first_sector;

    /// Length of the resource in sectors.
    unsigned long length_in_sectors;

    /// Length of the resource in bytes.
    unsigned long length;

    /// Bitmask of resource flags (e.g., SHRES_FLAG_FONT, SHRES_FLAG_16x16).
    unsigned long flags;

    /// Length of the resource name (as stored in the directory).
    unsigned char name_len;

    /// Null-terminated name of the resource (up to MAX_RES_NAME_LEN).
    char name[MAX_RES_NAME_LEN + 1];

    /// Current read position in bytes (used internally; not stored on disk).
    unsigned long position;
};

/// Type alias for a directory handle (really a sector index).
#define shared_resource_dir unsigned int

/**
 * @brief Opens a named shared resource and verifies required flags.
 *
 * @param resource_name The null-terminated name of the resource to open.
 * @param required_flags A bitmask of flags that must be set on the resource.
 * @param file_handle Pointer to a shared_resource structure to populate.
 * @return 0 on success, 1 if not found or error.
 */
char shopen(char *resource_name, unsigned long required_flags,
            struct shared_resource *file_handle);

/**
 * @brief Reads data from an open shared resource file.
 *
 * @param ptr Pointer to the output buffer.
 * @param count Number of bytes to read.
 * @param f Pointer to the shared_resource handle.
 * @return Number of bytes actually read.
 */
unsigned int shread(unsigned char *ptr, unsigned int count,
                    struct shared_resource *f);

/**
 * @brief Seeks to a new position in an open shared resource.
 *
 * @param f Pointer to the shared_resource handle.
 * @param offset Byte offset from the origin.
 * @param whence One of SEEK_SET, SEEK_CUR, or SEEK_END.
 * @return 0 on success, 1 if the seek was out of bounds or invalid.
 */
char shseek(struct shared_resource *f, long offset, unsigned char whence);

/**
 * @brief Opens the shared resource directory for iteration.
 *
 * @return A directory handle (starting sector index), or 0xFFFF on failure.
 */
shared_resource_dir shdopen(void);

/**
 * @brief Reads the next matching directory entry.
 *
 * @param required_flags A bitmask of required flags the resource must match.
 * @param directory_handle Pointer to the directory handle returned by shdopen().
 * @param dirent Pointer to a shared_resource structure to populate.
 * @return
 *   - 0 on success (entry read and matches),
 *   - 1 if the trap failed,
 *   - 2 if end of directory reached (no more entries match).
 */
char shdread(unsigned long required_flags,
             shared_resource_dir *directory_handle,
             struct shared_resource *dirent);

#endif // SHRES_H
