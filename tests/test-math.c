/**
 * @example test-math.c
 *
 * Tests for math functions
 *
 * This can be run in Xemu in testing mode with e.g.
 *
 *     xmega65 -testing -headless -sleepless -prg test-math.prg
 *
 * If a test fails, xemu will exit with a non-zero return code.
 */

#include <mega65/math.h>
#include <mega65/tests.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
    uint32_t a32 = 0xffffeeee;
    uint32_t b32 = 0xffffffff;
    uint16_t a16 = 0xeeee;
    uint16_t b16 = 0xffff;
    uint8_t a8 = 0xee;
    uint8_t b8 = 0xff;

    volatile uint16_t i = 2;
    volatile uint16_t j = 2;

    mega65_io_enable();

    assert_eq(i * j, 4);

    // 32-bit
    assert_eq(umul32(1, 0), 0);
    assert_eq(umul32(2, 8), 16);
    assert_eq(umul32(255, 255), 65025);
    assert_eq(umul32(a32, b32), 4370);
    assert_eq(udiv32(0, 1), 0);
    assert_eq(udiv32(1, 2), 0);
    assert_eq(udiv32(100, 2), 50);
    assert_eq(udiv32(UINT32_MAX, UINT32_MAX), 1);

    // 16-bit
    assert_eq(umul16(1, 0), 0);
    assert_eq(umul16(2, 8), 16);
    assert_eq(umul16(a16, b16), 4370);
    assert_eq(udiv16(125, 25), 5);
    assert_eq(udiv16(UINT16_MAX, UINT16_MAX), 1);

    // 8-bit
    assert_eq(umul8(a8, b8), 18);
    assert_eq(umul8(1, 0), 0);
    assert_eq(umul8(2, 8), 16);

    xemu_exit(EXIT_SUCCESS);
    return 0;
}
