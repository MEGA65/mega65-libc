#include <mega65/memory.h>
#include <mega65/random.h>
#include <stdint.h>

uint32_t random_temp;
uint8_t random_step;
uint8_t random_byte, raster_temp;

void generate_random_byte(void)
{
    random_byte = 0;
    random_step = 32;

    while (random_step--) {
        random_byte
            = (unsigned char)(random_byte << 1) | ((random_byte >> 7) ^ (PEEK(0xD6DE) & 0x01));
        // We then have to wait 10usec before the next value is ready.
        // 1 raster line is more than that, so just wait one raster line
        raster_temp = PEEK(0xD052);
        while (PEEK(0xD052) == raster_temp) {
            continue;
        }
    }
}

uint32_t random32(uint32_t range)
{
    generate_random_byte();
    POKE(0xD770, random_byte);
    generate_random_byte();
    POKE(0xD771, random_byte);
    generate_random_byte();
    POKE(0xD772, random_byte);
    generate_random_byte();
    POKE(0xD773, random_byte);

    if (!range) {
        return PEEK32(0xD770);
    }
    POKE32(0xD774, range);
    return PEEK32(0xD77C);
}

uint16_t random16(uint16_t range)
{
    generate_random_byte();
    POKE(0xD770, random_byte);
    generate_random_byte();
    POKE(0xD771, random_byte);

    POKE(0xD772, 0);
    POKE(0xD773, 0);
    POKE(0xD776, 0);
    POKE(0xD777, 0);

    if (!range) {
        return PEEK16(0xD770);
    }
    POKE16(0xD774, range);
    return PEEK16(0xD77A);
}

uint8_t random8(uint8_t range)
{
    // We don't really trust that the low enough bit has enough entropy
    // on a single reading, so we calculate the XOR of several samples
    generate_random_byte();

    if (range) {
        POKE(0xD770, random_byte);
        POKE(0xD771, 0);
        POKE(0xD774, range);
        POKE(0xD775, 0);
        return PEEK(0xD779);
    }
    else {
        return random_byte;
    }
}

uint32_t xorshift32_state = 1;

/* The state word must be initialized to non-zero */
uint32_t xorshift32(void)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = xorshift32_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    xorshift32_state = x;
    return xorshift32_state;
}

void xorshift32_seed(uint32_t seed)
{
    // If seed is zero, then pick a random one
    xorshift32_state = seed;
    while (!xorshift32_state) {
        xorshift32_state = random32(0);
    }
}

void srand(uint32_t seed)
{
    xorshift32_seed(seed);
}

uint32_t rand32(uint32_t range)
{
    xorshift32();
    if (!range) {
        return xorshift32_state;
    }
    POKE32(0xD770, xorshift32_state);
    POKE32(0xD774, range);
    return PEEK32(0xD77C);
}

uint16_t rand16(uint16_t range)
{
    xorshift32();
    if (!range) {
        return (uint16_t)xorshift32_state;
    }
    POKE32(0xD770, xorshift32_state);
    POKE32(0xD774, range); // @todo Is this correct? range is 16-bit, but POKE32
                           // writes 32-bit
    return PEEK16(0xD77C);
}

#ifdef DEBUG
#include <stdio.h>
#include <debug.h>
char dmsg[80];
#endif

uint8_t rand8(uint8_t range)
{
    xorshift32();
    if (!range) {
        return (uint8_t)xorshift32_state;
    }

    // We do it this way to be compatible with older bitstreams that
    // have the smaller multiplier.  Not relevant once 138-hdmi-audio-27mhz
    // branch has been merged in.
    POKE(0xD770, xorshift32_state & 0xff);
    POKE(0xD771, 0);
    POKE(0xD774, range);
    POKE(0xD775, 0);

#ifdef DEBUG
    snprintf(dmsg, 80,
        "range=%d, rand=$%02x, regs=%02x %02x, %02x %02x, %02x, %02x\n", range,
        PEEK(0xD779), PEEK(0xD770), PEEK(0xD771), PEEK(0xD774), PEEK(0xD775),
        PEEK(0xD778), PEEK(0xD779));
    debug_msg(dmsg);
#endif

    return PEEK(0xD779);
}
