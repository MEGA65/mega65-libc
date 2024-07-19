#include <mega65/memory.h>
#include <mega65/tests.h>
#include <mega65/debug.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __CC65__
unsigned char __tests_out;
#endif
unsigned short __ut_issueNum;
unsigned char __ut_subissue;

void xemu_exit(int exit_code)
{
    POKE(XEMU_CONTROL, (uint8_t)exit_code);
    POKE(XEMU_CONTROL, XEMU_QUIT);
    exit(exit_code);
}

#ifdef __CC65__
char __debug_text[40 + 1];
void assert_eq(int32_t a, int32_t b)
{
    sprintf(__debug_text, "ASSERT-EQ %ld == %ld", a, b);
    debug_msg(__debug_text);
    if (a != b) {
        xemu_exit(EXIT_FAILURE);
    }
}
#else
void assert_eq(int64_t a, int64_t b)
{
    char msg[40 + 1];
    sprintf(msg, "ASSERT-EQ %lld == %lld", a, b);
    debug_msg(msg);
    if (a != b) {
        xemu_exit(EXIT_FAILURE);
    }
}
#endif

void unit_test_report(
    unsigned short issue, unsigned char sub, unsigned char status)
{
#ifdef __CC65__
    __tests_out = issue & 0xff;
    __asm__("LDA %v", __tests_out);
    __asm__("STA $D643");
    __asm__("CLV");
    __tests_out = issue >> 8;
    __asm__("LDA %v", __tests_out);
    __asm__("STA $D643");
    __asm__("CLV");
    __tests_out = sub;
    __asm__("LDA %v", __tests_out);
    __asm__("STA $D643");
    __asm__("CLV");
    __tests_out = status;
    __asm__("LDA %v", __tests_out);
    __asm__("STA $D643");
    __asm__("CLV");
#else
    asm volatile("st%0 $D643\n"
                 "clv" ::"a"((uint8_t)(issue & 0xff)));
    asm volatile("st%0 $D643\n"
                 "clv" ::"a"((uint8_t)(issue >> 8)));
    asm volatile("st%0 $D643\n"
                 "clv" ::"a"(sub));
    asm volatile("st%0 $D643\n"
                 "clv" ::"a"(status));
#endif
}

void _unit_test_msg(char* msg, char cmd)
{
    unsigned char* current;

    unit_test_report(0, 0, cmd);
    current = msg;

    while (*current) {
#ifdef __CC65__
        __tests_out = *current;
        __asm__("LDA %v", __tests_out);
        __asm__("STA $D643");
        __asm__("CLV");
#else
        asm volatile("st%0 $D643\n"
                     "clv" ::"a"(*current));
#endif
        current++;
    }

#ifdef __CC65__
    __asm__("LDA #92");
    __asm__("STA $D643");
    __asm__("CLV");
#else
    asm volatile("lda #92\n"
                 "sta $D643\n"
                 "clv" ::
                     : "a");
#endif
}

void unit_test_set_current_name(char* name)
{
    _unit_test_msg(name, 0xfe);
}

void unit_test_log(char* msg)
{
    _unit_test_msg(msg, 0xfd);
}

void unit_test_setup(char* testName, unsigned short issueNum)
{
    POKE(0xD02F, 0x47);
    POKE(0xD02F, 0x53);
    __ut_issueNum = issueNum;
    __ut_subissue = 0;
    unit_test_set_current_name(testName);
    unit_test_report(__ut_issueNum, __ut_subissue, 0xf0);
}

void unit_test_ok(char* msg)
{
    if (msg) {
        unit_test_log(msg);
    }
    unit_test_report(__ut_issueNum, __ut_subissue, 0xf2);
    ++__ut_subissue;
}

void unit_test_fail(char* msg)
{
    if (msg) {
        unit_test_log(msg);
    }
    unit_test_report(__ut_issueNum, __ut_subissue, 0xf3);
    ++__ut_subissue;
}

void unit_test_done(void)
{
    unit_test_report(__ut_issueNum, __ut_subissue, 0xff);
}

static void _unit_test_vic_get_default(vic_config* conf)
{
    conf->crt_emu = 0;
    conf->h640 = 0;
    conf->v400 = 0;
    conf->side_border_width = 0x50;
    conf->border_col = 14;
    conf->background_col = 6;
    conf->cols = 40;
    conf->rows = 25;
    conf->lower_case_charset = 1;
    conf->disable_viciii_bug_compatibility = 1;
    conf->xscl = 0;
}

