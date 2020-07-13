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
 
    Version   0.5
    Date      2020-07-13

    CHANGELOG

    v0.4        Added getscreensize, setscreensize, setextendattribute, 
                set16bitcharmode, moveup,moveleft,moveright, movedown, gohome,
                flushkeybuf.
                Cache screen sizes for faster calls.
                Added cprintf escape codes for formatted screen colors and attributes.
                Added proper initialization function.
                Fixed a bug where screen was fixed at $8000!

    v0.5        Added fillrect, box, cgets, wherex, wherey, togglecase functions.
                Fixed moveXXXX to do multiple steps.  Minor optimizations in cputs/cputc.

*/

#ifndef M65LIBC_CONIO_H
#define M65LIBC_CONIO_H

#define VIC_BASE            0xD000UL
#define IS_H640             (PEEK(VIC_BASE + 0x31) & 128)
#define IS_V400             (PEEK(VIC_BASE + 0x31) & 8)
#define SET_H640()          POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) | 128)
#define CLEAR_H640()        POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) & 127)
#define SET_V400()          POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) | 8)
#define CLEAR_V400()        POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) & 0xF7)
#define IS_16BITCHARSET     (PEEK(VIC_BASE + 0x54) & 1)
#define SET_16BITCHARSET()  POKE(VIC_BASE + 0x54, PEEK(VIC_BASE + 0x54) |1)
#define CLEAR_16BITCHARSET() POKE(VIC_BASE + 0x54, PEEK(VIC_BASE + 0x54) & 0xFE)
#define IS_EXTATTR()        (PEEK(VIC_BASE + 0x31) & 32)
#define SET_EXTATTR()        POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) | 32)
#define CLEAR_EXTATTR()      POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) & 0xDF)
#define SCREEN_RAM_BASE_B0  (PEEK(VIC_BASE + 0x60)) // LSB
#define SCREEN_RAM_BASE_B1  (PEEK(VIC_BASE + 0x61))
#define SCREEN_RAM_BASE_B2  (PEEK(VIC_BASE + 0x62))
#define SCREEN_RAM_BASE_B3  (PEEK(VIC_BASE + 0x63) & 7) // upper nybble
#define SCREEN_RAM_BASE     ( ((long)SCREEN_RAM_BASE_B3 << 24) | ((long)SCREEN_RAM_BASE_B2 << 16) | ((long)SCREEN_RAM_BASE_B1 << 8) | (SCREEN_RAM_BASE_B0) )  
#define COLOR_RAM_BASE      0x1F800UL

/*------------------------------------------------------------------------
  Color and attributes
  -----------------------------------------------------------------------*/
#define ATTRIB_BLINK      0x10
#define ATTRIB_REVERSE    0x20
#define ATTRIB_UNDERLINE  0x80
#define ATTRIB_HIGHLIGHT  0x40

#define COLOUR_BLACK        0
#define COLOUR_WHITE        1
#define COLOUR_RED          2
#define COLOUR_CYAN         3
#define COLOUR_PURPLE       4
#define COLOUR_GREEN        5
#define COLOUR_BLUE         6
#define COLOUR_YELLOW       7
#define COLOUR_ORANGE       8
#define COLOUR_BROWN        9
#define COLOUR_PINK         10
#define COLOUR_GREY1        11
#define COLOUR_DARKGREY     11
#define COLOUR_GREY2        12
#define COLOUR_GREY         12
#define COLOUR_MEDIUMGREY   12
#define COLOUR_LIGHTGREEN   13
#define COLOUR_LIGHTBLUE    14
#define COLOUR_GREY3        15
#define COLOUR_LIGHTGREY    15

/*------------------------------------------------------------------------
  Keyboard ASCII codes 
  -----------------------------------------------------------------------*/
#define ASC_A
#define ASC_B
#define ASC_C
#define ASC_D
#define ASC_E
#define ASC_F
#define ASC_G
#define ASC_H
#define ASC_I
#define ASC_J
#define ASC_K
#define ASC_L
#define ASC_M
#define ASC_N
#define ASC_O
#define ASC_P
#define ASC_Q
#define ASC_R
#define ASC_S
#define ASC_T
#define ASC_U
#define ASC_V
#define ASC_W
#define ASC_X
#define ASC_Y
#define ASC_Z
#define ASC_F1
#define ASC_F3
#define ASC_F5
#define ASC_F7
#define ASC_F9
#define ASC_F11
#define ASC_F13
#define ASC_CRSR_RIGHT
#define ASC_CRSR_LEFT
#define ASC_CRSR_UP
#define ASC_CRSR_DOWN

/*------------------------------------------------------------------------
  Keyboard modifiers
  -----------------------------------------------------------------------*/
#define KEYMOD_RSHIFT   1
#define KEYMOD_LSHIFT   2
#define KEYMOD_CTRL     4
#define KEYMOD_MEGA     8
#define KEYMOD_ALT      16
#define KEYMOD_NOSCRL   32
#define KEYMOD_CAPSLOCK 64

/*------------------------------------------------------------------------
  Box styles
  -----------------------------------------------------------------------*/
