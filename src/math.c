#include <mega65/math.h>

uint32_t umul32(const uint32_t a, const uint32_t b)
{
    POKE32(MULTINA, a);
    POKE32(MULTINB, b);
    return PEEK32(MULTOUT);
}

uint32_t udiv32(const uint32_t a, const uint32_t b)
{
    POKE32(MULTINA, a);
    POKE32(MULTINB, b);
    while (PEEK(DIVBUSY) != 0) { };
    return PEEK32(DIVOUT_WHOLE);
}

uint16_t udiv16(const uint16_t a, const uint16_t b)
{
    POKE16(MULTINA, a);
    POKE16(MULTINB, b);
    POKE16(MULTINA + 2, 0);
    POKE16(MULTINB + 2, 0);
    while (PEEK(DIVBUSY) != 0) { };
    return PEEK16(DIVOUT_WHOLE);
}

uint16_t umul16(const uint16_t a, const uint16_t b)
{
    POKE16(MULTINA, a);
    POKE16(MULTINB, b);
    return PEEK16(MULTOUT);
}

#ifndef __clang__
uint8_t umul8(const uint8_t a, const uint8_t b)
{
    POKE(MULTINA, a);
    POKE(MULTINB, b);
    return PEEK(MULTOUT);
}
#endif
