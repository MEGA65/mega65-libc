#include "memory.h"

void debug_msg(char *m)
{
  // Write debug message to serial monitor
  while(*m) {    
    POKE(0xD643,*m);
    m++;
  }
  POKE(0xD643,0x0D);
  POKE(0xD643,0x0A);
}
