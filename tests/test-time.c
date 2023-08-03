/**
 * @example test-time.c
 *
 * Tests for time.h
 *
 * This can be run in Xemu in testing mode with e.g.
 *
 *     xmega65 -testing -headless -sleepless -prg test-time.prg
 *
 * If a test fails, Xemu exits with a non-zero return code.
 */
#include <mega65/memory.h>
#include <mega65/time.h>
#include <mega65/tests.h>
#include <mega65/targets.h>
#include <stdlib.h>

struct m65_tm tm;

int main(void)
{
    // Tests run in C64 mode so we need to enable mega65
    mega65_io_enable();

    getrtc(&tm);

    assert_eq(tm.tm_year + 1900 >= 2000, 1);
    assert_eq(tm.tm_mon < 12, 1);
    assert_eq(tm.tm_mday > 0, 1);
    assert_eq(tm.tm_mday < 32, 1);
    assert_eq(tm.tm_hour < 24, 1);
    assert_eq(tm.tm_min < 60, 1);
    assert_eq(tm.tm_sec < 61, 1);
    assert_eq(tm.tm_wday < 7, 1);
    assert_eq(tm.tm_yday < 366, 1);

    xemu_exit(EXIT_SUCCESS);
}
