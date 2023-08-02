#include <mega65/debug.h>

#ifdef __CC65__
unsigned char the_char;
#endif

void debug_msg(char* msg)
{
    while (*msg) {
#ifdef __CC65__
        the_char = *msg;
        __asm__("LDA %v", the_char);
        __asm__("STA $D643");
        __asm__("NOP");
#else
        asm volatile("st%0 $d643\n"
                     "nop" ::"a"(*msg)
                     : "a");
#endif
        msg++;
    }
#ifdef __CC65__
    __asm__("LDA #$0d");
    __asm__("STA $D643");
    __asm__("NOP");
    __asm__("LDA #$0a");
    __asm__("STA $D643");
    __asm__("NOP");
#else
    asm volatile("lda #$0d\n"
                 "sta $d643\n"
                 "nop\n"
                 "lda #$0a\n"
                 "sta $d643\n"
                 "nop" ::
                     : "a");
#endif
}
