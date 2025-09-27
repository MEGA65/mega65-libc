#include <mega65/hal.h>
#include <mega65/memory.h>

void usleep(uint32_t micros)
{
    // Sleep for desired number of micro-seconds.
    // Each VIC-II raster line is ~64 microseconds
    // this is not totally accurate, but is a reasonable approach
    while (micros > 64) {
#ifdef LLVM_503_WORKAROUND
      asm volatile(
        "ldx $D012\n"
        "1:\n"
        "cpx $D012\n"
        "beq 1b\n"
        :
        :
        : "x"   // X is clobbered
    );
#else
      uint8_t b = PEEK(0xD012);
        while (PEEK(0xD012) == b) {
	  continue;
        }
#endif
        micros -= 64; 
    }
    return;
}
