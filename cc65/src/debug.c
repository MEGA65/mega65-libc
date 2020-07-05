#include "memory.h"

unsigned char the_char;
void debug_msg(char *m)
{
  // Write debug message to serial monitor
  while(*m) {
    the_char=*m;
    __asm__ ("LDA %v",the_char);
    __asm__ ("STA $D643");
    __asm__ ("NOP");
    m++;
  }
  __asm__ ("LDA #$0d");
    __asm__ ("STA $D643");
    __asm__ ("NOP");
  __asm__ ("LDA #$0a");
    __asm__ ("STA $D643");
    __asm__ ("NOP");
}
