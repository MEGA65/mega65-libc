/*
 * A minimalistic CONIO.H style header  for the Mega65 libC
 * 
 * Author: Hernán Di Pietro <hernan.di.pietro@gmail.com>
 * Date  : 2020-06-28
 */

#ifndef M65LIBC_CONIO_H
#define M65LIBC_CONIO_H

#define VIC_BASE            0xD000UL
#define SCREEN_ADDRESS      0x8000
#define REG_H640            (PEEK(VIC_BASE+0x31) & 128)
#define REG_V400            (PEEK(VIC_BASE+0x31) & 8)
#define REG_16BITCHARSET    (PEEK(VIC_BASE + 0x54) & 1)
#define SCREEN_RAM_BASE_B0  (PEEK(VIC_BASE + 0x60)) // LSB
#define SCREEN_RAM_BASE_B1  (PEEK(VIC_BASE + 0x61))
#define SCREEN_RAM_BASE_B2  (PEEK(VIC_BASE + 0x62))
#define SCREEN_RAM_BASE_B3  (PEEK(VIC_BASE + 0x63) & 7) // upper nybble
#define SCREEN_RAM_BASE     ( ((long)SCREEN_RAM_BASE_B3 << 24) | ((long)SCREEN_RAM_BASE_B2 << 16) | ((long)SCREEN_RAM_BASE_B1 << 8) | (SCREEN_RAM_BASE_B0) )  
#define COLOR_RAM_BASE      0x1F800UL
#define ATTRIB_REVERSE 0x20
#define ATTRIB_BLINK 0x10
#define ATTRIB_UNDERLINE 0x80
#define ATTRIB_HIGHLIGHT 0x40
#define COLOUR_BLACK 0
#define COLOUR_WHITE 1
#define COLOUR_RED 2
#define COLOUR_CYAN 3
#define COLOUR_PURPLE 4
#define COLOUR_GREEN 5
#define COLOUR_BLUE 6
#define COLOUR_YELLOW 7
#define COLOUR_ORANGE 8
#define COLOUR_BROWN 9
#define COLOUR_PINK 10
#define COLOUR_GREY1 11
#define COLOUR_DARKGREY 11
#define COLOUR_GREY2 12
#define COLOUR_GREY 12
#define COLOUR_MEDIUMGREY 12
#define COLOUR_LIGHTGREEN 13
#define COLOUR_LIGHTBLUE 14
#define COLOUR_GREY3 15
#define COLOUR_LIGHTGREY 15

 /* Clear the text screen. Color RAM will be cleared with current text color */
void clrscr(); 

 /* Returns the dimensions of the text mode screen.  
    Ignores any virtual chargen dimensions */
void fastcall screensize(unsigned char* width, unsigned char* height);

/* Set the current border color */
void fastcall bordercolor(unsigned char c);

/* Set the current screen color */
void fastcall bgcolor(unsigned char c);

/* Set the current text color*/
void fastcall textcolor(unsigned char c);

/* Enable the reverse attribute */
void fastcall revers(unsigned char enable);

/* Enable the highlight attribute */
void fastcall highlight(unsigned char enable);

/* Enable the highlight attribute */
void fastcall blink(unsigned char enable);

/* Enable the highlight attribute */
void fastcall underline(unsigned char enable);

/* Put cursor at X,Y. 
   The function does not check for screen bounds! */
void fastcall gotoxy(unsigned char x, unsigned char y);

/* Put cursor at column X. The function does not check for screen bounds */
void fastcall gotox(unsigned char x);

/* Put cursor at row Y. The function does not check for screen bounds */
void fastcall gotoy(unsigned char x);

/* Enable cursor */
void fastcall cursor(unsigned char enable);

/* Output a single character at current position */
void fastcall cputc(unsigned char c);

/* Set color of character cell */
void ccellcolor(unsigned char x, unsigned char y, unsigned char c);

/* Output an hex-formatted number at current position with prec digits */
void cputhex(long n, unsigned char prec);

/* Output a decimal number at current position with padding digits */
void cputdec(long n, unsigned char padding, unsigned char leadingZ);

/* Output a string at current position */
void fastcall cputs(const char* s);

/* Output a string at x,y */
void cputsxy (unsigned char x, unsigned char y, const char* s);

/* Output a character at x,y */
void cputcxy (unsigned char x, unsigned char y, char c);

/* Wait until a character is in the keyboard buffer and return it */
unsigned char fastcall cgetc (void);

/* Return the character in the keyboard buffer, if any */
unsigned char fastcall kbhit (void);

unsigned char cprintf (const char* format, ...);
void cscanf(const char* format, ...);


#endif //M65LIBC_CONIO_H


