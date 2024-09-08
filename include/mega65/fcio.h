/**
 * @file fcio.h
 * @brief Full colour mode i/o library for the MEGA65
 *
 * Takes care of displaying text and images in full colour and super-extended
 * character modes. It has its own tutorial, located
 * [here](https://steph72.github.io/fcio-tutorial).
 *
 * If in C64 mode you must call `mega65_io_enable()` found in `memory.h`
 * before using any of the fcio functions.
 */

#ifndef __MEGA65_FCIO_H
#define __MEGA65_FCIO_H

#include <mega65/memory.h>
#include <stdbool.h>

#define FCBUFSIZE 0xff

#define COLOUR_BLACK 0       //!< Black color
#define COLOUR_WHITE 1       //!< White color
#define COLOUR_RED 2         //!< Red color
#define COLOUR_CYAN 3        //!< Cyan color
#define COLOUR_PURPLE 4      //!< Purple color
#define COLOUR_GREEN 5       //!< Green color
#define COLOUR_BLUE 6        //!< Blue color
#define COLOUR_YELLOW 7      //!< Yellow color
#define COLOUR_ORANGE 8      //!< Orange color
#define COLOUR_BROWN 9       //!< Brown color
#define COLOUR_PINK 10       //!< Pink color
#define COLOUR_GREY1 11      //!< Grey1 color
#define COLOUR_DARKGREY 11   //!< Dark grey color
#define COLOUR_GREY2 12      //!< Grey2 color
#define COLOUR_GREY 12       //!< Grey color
#define COLOUR_MEDIUMGREY 12 //!< Medium grey color
#define COLOUR_LIGHTGREEN 13 //!< Light green color
#define COLOUR_LIGHTBLUE 14  //!< Light blue color
#define COLOUR_GREY3 15      //!< Grey3 color
#define COLOUR_LIGHTGREY 15  //!< Light grey color

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef himemPtr
typedef unsigned long himemPtr;
#endif

#ifndef word
typedef unsigned int word;
#endif

typedef struct _fcioConf {
    himemPtr screenBase;
    himemPtr reservedBitmapBase;
    himemPtr reservedPaletteBase;
    himemPtr dynamicPaletteBase;
    himemPtr dynamicBitmapBase;
    himemPtr colourBase;
} fcioConf;

extern char* fcbuf;

// --------------- graphics ------------------

typedef struct _fciInfo {
    himemPtr baseAdr;        ///< bitmap base address
    himemPtr paletteAdr;     ///< palette data base address
    byte paletteSize;        ///< size of palette (in palette entries)
    bool reservedSysPalette; ///< if true, don't use colours 0-15
    byte columns;            ///< number of character columns for image
    byte rows;               ///< number of character rows
    word size;               ///< size of bitmap
} fciInfo;

typedef struct _textwin {
    byte xc;            ///< current cursor X position
    byte yc;            ///< current cursor Y position
    byte x0;            ///< X origin
    byte y0;            ///< Y origin
    byte width;         ///< window width
    byte height;        ///< window height
    byte textcolor;     ///< current window text colour
    byte extAttributes; ///< current window extended text attributes
} textwin;

extern byte gScreenColumns;  ///< number of screen columns (in characters)
extern byte gScreenRows;     ///< number of screen rows (in characters)
extern textwin* gCurrentWin; ///< current window

// --- general & initializations ---

/**
 * @brief initialize fcio library with given initial screen mode
 *
 * @param h640 horizontal resolution; true: 640px, false: 320px
 * @param v400 vertical resolution; true: 400px, false: 200px
 * @param config pointer to FCIO config block (or NULL for standard
 * configuration)
 * @param rows character rows (or 0 for default row count)
 * @param reservedBitmapFile reserved bitmap to load upon initialization (or
 * NULL for none)
 */
void fc_init(byte h640, byte v400, fcioConf* config, byte rows,
    char* reservedBitmapFile);

/**
 * @brief set new full colour screen mode
 *
 * @param h640 horizontal resolution; true: 640px, false: 320px
 * @param v400 vertical resolution; true: 400px, false: 200px
 * @param rows character rows (or 0 for standard configuration)
 */
void fc_screenmode(byte h640, byte v400, byte rows);

/**
 * @brief fall back to 8 bit screen mode
 *
 */
void fc_go8bit(void);

/**
 * @brief display fatal error message and stop execution
 *
 * @param format printf-style format string
 * @param ... format string parameters
 */
void fc_fatal(const char* format, ...);

// ----------------------------------------------------------------------------
// lines and blocks
// ----------------------------------------------------------------------------

/**
 * @brief draw a block in the current window
 *
 * @param x0 origin x
 * @param y0 origin y
 * @param width block width
 * @param height block height
 * @param character ascii character to plot
 * @param col colour to plot
 */
void fc_block(
    byte x0, byte y0, byte width, byte height, byte character, byte col);

