# MEGA65-libc

Simple C library for the MEGA65

# Using the library

- **CC65**: Include the `.c` and `.s` files from the `src/` directory that you need.
- **Clang**: Either use the provided CMake setup as detailed below, or include the `.c` and `.h` files you need.
- **KickC**: Support will be coming soon (or sooner, if someone would like to port the routines :-)

# Development and building

## CC65

1. Install [CC65](https://cc65.github.io) with e.g. `brew install cc65` or `apt install cc65`.
2. Build by running `make -f Makefile_cc65` inside the `mega65-libc/` directory.

## Clang

1. Install [llvm-mos-sdk](https://github.com/llvm-mos/llvm-mos-sdk#getting-started).
2. Configure and make with:
~~~ sh
cmake -DCMAKE_PREFIX_PATH=<llvm-mos-sdk-install-prefix> -B build/ mega65-libc/
cd build/
make
make test # if `xmega65` (Xemu) is in your path
~~~

### Dependent CMake projects

`CMakeLists.txt` of a dependent project could look like this:
~~~ cmake
cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
project(myproject VERSION 0.1.0 LANGUAGES C)
find_package(mega65libc REQUIRED)
add_compile_options(-mcpu=mos65ce02 -Os -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token)
add_executable(main main.c)
target_link_libraries(main mega65libc::mega65libc)
set_target_properties(main PROPERTIES OUTPUT_NAME main.prg)
~~~
See more [here](https://github.com/llvm-mos/llvm-mos-sdk#developing-for-6502-with-cmake).

### CPM.cmake dependency manager

If using [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake),
`mega65libc` can be easily added and automatically downloaded to your project with:
~~~ cmake
CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY mega65/mega65-libc GIT_TAG master)
target_link_libraries(<mytarget> mega65libc)
~~~

# Function descriptions

## FAT32 File Access

```
void closeall(void);
unsigned char open(char *filename);
void close(unsigned char fd);
unsigned short read512(unsigned char fd,unsigned char *buffer);
```

To use these functions you must include `fileio.h`

## FAT32 Directory Access

Functions similar to the POSIX equivalents are provided. Key differences are that `unsigned char *`
is used instead of `DIR *` for the directory handle, and `readdir()` returns a pointer to a `m65_dirent` struct,
instead of `dirent`struct.

As file handle support is still partial in the hypervisor, only one directory or file can be safely open at any
point in time, and it is wise to call `closeall()` before opening any file or directory.

`opendir()` currently takes no path as input, as the Hypervisor can only work on a single directory at the moment.
Support for sub-directories will come in the fullness of time.

```
unsigned char *opendir(void);
m65_dirent *readdir(unsigned char *dir_handle);
void closedir(unsigned char *dir_handle);
```

To use these functions you must include `dirent.h` 

## Clock Access

`getrtc(struct m65_tm *)` and `setrtc(struct struct m65_tm *)` allow retrieval and setting of the real-time-clock
(RTC) using structures broadly similar to the posix tm structure.
These routines abstract the different model RTCs that exist on different MEGA65 hardware targets.

To use these functions you must include `time.h`

## Full Colour Mode I/O 

The `fcio` portion of the mega65-libc takes care of displaying text and images in full colour & super-extended character modes. 
It has its own tutorial, located <a href="https://steph72.github.io/fcio-tutorial/">here.</a>

## Text Console I/O

The `conio.h` file is projected to support all MEGA65 text features in the future.  

Keep in mind!

1) Use `conioinit()` to initialize internal state.
2) **no bounds checking is done in any function due to lazyness and/or performance reasons. Be careful**.

```

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

/* Enable cursor */
void fastcall cursor(unsigned char enable);

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

```
