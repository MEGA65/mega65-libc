/**
 * @example test-fileio.c
 *
 * Tests for fileio.h for loading CHARROM.M65 from the SD card
 *
 * This can be run in Xemu in testing mode with e.g.
 *
 *     xmega65 -testing -headless -sleepless -prg test-fileio.prg
 *
 * If a test fails, Xemu exits with a non-zero return code.
 */
#include <mega65/memory.h>
#include <mega65/fileio.h>
#include <mega65/tests.h>
#include <mega65/debug.h>
#include <stdlib.h>

#define FILE_ERROR 0xff

// Input file on SD card: CHARROM.M65
char filename[11 + 1] = { 0x63, 0x68, 0x61, 0x72, 0x72, 0x6f, 0x6d, 0x2e, 0x6d,
    0x36, 0x35, 0x00 };
char* unknown_filename = "PHANTOM_FILE";
uint8_t file;
uint8_t error;
uint8_t buffer[512];
size_t num_bytes_read;

struct hyppo_version version;

int main(void)
{
    // No effect on MEGA65 target
    mega65_io_enable();

    // Get hypervisor version
    debug_msg("TEST: hyppoversion()");
    gethyppoversion(&version);
    assert_eq(version.hyppo_major >= 1, 1);
    assert_eq(version.hyppo_minor >= 2, 1);
    assert_eq(version.hdos_major >= 1, 1);
    assert_eq(version.hdos_minor >= 2, 1);

    // Good practice
    closeall();
    error = chdirroot();

    // Try to open non-existent file
    file = open(unknown_filename);
    if (file != FILE_ERROR) {
        xemu_exit(EXIT_FAILURE);
    }

    // Try to open existing file
    file = open(filename);
    if (file == FILE_ERROR) {
        xemu_exit(EXIT_FAILURE);
    }

    // Read single 512 byte chunk
    debug_msg("TEST: read512()");
    num_bytes_read = read512(buffer);
    assert_eq(num_bytes_read, 512);

    // Check first two bytes of chunk
    debug_msg("TEST: read512() - first two bytes");
    assert_eq(buffer[0], 0x3c);
    assert_eq(buffer[1], 0x66);

    // Check last two bytes of chunk
    debug_msg("TEST: read512() - last two bytes");
    assert_eq(buffer[510], 0x18);
    assert_eq(buffer[511], 0x00);

    // The size of CHARROM is 8 x 512 = 4096 bytes; let's read until EOF
    debug_msg("TEST: read512() until EOF");
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    // This should return 0, indicating EOF
    debug_msg("TEST: read512() EOF reached");
    assert_eq(read512(buffer), 0);

    // The very last byte of the file
    assert_eq(buffer[511], 0xf0);

    // This has no effect on the test, but let's call anyway
    close(file);
    closeall();

    xemu_exit(EXIT_SUCCESS);
    return 0;
}