void unit_test_vic_get_default_pal(vic_config* conf)
{
    _unit_test_vic_get_default(conf);
    conf->pal = 1;
}

void unit_test_vic_get_default_ntsc(vic_config* conf)
{
    _unit_test_vic_get_default(conf);
    conf->pal = 0;
}

static uint32_t screen_ram_addr = 0xc000;

void unit_test_init_vic(vic_config* conf)
{
    mega65_io_enable();

    // set vic-iii bug compatibility
    POKE(0xD07A, (PEEK(0xD07A) & 0xDF)
                     | (conf->disable_viciii_bug_compatibility ? 1 << 5 : 0));
    // enable hot registers and set side border width
    POKE(0xD05C, conf->side_border_width & 0xFF);
    POKE(0xD05D, 0xC0 | conf->side_border_width >> 8);
    // configure pal/ntsc
    POKE(0xD06F, (conf->pal ? 0 : 1 << 7) | (PEEK(0xD06F) & 0x7F));
    // normal chars, configure PALEMU
    POKE(0xD054, (conf->crt_emu ? 1 << 5 : 0) | (0x40));
    // H640 and V400
    POKE(0xD031, (conf->h640 ? 1 << 7 : 0) | (conf->v400 ? 1 << 3 : 0) | 0x60);
    // select charset
    POKE(0xD018, conf->lower_case_charset ? 0x16 : 0x14);
    // set xscl (will trigger a final hot register update)
    POKE(0xD016, 8 | (conf->xscl & 0x07));
    // border and background
    POKE(0xD020, conf->border_col);
    POKE(0xD021, conf->background_col);

    // keep screen ram at 0x400 for 40x25 screen (0xc000 otherwise)
    if (conf->cols == 40 && conf->rows == 25) {
        screen_ram_addr = 0x0400;
    }
    else {
        screen_ram_addr = 0xc000;
    }
    POKE(0xD060, (screen_ram_addr >> 0) & 0xff);
    POKE(0xD061, (screen_ram_addr >> 8) & 0xff);
    POKE(0xD062, (screen_ram_addr >> 16) & 0xff);

    lfill(screen_ram_addr, 0x20, (uint16_t)conf->rows * conf->cols);
    lfill(0xff80000L, 0x01, (uint16_t)conf->rows * conf->cols);
}

static uint16_t char_addr;
static uint32_t col_addr;
static uint8_t char_code;

void unit_test_print(uint8_t x, uint8_t y, uint8_t colour, char* msg)
{
    uint16_t cnt = 0;

    char_addr = x + y * (uint16_t)PEEK(0xD05E);
    col_addr = 0xff80000L + char_addr;
    char_addr += (uint16_t)screen_ram_addr;
    while (*msg) {
        char_code = *msg;
        if (*msg >= 0xc0 && *msg <= 0xe0) {
            char_code = *msg - 0x80;
        }
        else if (*msg >= 0x40 && *msg <= 0x60) {
            char_code = *msg - 0x40;
        }
        else if (*msg >= 0x60 && *msg <= 0x7A) {
            char_code = *msg - 0x20;
        }
        POKE(char_addr++, char_code);
        lpoke(col_addr++, colour);
        ++msg;
        ++cnt;
    }
    lfill(col_addr, colour, cnt);
}

static uint8_t frame_num;

void unit_test_read_pixel(
    short x, short y, uint8_t* red, uint8_t* green, uint8_t* blue)
{
    POKE(0xD07D, (uint8_t)x);
    POKE(0xD07E, (uint8_t)y);
    POKE(0xD07F, (uint8_t)((x >> 8) + ((y >> 8) << 4)));

    // Wait at least two whole frames
    frame_num = PEEK(0xD7FA);
    while (PEEK(0xD7FA) == frame_num) {
        continue;
    }
    frame_num = PEEK(0xD7FA);
    while (PEEK(0xD7FA) == frame_num) {
        continue;
    }
    frame_num = PEEK(0xD7FA);
    while (PEEK(0xD7FA) == frame_num) {
        continue;
    }

    POKE(0xD07C, 0x52);
    *red = PEEK(0xD07D);
    POKE(0xD07C, 0x92);
    *green = PEEK(0xD07D);
    POKE(0xD07C, 0xD2);
    *blue = PEEK(0xD07D);
}
