
#include <string.h>
#include <stdio.h>

unsigned char __tests_out;

void unit_test_report(unsigned short issue, unsigned char sub, unsigned char status)
{
  __tests_out = issue & 0xff;
  __asm__("LDA %v", __tests_out);
  __asm__("STA $D643");
  __asm__("NOP");
  __tests_out = issue >> 8;
  __asm__("LDA %v", __tests_out);
  __asm__("STA $D643");
  __asm__("NOP");
  __tests_out = sub;
  __asm__("LDA %v", __tests_out);
  __asm__("STA $D643");
  __asm__("NOP");
  __tests_out = status;
  __asm__("LDA %v", __tests_out);
  __asm__("STA $D643");
  __asm__("NOP");
}

void _unit_test_msg(char *msg, char cmd)
{
  unsigned char *current;

  unit_test_report(0, 0, cmd);
  current = msg;

  while (*current)
  {
    __tests_out = *current++;
    __asm__("LDA %v", __tests_out);
    __asm__("STA $D643");
    __asm__("NOP");
  }

  __asm__("LDA #92");
  __asm__("STA $D643");
  __asm__("NOP");
}

void unit_test_set_current_name(char *name)
{
  _unit_test_msg(name, 0xfe);
}

void unit_test_log(char *msg)
{
  _unit_test_msg(msg, 0xfd);
}