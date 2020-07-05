#include "memory.h"

void debug_msg(char *m)
{
  // Write debug message to serial monitor
  while(*m) {    
    asm ("LDA %v",*m);
    asm ("STA $D643");
    asm ("NOP");
    m++;
  }
}
