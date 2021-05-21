
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
    __tests_out = __tests_out;
    __asm__("LDA %v", __tests_out);
    __asm__("STA $D643");
    __asm__("NOP");
}