/**
 * @brief draw a horizontal line using extended (==fcm bitmap) characters
 *
 * @param x origin x
 * @param y origin y
 * @param width width
 * @param lineChar bitmap character index to use
 */
void fc_hlinexy(byte x, byte y, byte width, byte lineChar);

/**
 * @brief draw a vertical line using extended (==fcm bitmap) characters
 *
 * @param x origin x
 * @param y origin y
 * @param height height
 * @param lineChar bitmap character index to use
 */
void fc_vlinexy(byte x, byte y, byte height, byte lineChar);
void fc_line(byte x, byte y, byte width, byte character, byte col);

// ----------------------------------------------------------------------------
// --- keyboard input ---
// ----------------------------------------------------------------------------

void fc_emptyBuffer(void);
unsigned char fc_cgetc(void);
char fc_getkey(void);
char fc_getkeyP(byte x, byte y, const char* prompt);

/**
 * @brief get user input
 *
 * @param maxlen maximum length
 * @return char* a newly allocated area in memory containing the input string
 */
char* fc_input(byte maxlen);
int fc_getnum(byte maxlen);

// ----------------------------------------------------------------------------
// --- string and character output ---
// ----------------------------------------------------------------------------

/**
 * @brief clears the current text window
 *
 */
void fc_clrscr(void);

/**
 * @brief put character at current cursor position
 *
 * @param c the character
 */
void fc_putc(char c);

/**
 * @brief put string at current cursor position
 *
 * @param s the string
 */
void fc_puts(const char* s);

/**
 * @brief put string at given position in current window
 *
 * @param x
 * @param y
 * @param s the string
 */
void fc_putsxy(byte x, byte y, char* s);

/**
 * @brief put character at given position in current window
 *
 * @param x
 * @param y
 * @param c the character
 */
void fc_putcxy(byte x, byte y, char c);

/**
 * @brief print string at current cursor position
 *
 * @param format format string
 * @param ... format parameters
 */
void fc_printf(const char* format, ...);

/**
 * @brief set cursor position in current window
 *
 * @param x
 * @param y
 */
void fc_gotoxy(byte x, byte y);

/**
 * @brief get cursor x position
 *
 * @return byte cursor y position
 */
byte fc_wherex(void);

/**
 * @brief get cursor y position
 *
 * @return byte cursor y position
 */
byte fc_wherey(void);

/**
 * @brief set auto CR
 *
 * @param a whether to automatically issue a CR when the text cursor reaches the
 * right border
 */
void fc_setAutoCR(bool a);

/**
 * @brief set active window
 *
 * @param aWin window to activate
 */
void fc_setwin(textwin* aWin);

/**
 * @brief convenience method to create a window structure
 *
 * @param x0 x origin
 * @param y0 y origin
 * @param width window width
 * @param height window height
 * @return textwin* the created window structure
 */
textwin* fc_makeWin(byte x0, byte y0, byte width, byte height);

/**
 * @brief reset text window to whole screen
 *
 */
void fc_resetwin(void);

/**
 * @brief cursor control
 *
 * @param onoff 1=on, 0=off
 */
void fc_cursor(byte onoff);

/**
 * @brief center string at given position
 *
 * @param x x origin
 * @param y y origin
 * @param width width of area
 * @param text text to center
 */
void fc_center(byte x, byte y, byte width, char* text);

/**
 * @brief scrolls the content of the current window up
 *
 */
void fc_scrollUp(void);

/**
 * @brief scrolls the contents of the current window down
 *
 */
void fc_scrollDown(void);

// ----------------------------------------------------------------------------
// colour, attributes and palette handling
// ----------------------------------------------------------------------------

/**
 * @brief swap nybl of a byte
 *
 * @param in colour byte
 * @return in with swapped nybbels
 */
unsigned char fc_nyblswap(unsigned char in);

/**
 * @brief set or reset the revers attribute
 *
 * @param f reverse attribute flag
 */
void fc_revers(byte f);

/**
 * @brief set or reset the flash attribute
 *
 * @param f flash attribute flag
 */
void fc_flash(byte f);

/**
 * @brief set or reset the bold attribute
 *
 * @param f bold attribute flag
 */
void fc_bold(byte f);

/**
 * @brief set or reset the underline attribute
 *
 * @param f underline attribute flag
 */
void fc_underline(byte f);

/**
 * @brief load palette data into the VIC
 *
 * @param adr address palette data (de-nybellized triplets)
 * @param size size (in palette entries)
 * @param reservedSysPalette whether to overwrite colours 0-15
 */
void fc_loadPalette(himemPtr adr, byte size, byte reservedSysPalette);

/**
 * @brief set palette entry
 *
 * @param num palette index (0-255)
 * @param red red (0-255)
 * @param green green (0-255)
 * @param blue blue (0-255)
 */
void fc_setPalette(int num, byte red, byte green, byte blue);

