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
 
    Version   0.4
    Date      2020-06-28

    CHANGELOG

    v0.4        Added getscreensize, setscreensize, setextendattribute, 
                set16bitcharmode, moveup,moveleft,moveright, movedown, gohome,
                flushkeybuf.
                Cache screen sizes for faster calls.
                Added cprintf escape codes for formatted screen colors and attributes.
                Added proper initialization function.
                Fixed a bug where screen was fixed at $8000!

*/

#include "../include/conio.h"
#include <string.h>

#define PRINTF_IN_FORMAT_SPEC    0x1
#define PRINTF_FLAGS_LEADINGZERO 0x2
#define PRINTF_STATE_INIT        0
#define PRINTF_STATE_ESCAPE      1

// cprintf Screen Control Escape Codes.
//
// See ESCAPE_HASH() for how to generate.
//
typedef struct tagESCAPE_CODE
{
    unsigned char arg;
    void (*fn)(unsigned char);
} ESCAPE_CODE;

static ESCAPE_CODE escapeCode[255];
static unsigned char g_curTextColor = COLOUR_WHITE;
static unsigned char g_curX = 0;
static unsigned char g_curY = 0;
static unsigned char g_curScreenW = 0;
static unsigned char g_curScreenH = 0;
static const unsigned char hexDigits[] = { '0','1','2','3','4','5','6','7','8','9',0x41,0x42,0x43,0x44,0x45,0x46};