#define BOX_STYLE_INNER 0
#define BOX_STYLE_MID   1
#define BOX_STYLE_OUTER 2
#define BOX_STYLE_ROUND 3

/*------------------------------------------------------------------------
  Public structs
  -----------------------------------------------------------------------*/
typedef struct tagRECT
{
    unsigned char left, top, right, bottom;
} RECT;

/*------------------------------------------------------------------------
  Screen configuration and setup
  -----------------------------------------------------------------------*/

/* Initialize library internal state */
void conioinit(void);

/* Sets the active screen RAM address */
void setscreenaddr(long addr);

/* Gets the currently active screen RAM address */
long getscreenaddr(void);

 /* Clear the text screen. Color RAM will be cleared with current text color */
void clrscr(void); 

 /* Returns the dimensions of the text mode screen.  
    Ignores any virtual chargen dimensions */
void fastcall getscreensize(unsigned char* width, unsigned char* height);

 /* Sets the screen size in rows and columns of text.
    Unsupported values are currently ignored */
void fastcall setscreensize(unsigned char width, unsigned char height);

/* Sets or clear the 16-bit character mode */
void fastcall set16bitcharmode(unsigned char f);

/* Sets or clear the extended attribute mode (blink, underline, bold, highlight)*/
void fastcall setextendedattrib(unsigned char f);

/* Toggles the character set case */
void fastcall togglecase(void);

/*------------------------------------------------------------------------
  Color and Attributes
  -----------------------------------------------------------------------*/

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

/* Set color of character cell */
void cellcolor(unsigned char x, unsigned char y, unsigned char c);


/*------------------------------------------------------------------------
  Screen draw operations
  -----------------------------------------------------------------------*/

/* Fill a rectangular area with character and color value */
void fillrect(const RECT *rc, unsigned char ch, unsigned char col);

/* Draws a box with graphic characters. Style can be set to BOX_STYLE_ROUNDED,
   BOX_STYLE_INNER, BOX_STYLE_OUTER, BOX_STYLE_MID */
void box(const RECT *rc, unsigned char color, unsigned char style, 
         unsigned char clear, unsigned char shadow);



/*------------------------------------------------------------------------
  Cursor Movement
  -----------------------------------------------------------------------*/

/* Put cursor at home (0,0) */
void fastcall gohome(void);

/* Put cursor at X,Y. 
   The function does not check for screen bounds! */
void fastcall gotoxy(unsigned char x, unsigned char y);

/* Put cursor at column X. The function does not check for screen bounds */
void fastcall gotox(unsigned char x);

/* Put cursor at row Y. The function does not check for screen bounds */
void fastcall gotoy(unsigned char x);

/* Move cursor up X times with wraparound */
void fastcall moveup(unsigned char count);

/* Move cursor down X times with wraparound */
void fastcall movedown(unsigned char count);

/* Move cursor left X times, going to next line.*/ 
void fastcall moveleft(unsigned char count);

/* Move cursor right X times, going to prev line*/
void fastcall moveright(unsigned char count);

/* Enable cursor when input read */
void fastcall cursor(unsigned char enable);

/* Get current X position */
unsigned char wherex(void);

/* Get current Y position */
unsigned char wherey(void);

/*------------------------------------------------------------------------
  Text output
  -----------------------------------------------------------------------*/

/* Output a single character at current position */
void fastcall cputc(unsigned char c);

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

/*  Print formatted output. 
    
    Escape strings can be used to modify attributes, move cursor,etc,
    similar to PRINT in CBM BASIC. Available escape codes:
   
    Cursor positioning 

    \t           Go to next tab position (multiple of 8s)
    \r           Return
    \n           New-line (assume \r like in C printf)

    {clr}        Clear screen        {home}      Move cursor to home (top-left)
    {d}          Move cursor down    {u}        Move cursor up
    {r}          Move cursor right   {l}      Move cursor left

    Attributes

    {rvson}    Reverse attribute ON   {rvsoff}   Reverse attribute OFF
    {blon}     Blink attribute ON     {bloff}    Blink attribute OFF
    {ulon}     Underline attribute ON {uloff}    Underline attribute OFF

    Colors (default palette 0-15)
    {blk}    {wht}    {red}   {cyan}  {pur}   
    {grn}    {blu}    {yel}   {ora}   {brn}    
    {pink}   {gray1}  {gray2} {lblu}  {lgrn} 
    {gray3}
*/

unsigned char cprintf (const unsigned char* format, ...);

/*------------------------------------------------------------------------
  Keyboard input 
  -----------------------------------------------------------------------*/
/* Wait until a character is in the keyboard buffer and return it */
unsigned char fastcall cgetc (void);

/* Return the character in the keyboard buffer, if any */
unsigned char fastcall kbhit (void);

/* Return the key modifiers state, where bits:

    Bit           Meaning
    -----------------------------------------
    0             Right SHIFT state
    1             Left  SHIFT state
    2             CTRL state
    3             MEGA/C= state
    4             ALT state
    5             NOSCRL state
    6             CAPSLOCK state
    7             Reserved
*/
unsigned char getkeymodstate(void);

/* Flush the hardware keyboard buffer */
void flushkeybuf(void);

void cscanf(const char* format, ...);


#endif //M65LIBC_CONIO_H