/**
 * @brief load FCI palette into the VIC
 *
 * @param info pointer to FCI image info block
 */
void fc_loadFCIPalette(fciInfo* info);

/**
 * @brief set palette entries to black
 *
 * @param reservedSysPalette exclude colours 0-15
 */
void fc_zeroPalette(byte reservedSysPalette);

/**
 * @brief fade palette to or from the desired values
 *
 * @param adr address of palette data
 * @param size number of palette entries
 * @param reservedSysPalette exclude colours 0-15
 * @param steps number of steps for fade (max. 255)
 * @param fadeOut if true, fade out rather than in
 */
void fc_fadePalette(
    himemPtr adr, byte size, byte reservedSysPalette, byte steps, bool fadeOut);

/**
 * @brief reset palette to reserved FCI palette
 *
 * If a reserved FCI is used (see @a fc_loadReservedFCI), reset the current
 * palette to the reserved one.
 */
void fc_resetPalette(void);

/**
 * @brief set text colour of current window
 *
 * @param c text colour index
 */
void fc_textcolor(byte c);

/**
 * @brief set border colour
 *
 */
#define fc_bordercolor(C) POKE(0xd020u, C)

/**
 * @brief set background colour
 *
 */
#define fc_bgcolor(C) POKE(0xd021u, C)

// ----------------------------------------------------------------------------
// graphic areas
// ----------------------------------------------------------------------------

/**
 * @brief free automatically allocated graphic areas
 *
 */
void fc_freeGraphAreas(void);

/**
 * @brief Adds a graphics rectangle to the screen.
 *
 * @param x0 x origin (in characters)
 * @param y0 y origin (in characters)
 * @param width width (in characters)
 * @param height height (in characters)
 * @param bitmapData address of bitmap data
 *
 * Display a bitmap that has been loaded with @a fc_loadFCI without
 * setting the palette (useful for fading in images after loading)
 *
 */
void fc_addGraphicsRect(
    byte x0, byte y0, byte width, byte height, himemPtr bitmapData);

/**
 * @brief allocate memory for FCI file and load it
 *
 * @param filename name of FCI file to load
 * @param address address to load bitmap (or 0 for automatic allocation)
 * @param pAddress address to load palette (or 0 for automatic allocation)
 * @return fciInfo* info block containging start address and metadata
 *
 * This function is used to load an FCI image into memory. If used with
 * automatic memory allocation (0 for @a address and @a pAddress), memory is
 * taken from banks 4 and 5 to store the bitmap data.
 *
 * When automatically alloacting graphic areas, it is quite possible to run
 * out of memory. Therefore, it is advisable to always clear previously
 * allocated graphic areas with @a fc_freeGraphicAreas after usage.
 *
 * @warning only loads the picture, doesn't display it!
 *
 */
fciInfo* fc_loadFCI(char* filename, himemPtr address, himemPtr paletteAddress);

/**
 * @brief display FCI image
 *
 * @param info FCI image info block
 * @param x0 x origin (in characters)
 * @param y0 y origin (in characters)
 * @param setPalette also overwrites current palette with image palette if true
 */
void fc_displayFCI(fciInfo* info, byte x0, byte y0, bool setPalette);

/**
 * @brief fade in FCI image
 *
 * @param info FCI image info block
 * @param x0 x origin (in characters)
 * @param y0 y origin (in characters)
 * @param steps number of steps for fade (255=max)
 */
void fc_fadeFCI(fciInfo* info, byte x0, byte y0, byte steps);

/**
 * @brief load FCI file and display it
 *
 * @param filename FCI file to load
 * @param x0 origin x
 * @param y0 origin y
 * @return fciInfo* associated fciInfo block for file
 */
fciInfo* fc_displayFCIFile(char* filename, byte x0, byte y0);

/**
 * @brief plot extended (==full colour) character
 *
 * @param x screen column
 * @param y screen row
 * @param c character index
 */
void fc_plotExtChar(byte x, byte y, byte c);

/**
 * @brief plot screencode character
 *
 * @param x screen column
 * @param y screen row
 * @param c character code
 * @param color character colour
 * @param exAttr extended attributes
 */
void fc_plotScreenChar(byte x, byte y, byte c, byte color, byte exAttr);

/**
 * @deprecated The function name is misleading as the supplied character code
 *             is expected to be a screen character code, not a PETSCII code.
 *             Use @ref fc_plotScreenChar(byte, byte, byte, byte, byte) instead.
 * @brief plot screencode character
 *
 * @param x screen column
 * @param y screen row
 * @param c character code
 * @param color character colour
 * @param exAttr extended attributes
 */
#ifdef __clang__
[[deprecated("Use fc_plotScreenChar() instead.")]]
#endif
void fc_plotPetsciiChar(byte x, byte y, byte c, byte color, byte exAttr);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_FCIO_H
