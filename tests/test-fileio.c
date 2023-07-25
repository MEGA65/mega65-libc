/**
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
#include <stdlib.h>

#define assert_eq(A, B)                                                        \
    if (A != B)                                                                \
    xemu_exit(EXIT_FAILURE)

// Input file on SD card: CHARROM.M65
char filename[11 + 1] = { 0x63, 0x68, 0x61, 0x72, 0x72, 0x6f, 0x6d, 0x2e, 0x6d,
    0x36, 0x35, 0x00 };
uint8_t file;
uint8_t buffer[512];
size_t num_bytes_read;

int main(void)
{
    // Tests run in C64 mode so we need to enable mega65
    mega65_io_enable();

    // Good practice
    closeall();
    chdirroot();

    // Check open status
    file = open(filename);
    if (file == 0xff) {
        xemu_exit(EXIT_FAILURE);
    }

    // Read single 512 byte chunk
    num_bytes_read = read512(buffer);
    assert_eq(num_bytes_read, 512);

    // Check first two bytes of chunk
    assert_eq(buffer[0], 0x3c);
    assert_eq(buffer[1], 0x66);

    // Check last two bytes of chunk
    assert_eq(buffer[510], 0x18);
    assert_eq(buffer[511], 0x00);

    // The size of CHARROM is 8 x 512 = 4096 bytes; let's read until EOF
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 512);
    assert_eq(read512(buffer), 0);

    // This has no effect on the test, but let's call anyway
    close(file);
    closeall();

    xemu_exit(EXIT_SUCCESS);
}
