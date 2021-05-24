
#include <string.h>
#include <stdio.h>
#include <stdint.h>


unsigned char __tests_out;
unsigned short __ut_issueNum;
unsigned char __ut_subissue;

#define POKE(a, v) *((uint8_t*)a) = (uint8_t)v
#define PEEK(a) ((uint8_t)(*((uint8_t*)a)))

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

void unit_test_setup(char *testName, unsigned short issueNum)
{
  POKE(0xD02F, 0x47);
  POKE(0xD02F, 0x53);
  __ut_issueNum = issueNum;
  __ut_subissue = 0;
  unit_test_set_current_name(testName);
  unit_test_report(__ut_issueNum, __ut_subissue, 0xf0);
}

void unit_test_ok(void)
{
  unit_test_report(__ut_issueNum, __ut_subissue, 0xf2);
  ++__ut_subissue;
}

void unit_test_fail(char *msg)
{
  unit_test_report(__ut_issueNum, __ut_subissue, 0xf3);
  if (msg)
  {
    unit_test_log(msg);
  }
  ++__ut_subissue;
}

void unit_test_done(void) {
    unit_test_report(__ut_issueNum, __ut_subissue, 0xff);
}