static unsigned char hash(const unsigned char *str, const unsigned char maxLen)
{    
    unsigned long hash = 277;
    register unsigned char c;
    register unsigned char len = 0;
    while ( (c = *str++) && (len < maxLen))
    {
        len++;
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

static void clrscr_(unsigned char) { clrscr(); gohome(); } // Callable from Escape Code table 
static void gohome_(unsigned char) { gohome(); } // Callable from Escape Code table
static void escNOP(unsigned char) { /* do nothing */ }

void conioinit(void)
{
    register unsigned char i = 0;

    // Make sure we go to VIC-IV IO mode

    POKE(0xD02fL, 0x47);
    POKE(0xd02fL, 0x53);

    g_curScreenW = IS_H640 ? 80 : 40;
    g_curScreenH = IS_V400 ? 50 : 25;

    flushkeybuf();
    
    for (i = 0; i < sizeof(escapeCode)/sizeof(escapeCode[0]); ++i) 
    {
        escapeCode[i].fn = escNOP;
        escapeCode[i].arg = 0x0;
    }

    // Setup escape codes according to it's hashed strings. 
    // We know that for those codes and with k=277 there are no collisions.
    // Adding new codes should verify no collisions are added by changing k 
    // or by using another algorithm.

    escapeCode[1].fn = moveleft;
    escapeCode[7].fn = moveright;
    escapeCode[10].fn = moveup;
    escapeCode[22].fn = clrscr_;
    escapeCode[30].fn = gohome_;
    escapeCode[49].fn = underline;
    escapeCode[57].fn = textcolor;
    escapeCode[57].arg = COLOUR_GREY1;
    escapeCode[58].fn = textcolor;
    escapeCode[58].arg = COLOUR_GREY2;
    escapeCode[59].fn = textcolor;
    escapeCode[59].arg = COLOUR_GREY3;
    escapeCode[64].fn = textcolor;
    escapeCode[64].arg = COLOUR_CYAN;
    escapeCode[68].fn = textcolor;
    escapeCode[68].arg = COLOUR_LIGHTBLUE;
    escapeCode[72].fn = textcolor;
    escapeCode[72].arg = COLOUR_LIGHTGREEN;
    escapeCode[96].fn = blink;
    escapeCode[96].arg = 1;
    escapeCode[139].fn = revers;
    escapeCode[140].fn = textcolor;
    escapeCode[140].arg = COLOUR_PURPLE;
    escapeCode[147].fn = underline;
    escapeCode[147].arg = 1;
    escapeCode[151].fn = textcolor;
    escapeCode[151].arg = COLOUR_BROWN;
    escapeCode[158].fn = blink;
    escapeCode[168].fn = textcolor;
    escapeCode[168].arg = COLOUR_WHITE;
    escapeCode[173].fn = revers;
    escapeCode[173].arg = 1;
    escapeCode[191].fn = textcolor;
    escapeCode[191].arg = COLOUR_YELLOW;
    escapeCode[199].fn = textcolor;
    escapeCode[199].arg = COLOUR_PINK;
    escapeCode[206].fn = textcolor;
    escapeCode[206].arg = COLOUR_BLACK;
    escapeCode[215].fn = textcolor;
    escapeCode[215].arg = COLOUR_ORANGE;
    escapeCode[216].fn = textcolor;
    escapeCode[216].arg = COLOUR_BLUE;
    escapeCode[220].fn = textcolor;
    escapeCode[220].arg = COLOUR_GREEN;
    escapeCode[240].fn = textcolor;
    escapeCode[240].arg = COLOUR_RED;
    escapeCode[249].fn = movedown;
}

void setscreenaddr(long address)
{
    POKE(VIC_BASE + 0x60, address & 0x0000FFUL);
    POKE(VIC_BASE + 0x61, (address & 0xFF00UL) >> 8);
    POKE(VIC_BASE + 0x62, (address & 0xFF0000UL) >> 16);
    POKE(VIC_BASE + 0x63, (PEEK(VIC_BASE + 0x63) & 0xF) | ((address & 0xF000000UL) >> 24));
}

long getscreenaddr(void)
{
    return SCREEN_RAM_BASE;
}

void setscreensize(unsigned char w, unsigned char h)
{
    if (w == 80)
        SET_H640();
    else if (w == 40)
        CLEAR_H640();
    
    if (h == 50)
        SET_V400();
    else if (h == 25)
        CLEAR_V400();

    // Cache values.
    if (w == 40 || w == 80)
        g_curScreenW = w;
    if (h == 25 || h == 50)
        g_curScreenH = h;
}

void getscreensize(unsigned char* width, unsigned char *height)
{
    *width = g_curScreenW;
    *height = g_curScreenH;
}

void set16bitcharmode(unsigned char f)
{
    if (f)
        SET_16BITCHARSET();
    else
        CLEAR_16BITCHARSET();
}

void setextendedattrib(unsigned char f)
{
    if (f)
        SET_EXTATTR();
    else
        CLEAR_EXTATTR();
}

void clrscr()
{
    unsigned int cBytes = 0;
    cBytes = (unsigned int)g_curScreenW * g_curScreenH * (IS_16BITCHARSET ? 2 : 1);
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

void cellcolor(unsigned char x, unsigned char y, unsigned char c)
{
    lpoke(COLOR_RAM_BASE + (y * (unsigned int) g_curScreenW) + x, c);
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

void gohome(void)
{
    gotoxy(0,0);
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


void  moveup(unsigned char count)
{
    g_curY--;
}

void  movedown(unsigned char count)
{
    g_curY++;
}

void  moveleft(unsigned char count)
{
    g_curX--;
}

void  moveright(unsigned char count)
{
    g_curX++;
}


unsigned char cprintf (const unsigned char* fmt, ...)
{
    unsigned char printfState = PRINTF_STATE_INIT;
    unsigned char escHash = 0;
    unsigned char cch = 0;

    while (*fmt)
    {
        switch (printfState)
        {
        case PRINTF_STATE_INIT:
            switch (*fmt)
            {
            case '{':
                printfState = PRINTF_STATE_ESCAPE;
                break;

            case '\t': // Tab
                //moveleft((g_curX + 7) / 8) * 8, 1);
                break;

            case '\n': // New-line
                gotoxy(0, g_curY + 1);
                break;

            default:
                cputc(*fmt);
            }
            break;

        case PRINTF_STATE_ESCAPE:
            if (*fmt == '{') // print literal
            {
                cputc(*fmt);
                printfState = PRINTF_STATE_INIT;
                break;
            }
            
            cch = 0;
            while(fmt &&  (*fmt != '}'))
            {
                fmt++;
                cch++;
            }

            if (*fmt != '}') // bailout.
                return 255;
            
            escHash = hash(fmt - cch, cch);
            escapeCode[escHash].fn(escapeCode[escHash].arg);
            printfState = PRINTF_STATE_INIT;
            break;
        }

        fmt++;
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
    unsigned char len = strlen(s);
    lcopy( (long) s, SCREEN_RAM_BASE + (y * (unsigned int) g_curScreenW) + x, len);     
    lfill(COLOR_RAM_BASE + (y * (unsigned int) g_curScreenW) + x, g_curTextColor, len);
    g_curY = y + ((x + len) / g_curScreenW);
    g_curX = (x + len) % g_curScreenW;
}

void cputcxy (unsigned char x, unsigned char y, char c)
{
    lpoke(SCREEN_RAM_BASE + (y * (unsigned int) g_curScreenW) + x, c);
    lpoke(COLOR_RAM_BASE + (y * (unsigned int) g_curScreenW) + x, g_curTextColor);
    g_curX = (x == g_curScreenW - 1) ? 0 : (x + 1);
    g_curY = (x == g_curScreenW - 1) ? (y + 1) : y;
}

unsigned char cgetc (void)
{
    unsigned char k;
    while ((k = PEEK(0xD610U)) == 0);
    POKE(0xD610U,0);
    return k;
}

unsigned char getkeymodstate(void)
{
    return PEEK(0xD611U);
}

unsigned char kbhit(void)
{
    return PEEK(0xD610U);
}

void flushkeybuf(void)
{
    while (PEEK(0xD610U))
        POKE(0xD610U,0);
}