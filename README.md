# mega65-libc
Simple C library for the MEGA65

# Using the library

## With CC65

Simply include the .c and .s files from the cc65/src directory that you need 

## With KickC

KickC support will be coming soon (or sooner, if someone would like to port the routines ;)

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

## Text Console I/O

The `conio.h` file is projected to support all MEGA65 text features in the future.  

In this version, remember to set VIC-III extended character attributes register in $D031 if you want to use the attribute function calls.

Note that **no bounds checking is done in any function due to lazyness and/or performance reasons. Be careful**.


```

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
```