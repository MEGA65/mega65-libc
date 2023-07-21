/**
 * Tests for memory.h
 *
 * This can be run in Xemu in testing mode with e.g.
 *
 *     xmega65 -testing -headless -sleepless -prg test-memory.prg
 *
 * If a test fails, xemu will exit with a non-zero return code.
 */
#include <stdlib.h>
#include <stdio.h>
#include <mega65/memory.h>

#define XEMU_CONTROL 0xD6CF
#define XEMU_QUIT 0x42

void xemu_exit(int exit_code)
{
    POKE(XEMU_CONTROL, (uint8_t)exit_code);
    POKE(XEMU_CONTROL, XEMU_QUIT);
    exit(exit_code);
}

#define assert_eq(A, B)                                                        \
    if (A != B)                                                                \
    xemu_exit(EXIT_FAILURE)

int main(void)
{
    // Integer sizes
    assert_eq(sizeof(uint8_t), 1);
    assert_eq(sizeof(unsigned char), 1);
    assert_eq(UINT8_MAX, 0xFF);

    assert_eq(sizeof(uint16_t), 2);
    assert_eq(sizeof(unsigned int), 2);
    assert_eq(UINT16_MAX, 0xFFFF);

    assert_eq(sizeof(uint32_t), 4);
    assert_eq(sizeof(unsigned long), 4);
    assert_eq(UINT32_MAX, 0xFFFFFFFF);

    assert_eq(INT16_MAX, 0x7FFF);
    assert_eq(INT32_MAX, 0x7FFFFFFF);

    // PEEK and POKE macros
    POKE(0x3000, 7);
    POKE(0x3001, 9);
    POKE(0x3002, 13);
    assert_eq(PEEK(0x3000), 7);
    assert_eq(PEEK(0x3001), 9);
    assert_eq(PEEK(0x3002), 13);

    // lcopy
    lcopy(0x3000, 0x4000, 3);
    assert_eq(PEEK(0x4000), 7);
    assert_eq(PEEK(0x4001), 9);
    assert_eq(PEEK(0x4002), 13);

    // lpoke and lpeek
    lpoke(0x4000, 13);
    lpoke(0x4001, 9);
    lpoke(0x4002, 7);
    assert_eq(lpeek(0x4000), 13);
    assert_eq(lpeek(0x4001), 9);
    assert_eq(lpeek(0x4002), 7);

    // dma_poke and dma_peek
    dma_poke(0x4000, 13);
    dma_poke(0x4001, 9);
    dma_poke(0x4002, 7);
    assert_eq(dma_peek(0x4000), 13);
    assert_eq(dma_peek(0x4001), 9);
    assert_eq(dma_peek(0x4002), 7);

    // lfill
    lfill(0x3000, 1, 3);
    assert_eq(PEEK(0x3000), 1);
    assert_eq(PEEK(0x3001), 1);
    assert_eq(PEEK(0x3002), 1);

    // lfill_skip
    lfill_skip(0x3000, 0, 3, 2);
    assert_eq(PEEK(0x3000), 0);
    assert_eq(PEEK(0x3001), 1);
    assert_eq(PEEK(0x3002), 0);

    // POKE16 and PEEK16 macros
    POKE16(0x3000, 0xAABB);
    assert_eq(PEEK16(0x3000), 0xAABB);

    // POKE32 and PEEK32 macros
    POKE32(0x3000, 0xAABBCCDD);
    assert_eq(PEEK32(0x3000), 0xAABBCCDD);

    xemu_exit(EXIT_SUCCESS);
}
