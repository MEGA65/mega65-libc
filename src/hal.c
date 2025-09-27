#include <mega65/hal.h>
#include <mega65/memory.h>

void usleep(uint32_t micros)
{
    // Sleep for desired number of micro-seconds.
    // Each VIC-II raster line is ~64 microseconds
    // this is not totally accurate, but is a reasonable approach
    while (micros > 64) {
        uint8_t b = PEEK(0xD012);
        while (PEEK(0xD012) == b) {
            continue;
        }
	if (micros>=64) micros -= 64; else break;
    }
    return;
}
