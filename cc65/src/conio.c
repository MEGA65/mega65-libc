/*  CONIO.H style Text mode support  for the Mega65 libC

    Copyright (c) 2020 Hernán Di Pietro

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 
    Version   0.3
    Date      2020-06-28
*/

#include "../include/conio.h"
#include "../include/memory.h"

#define PRINTF_IN_FORMAT_SPEC    0x1
#define PRINTF_FLAGS_LEADINGZERO 0x2

static unsigned char g_curTextColor = COLOUR_WHITE;
static unsigned char g_curAttr = 0;
static unsigned char g_curX = 0;
static unsigned char g_curY = 0;
static const unsigned char hexDigits[] = { '0','1','2','3','4','5','6','7','8','9',0x41,0x42,0x43,0x44,0x45,0x46};

static unsigned char strlen(const char* s)
{
    unsigned char len = 0;
    while(*s++)
        len++;

    return len;
}   

void screensize(unsigned char* width, unsigned char *height)
{
    *width = REG_H640 ? 80 : 40;
    *height = REG_V400 ? 50 : 25;
}

void clrscr()
{
    unsigned int cBytes = 0;
    unsigned char w = 0, h = 0;
    
    screensize(&w, &h);
    cBytes = (unsigned int)w * h * (REG_16BITCHARSET ? 2 : 1);
    lfill(SCREEN_RAM_BASE, ' ', cBytes);
    lfill(COLOR_RAM_BASE,  g_curTextColor, cBytes);
}

void bordercolor(unsigned char c)
{
    POKE(VIC_BASE + 0x20, c);
}

void bgcolor(unsigned char c)
{
    POKE(VIC_BASE + 0x21, c);
}

void textcolor(unsigned char c)
{
    g_curTextColor = (g_curTextColor & 0xF0) | (c & 0xf);
}

void ccellcolor(unsigned char x, unsigned char y, unsigned char c)
{
    unsigned char w = 0, h = 0;
    screensize(&w, &h);
    lpoke(COLOR_RAM_BASE + (y * (unsigned int) w) + x, c);
}

void revers(unsigned char enable)
{
    if (enable)
        g_curTextColor |= ATTRIB_REVERSE;
    else
        g_curTextColor &= ~ATTRIB_REVERSE;
}

void highlight(unsigned char enable)
{
    if (enable)
        g_curTextColor |= ATTRIB_HIGHLIGHT;
    else
        g_curTextColor &= ~ATTRIB_HIGHLIGHT;
}

void blink(unsigned char enable)
{
    if (enable)
        g_curTextColor |= ATTRIB_BLINK;
    else
        g_curTextColor &= ~ATTRIB_BLINK;
}

void underline(unsigned char enable)
{
    if (enable)
        g_curTextColor |= ATTRIB_UNDERLINE;
    else
        g_curTextColor &= ~ATTRIB_UNDERLINE;
}

void gotoxy(unsigned char x, unsigned char y)
{
    g_curX = x;
    g_curY = y;
}

void gotox(unsigned char x)
{
    g_curX = x;
}

void gotoy(unsigned char y)
{
    g_curY = y;
}

void cputc(unsigned char c)
{
    cputcxy(g_curX, g_curY, c);
}

unsigned char cprintf (const char* fmt, ...)
{
    // TODO: IMPLEMENT.

    // unsigned char readItems = 0;
    // unsigned char flags = 0;
    
    // va_list va;
    
    while(*fmt)
    {
        cputc(*fmt++);
    }
}

void cputhex(long n, unsigned char prec)
{
    unsigned char buffer[10];
    buffer[0] = '$';
    buffer[1] = hexDigits[(n & 0xF0000000UL) >> 28];
    buffer[2] = hexDigits[(n & 0x0F000000UL) >> 24];
    buffer[3] = hexDigits[(n & 0x00F00000UL) >> 20];
    buffer[4] = hexDigits[(n & 0x000F0000UL) >> 16];
    buffer[5] = hexDigits[(n & 0x0000F000UL) >> 12];
    buffer[6] = hexDigits[(n & 0x00000F00UL) >> 8];
    buffer[7] = hexDigits[(n & 0x000000F0UL) >> 4];
    buffer[8] = hexDigits[(n & 0x0000000FUL) ];
    buffer[9] = '\0';
    buffer[8 - prec] = '$';
    cputs(&buffer[8 - prec]);
}

void cputdec(long n, unsigned char padding, unsigned char leadingZeros)
{
    unsigned char buffer[11];
    unsigned char rem = 0, i = 0;
    char digit = 9;
    buffer[10] = '\0';
    do
    {
        rem = n % 10;
        n /= 10;
        buffer[digit--] = hexDigits[rem];
    } while ( (digit >= 0) && (n != 0) );

    while ( (digit >= 0) && (leadingZeros--))
    {
        buffer[digit--] = hexDigits[0];
    }

    cputs(&buffer[digit + 1]);
}

void cputs(const char *s)
{
    cputsxy(g_curX, g_curY, s);
}

void cputsxy(unsigned char x, unsigned char y, const char *s)
{
    unsigned char w = 0, h = 0, len = strlen(s);
    screensize(&w, &h);
    lcopy( (long) s, SCREEN_ADDRESS + (y * (unsigned int) w) + x, len);     
    lfill(COLOR_RAM_BASE + (y * (unsigned int) w) + x, g_curTextColor, len);
    g_curY = y + ((x + len) / w);
    g_curX = (x + len) % w;
}

void cputcxy (unsigned char x, unsigned char y, char c)
{
    unsigned char w = 0, h = 0;
    screensize(&w, &h);
    lpoke(SCREEN_ADDRESS + (y * (unsigned int) w) + x, c);
    lpoke(COLOR_RAM_BASE + (y * (unsigned int) w) + x, g_curTextColor);
    g_curX = (x == w - 1) ? 0 : (x + 1);
    g_curY = (x == w - 1) ? (y + 1) : y;
}

unsigned char cgetc (void)
{
    unsigned char k;
    POKE(0xD610U,0);
    while ((k = PEEK(0xD610U)) == 0);
    return k;
}

unsigned char kbhit(void)
{
    return PEEK(0xD610U);
}
