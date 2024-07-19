/*
 * fcio.c
 * MEGA65 full colour mode console and bitmap display support
 *
 * Copyright (C) 2019-21 - Stephan Kleinert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#define __fastcall__ // to silence stupid vscode warning

#ifdef __clang__
// C64 needs to be defined to include the <c64.h> header below
#define __C64__ 1
#endif

#include <mega65/fcio.h>
#include <mega65/memory.h>
#include <c64.h>
#include <cbm.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_FCI_BLOCKS 16
#define MAX_WINDOWS 8

#define TOPBORDER_PAL 0x58
#define BOTTOMBORDER_PAL 0x1e8

#define TOPBORDER_NTSC 0x27
#define BOTTOMBORDER_NTSC 0x1b7

char* fcbuf = (char*)0x0400; // general purpose buffer
// DMAlist is at 0x500
fciInfo** infoBlocks = (fciInfo**)0x0600; // pointers to fci info blocks
textwin* defaultWin = (textwin*)0x0700;
textwin* gCurrentWin;
fcioConf* gFcioConfig;
byte winCount = MAX_WINDOWS;

fcioConf stdConfig = {
    0x12000l,   // location of 16 bit screen
    0x14000l,   // reserved bitmap graphics graphics
    0x15000l,   // reserved system palette
    0x15300l,   // loaded palettes base
    0x40000l,   // loaded bitmaps base
    0xff81000l, // attribute/colour ram
};

#define VIC_BASE 0xD000UL

#define VIC2CTRL (*(unsigned char*)(0xd016))
#define VIC4CTRL (*(unsigned char*)(0xd054))
#define VIC3CTRL (*(unsigned char*)(0xd031))
#define LINESTEP_LO (*(unsigned char*)(0xd058))
#define LINESTEP_HI (*(unsigned char*)(0xd059))
#define CHRCOUNT (*(unsigned char*)(0xd05e))
#define HOTREG (*(unsigned char*)(0xd05d))

#define SCNPTR_0 (*(unsigned char*)(0xd060))
#define SCNPTR_1 (*(unsigned char*)(0xd061))
#define SCNPTR_2 (*(unsigned char*)(0xd062))
#define SCNPTR_3 (*(unsigned char*)(0xd063))

// special graphics characters
#define H_COLUMN_END 4
#define H_COLUMN_START 5
#define CURSOR_CHARACTER 0x5f

#define bitset(byte, nbit) ((byte) |= (1 << (nbit)))
#define bitclear(byte, nbit) ((byte) &= ~(1 << (nbit)))
#define bitflip(byte, nbit) ((byte) ^= (1 << (nbit)))
#define bitcheck(byte, nbit) ((byte) & (1 << (nbit)))

#define COLOUR_RAM_OFFSET (gFcioConfig->colourBase - 0xff80000l)

word gScreenSize;          // screen size (in characters)
byte gScreenColumns;       // number of screen columns (in characters)
byte gScreenRows;          // number of screen rows (in characters)
himemPtr nextFreeGraphMem; // next free graphics block in banks 4 & 5
himemPtr nextFreePalMem;   // location of next free palette memory block
byte infoBlockCount;       // number of info blocks
byte cgi;                  // universal loop var

int gTopBorder;
int gBottomBorder;

// flags
bool csrflag; // cursor on/off
bool autoCR;

unsigned int readExt(FILE* inFile, himemPtr addr, byte skipCBMAddressBytes)
{

    unsigned int readBytes;
    unsigned int overallRead;
    unsigned long insertPos;

    insertPos = addr;
    overallRead = 0;

    if (skipCBMAddressBytes) {
        fread(fcbuf, 1, 2, inFile);
    }

    do {
        readBytes = fread(fcbuf, 1, FCBUFSIZE, inFile);
        if (readBytes) {
            overallRead += readBytes;
            lcopy((long)fcbuf, insertPos, readBytes);
            insertPos += readBytes;
        }
    } while (readBytes);

    return overallRead;
}

unsigned int loadExt(char* filename, himemPtr addr, byte skipCBMAddressBytes)
{

    FILE* inFile;
    word readBytes;

    inFile = fopen(filename, "r");
    readBytes = readExt(inFile, addr, skipCBMAddressBytes);
    fclose(inFile);

    if (readBytes == 0) {
        fc_fatal("0 bytes from %s", filename);
    }

    return readBytes;
}

void fc_loadReservedBitmap(char* name)
{
    fc_loadFCI(name, gFcioConfig->reservedBitmapBase,
        gFcioConfig->reservedPaletteBase);
    fc_resetPalette();
}

void fc_init(
    byte h640, byte v400, fcioConf* config, byte rows, char* reservedBitmapFile)
{
    mega65_io_enable();

    puts("\n");       // cancel leftover quote mode from wrapper or whatever
    cbm_k_bsout(14);  // lowercase
    cbm_k_bsout(147); // clr

    gFcioConfig = config ? config : &stdConfig;

    if ((PEEK(53359U) & 128) == 0) {
        gTopBorder = TOPBORDER_PAL;
        gBottomBorder = BOTTOMBORDER_PAL;
    }
    else {
        gTopBorder = TOPBORDER_NTSC;
        gBottomBorder = BOTTOMBORDER_NTSC;
    }
    infoBlockCount = 0;
    for (cgi = 0; cgi < MAX_FCI_BLOCKS; ++cgi) {
        infoBlocks[cgi] = NULL;
    }
    fc_freeGraphAreas();
    fc_bgcolor(COLOR_BLACK);
    fc_bordercolor(COLOR_BLACK);

    fc_screenmode(h640, v400, rows);
    autoCR = true;

    if (reservedBitmapFile) {
        fc_loadReservedBitmap(reservedBitmapFile);
    }
    fc_textcolor(COLOR_GREEN);
}

static unsigned char swp;
unsigned char fc_nyblswap(unsigned char in) // oh why?!
{
    swp = in;
#ifdef __CC65__
    __asm__("lda %v", swp);
    __asm__("asl  a");
    __asm__("adc  #$80");
    __asm__("rol a");
    __asm__("asl a");
    __asm__("adc  #$80");
    __asm__("rol  a");
    __asm__("sta %v", swp);
#elif defined(__clang__)
#pragma GCC warning "LLVM assembly needs to be checked in fc_nyblswap()"
    asm volatile("ld%0\n"
                 "asl a\n"
                 "adc #$80\n"
                 "rol a\n"
                 "asl a\n"
                 "adc #$80\n"
                 "rol a\n"
                 "st%0"
                 : "+a"(swp));
#else
#pragma GCC warning "fc_nyblswap() is not implemented for this compiler"
#endif
    return swp;
}

void fc_flash(byte f)
{
    if (f) {
        bitset(gCurrentWin->extAttributes, 4);
    }
    else {
        bitclear(gCurrentWin->extAttributes, 4);
    }
}

void fc_revers(byte f)
{
    if (f) {
        bitset(gCurrentWin->extAttributes, 5);
    }
    else {
        bitclear(gCurrentWin->extAttributes, 5);
    }
}

void fc_bold(byte f)
{
    if (f) {
        bitset(gCurrentWin->extAttributes, 6);
    }
    else {
        bitclear(gCurrentWin->extAttributes, 6);
    }
}

void fc_underline(byte f)
{
    if (f) {
        bitset(gCurrentWin->extAttributes, 7);
    }
    else {
        bitclear(gCurrentWin->extAttributes, 7);
    }
}

void fc_resetPalette(void)
{
    mega65_io_enable();
    fc_loadPalette(gFcioConfig->reservedPaletteBase, 255, false);
}

void fc_fatal(const char* format, ...)
{
    char buf[160];
    va_list args;

    mega65_io_enable();
    va_start(args, format);
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
    vsnprintf(buf, 159, format, args);
#pragma clang diagnostic pop
#else
    vsprintf(buf, format, args);
#endif
    va_end(args);
    fc_go8bit();
    fc_bordercolor(2);
    fc_bgcolor(0);
    puts("## fatal error ##");
    puts(buf);
    while (1)
        ;
}

void fc_freeGraphAreas(void)
{
    for (cgi = 0; cgi < infoBlockCount; ++cgi) {
        if (infoBlocks[cgi] != NULL) {
            free(infoBlocks[cgi]);
            infoBlocks[cgi] = NULL;
        }
    }
    nextFreeGraphMem = gFcioConfig->dynamicBitmapBase;
    nextFreePalMem = gFcioConfig->dynamicPaletteBase;
    infoBlockCount = 0;
}

/*
very simple graphics memory allocation scheme:
try to find space in 128K beginning at GRAPHBASE, without
crossing bank boundaries. If everything's full, bail out.
*/

himemPtr fc_allocGraphMem(word size)
{
    himemPtr adr = nextFreeGraphMem;

    if (nextFreeGraphMem + size < gFcioConfig->dynamicBitmapBase + 0x10000) {
        nextFreeGraphMem += size;
        return adr;
    }
    if (nextFreeGraphMem < gFcioConfig->dynamicBitmapBase + 0x10000) {
        nextFreeGraphMem = gFcioConfig->dynamicBitmapBase + 0x10000;
        adr = nextFreeGraphMem;
    }
    if (nextFreeGraphMem + size < gFcioConfig->dynamicBitmapBase + 0x20000) {
        nextFreeGraphMem += size;
        return adr;
    }
    return 0;
}

himemPtr fc_allocPalMem(word size)
{
    himemPtr adr = nextFreePalMem;
    if (nextFreePalMem < 0x1e000) // TODO: don't hardcode boundaries
    {
        nextFreePalMem += size;
        return adr;
    }
    return 0;
}

char asciiToPetscii(byte c)
{
    // TODO: could be made much faster with translation table
    if (c == '_') {
        return 100;
    }
    if (c >= 64 && c <= 95) {
        return c - 64;
    }
    if (c >= 96 && c < 192) {
        return c - 32;
    }
    if (c >= 192) {
        return c - 128;
    }
    return c;
}

#ifdef __clang__
void adjustBorders(byte extraRows, __attribute__((unused)) byte extraColumns)
#else
void adjustBorders(byte extraRows, byte extraColumns)
#endif
{
    byte extraTopRows = 0;
    byte extraBottomRows = 0;
    int newBottomBorder;

    extraColumns++; // TODO: support for extra columns
    extraBottomRows = extraRows / 2;
    extraTopRows = extraRows - extraBottomRows;

    // top border position
    POKE(53320u, (uint8_t)(gTopBorder - (extraTopRows * 8)));
    // top text position
    POKE(53326u, (uint8_t)(gTopBorder - (extraTopRows * 8)));

    newBottomBorder = gBottomBorder + (extraBottomRows * 8);

    POKE(53322U, (uint8_t)(newBottomBorder % 256));
    POKE(53323U, (uint8_t)(newBottomBorder / 256));

    POKE(53371u, gScreenRows);
}

void fc_screenmode(byte h640, byte v400, byte rows)
{
    int extraRows = 0;

    mega65_io_enable();
    if (rows == 0) {
        gScreenRows = v400 ? 50 : 25;
    }
    else {
        gScreenRows = rows;
    }

    HOTREG |= 0x80;   // enable HOTREG if previously disabled
    VIC4CTRL |= 0x04; // enable full colour for characters with high byte set
    VIC4CTRL |= 0x01; // enable 16 bit characters

    if (h640) {
        VIC3CTRL |= 0x80; // enable H640
        VIC2CTRL |= 0x01; // shift one pixel to the right (VIC-III bug)
        gScreenColumns = 80;
    }
    else {
        VIC3CTRL &= 0x7f; // disable H640
        gScreenColumns = 40;
    }

    if (v400) {
        VIC3CTRL |= 0x08;
        extraRows = gScreenRows - 50;
    }
    else {
        VIC3CTRL &= 0xf7;
        extraRows = (gScreenRows - 25) * 2;
    }

    gScreenSize = gScreenRows * gScreenColumns;
    lfill_skip(gFcioConfig->screenBase, 32, gScreenSize, 2);
    lfill(gFcioConfig->colourBase, 0, gScreenSize * 2);

    HOTREG &= 127; // disable hotreg

    if (extraRows > 0) {
        adjustBorders((byte)extraRows, 0);
    }

    // move colour RAM because of stupid CBDOS himem usage
    POKE(53348u, COLOUR_RAM_OFFSET & 0xff);
    POKE(53349u, (COLOUR_RAM_OFFSET >> 8) & 0xff);

    CHRCOUNT = gScreenColumns;
    LINESTEP_LO
        = gScreenColumns * 2; // *2 to have 2 screen bytes == 1 character
    LINESTEP_HI = 0;

    SCNPTR_0 = gFcioConfig->screenBase & 0xff; // screen to 0x12000
    SCNPTR_1 = (gFcioConfig->screenBase >> 8) & 0xff;
    SCNPTR_2 = (gFcioConfig->screenBase >> 16) & 0xff;
    SCNPTR_3 &= 0xF0 | ((gFcioConfig->screenBase) << 24 & 0xff);

    fc_resetwin();
    fc_clrscr();
}

void fc_go8bit(void)
{
    mega65_io_enable();
    VIC3CTRL = 96;    // quit bitplane mode if set
    POKE(53297L, 96); // quit bitplane mode
    SCNPTR_0 = 0x00;  // screen back to 0x800
    SCNPTR_1 = 0x08;
    SCNPTR_2 = 0x00;
    SCNPTR_3 &= 0xF0;
    VIC4CTRL &= 0xFA; // clear fchi and 16bit chars
    CHRCOUNT = 40;
    LINESTEP_LO = 40;
    LINESTEP_HI = 0;
    HOTREG |= 0x80;   // enable hotreg
    VIC3CTRL &= 0x7f; // disable H640
    VIC3CTRL &= 0xf7; // disable V400
    cbm_k_bsout(14);  // lowercase charset
    fc_setPalette(0, 0, 0, 0);
    fc_setPalette(1, 255, 255, 255);
    fc_setPalette(2, 255, 0, 0);
}

void fc_plotExtChar(byte x, byte y, byte c)
{
    word charIdx;
    uint32_t adr;
    charIdx = (word)((gFcioConfig->reservedBitmapBase / 64) + c);
    adr = (x * 2) + (y * gScreenColumns * 2);
    lpoke(gFcioConfig->screenBase + adr, (uint8_t)(charIdx % 256));
    lpoke(gFcioConfig->screenBase + adr + 1, (uint8_t)(charIdx / 256));
}

void fc_addGraphicsRect(
    byte x0, byte y0, byte width, byte height, himemPtr bitmapData)
{
    static byte x, y;
    uint32_t adr;
    word currentCharIdx;

    currentCharIdx = (word)(bitmapData / 64);

    for (y = y0; y < y0 + height; ++y) {
        for (x = x0; x < x0 + width; ++x) {
            adr = gFcioConfig->screenBase + (x * 2) + (y * gScreenColumns * 2);
            lpoke(adr + 1,
                currentCharIdx / 256); // set highbyte first to avoid blinking
            lpoke(adr, (uint8_t)(currentCharIdx
                                 % 256)); // while setting up the screeen
            currentCharIdx++;
        }
    }
}

fciInfo* fc_loadFCI(char* filename, himemPtr address, himemPtr paletteAddress)
{
    static byte numColumns, numRows, lastColourIndex;
    static byte fciOptions;
    static byte reservedSysPalette;

    FILE* fcifile;
    byte* palette;
    word palsize;
    word imgsize;
    word bytesRead;
    himemPtr bitmampAdr;
    himemPtr palAdr;
    fciInfo* info;

    info = NULL;

    if (!address) {
        info = (fciInfo*)malloc(sizeof(fciInfo));
        infoBlocks[infoBlockCount++] = info;
    }

    fcifile = fopen(filename, "rb");

    if (!fcifile) {
        fc_fatal("fci not found %s", filename);
    }
    fread(fcbuf, 1, 9, fcifile);

    numRows = fcbuf[5];
    numColumns = fcbuf[6];
    fciOptions = fcbuf[7];
    lastColourIndex = fcbuf[8];
    reservedSysPalette = fciOptions & 2;

    palsize = (lastColourIndex + 1) * 3;
    palette = (byte*)malloc(palsize);
    fread(palette, 3, lastColourIndex + 1, fcifile);

    if (!paletteAddress) {
        palAdr = fc_allocPalMem(palsize);
        if (palAdr == 0) {
            fc_fatal("no room for palette");
        }
    }
    else {
        palAdr = paletteAddress;
    }
    lcopy((long)palette, palAdr, palsize);
    free(palette);
    imgsize = numColumns * numRows * 64;

    fread(fcbuf, 1, 3, fcifile);
    if (0 != memcmp(fcbuf, "img", 3)) {
        fc_fatal("image marker not found in %s", filename);
    }

    if (!address) {
        bitmampAdr = fc_allocGraphMem(imgsize);
        if (bitmampAdr == 0) {
            fc_fatal("no memory for %s", filename);
        }
    }
    else {
        bitmampAdr = address;
    }

    bytesRead = readExt(fcifile, bitmampAdr, false);
    fclose(fcifile);

    if (info != NULL) {
        info->columns = numColumns;
        info->rows = numRows;
        info->size = bytesRead;
        info->baseAdr = bitmampAdr;
        info->paletteAdr = palAdr;
        info->paletteSize = lastColourIndex;
        info->reservedSysPalette = reservedSysPalette;
    }

    mega65_io_enable(); // kernal has the disgusting habit of resetting vic
                        // personality

    return info;
}

void fc_zeroPalette(byte reservedSysPalette)
{
    byte start;

    mega65_io_enable();
    start = reservedSysPalette ? 16 : 0;
    for (cgi = start; cgi < 255; ++cgi) {
        POKE(0xd100u + cgi, 0); // palette[colAdr];
        POKE(0xd200u + cgi, 0); // palette[colAdr + 1];
        POKE(0xd300u + cgi, 0); // palette[colAdr + 2];
    }
}

void fc_loadPalette(himemPtr adr, byte size, byte reservedSysPalette)
{
    himemPtr colAdr;
    byte start;
    int i;
    start = reservedSysPalette ? 16 : 0;

    for (i = start; i <= size; ++i) {
        colAdr = (himemPtr)(i * 3);
        // fc_printf("\n%d (%lx) : %2x %2x %2x", i, adr + colAdr, lpeek(adr +
        // colAdr), lpeek(adr + colAdr + 1), lpeek(adr + colAdr + 2));
        // fc_getkey();
        // palette[colAdr];
        POKE(0xd100u + (unsigned int)i, fc_nyblswap(lpeek(adr + colAdr)));
        // palette[colAdr + 1];
        POKE(0xd200u + (unsigned int)i, fc_nyblswap(lpeek(adr + colAdr + 1)));
        // palette[colAdr + 2];
        POKE(0xd300u + (unsigned int)i, fc_nyblswap(lpeek(adr + colAdr + 2)));
    }
}

void fc_fadePalette(
    himemPtr adr, byte size, byte reservedSysPalette, byte steps, bool fadeOut)
{
    byte i;
    byte startReg;
    byte* destPalette;
    byte* entry;

    byte start, end, step;

    startReg = reservedSysPalette ? 16 : 0;
    destPalette = malloc(size * 3);
    lcopy(adr, (uint32_t)destPalette, size * 3);

    if (fadeOut) {
        start = steps;
        end = 0;
        step = (byte)(-1);
    }
    else {
        start = 0;
        end = steps;
        step = 1;
    }

    for (i = start; i != end; i += step) {
        entry = destPalette + (startReg * 3);
        for (cgi = startReg; cgi < size; ++cgi, entry += 3) {
            POKE(0xd100u + cgi, fc_nyblswap((*(entry)*i) / steps));
            POKE(0xd200u + cgi, fc_nyblswap((*(entry + 1) * i) / steps));
            POKE(0xd300u + cgi, fc_nyblswap((*(entry + 2) * i) / steps));
        }
    }
    free(destPalette);
}

void fc_fadeFCI(fciInfo* info, byte x0, byte y0, byte steps)
{
    fc_zeroPalette(info->reservedSysPalette);
    fc_addGraphicsRect(x0, y0, info->columns, info->rows, info->baseAdr);
    fc_fadePalette(info->paletteAdr, info->paletteSize,
        info->reservedSysPalette, steps, false);
}

void fc_displayFCI(fciInfo* info, byte x0, byte y0, bool setPalette)
{
    fc_addGraphicsRect(x0, y0, info->columns, info->rows, info->baseAdr);
    if (setPalette) {
        fc_loadFCIPalette(info);
    }
}

void fc_loadFCIPalette(fciInfo* info)
{
    fc_loadPalette(
        info->paletteAdr, info->paletteSize, info->reservedSysPalette);
}

fciInfo* fc_displayFCIFile(char* filename, byte x0, byte y0)
{
    fciInfo* info;
    info = fc_loadFCI(filename, 0, 0);
    fc_displayFCI(info, x0, y0, true);
    return info;
}

void fc_scrollUp(void)
{
    static byte y;
    uint32_t bas0, bas1;
    for (y = gCurrentWin->y0; y < gCurrentWin->y0 + gCurrentWin->height - 1;
         y++) {
        bas0 = gFcioConfig->screenBase
             + (himemPtr)(gCurrentWin->x0 * 2 + (y * gScreenColumns * 2));
        bas1 = gFcioConfig->screenBase
             + (himemPtr)(gCurrentWin->x0 * 2 + ((y + 1) * gScreenColumns * 2));
        lcopy((uint32_t)bas1, (uint32_t)bas0, gCurrentWin->width * 2);
        bas0 = gFcioConfig->colourBase
             + (himemPtr)(gCurrentWin->x0 * 2 + (y * gScreenColumns * 2));
        bas1 = gFcioConfig->colourBase
             + (himemPtr)(gCurrentWin->x0 * 2 + ((y + 1) * gScreenColumns * 2));
        lcopy(bas1, bas0, gCurrentWin->width * 2);
    }
    fc_line(0, gCurrentWin->height - 1, gCurrentWin->width, 32,
        gCurrentWin->textcolor);
}

void fc_scrollDown(void)
{
    signed char y;
    uint32_t bas0, bas1;
    for (y = gCurrentWin->y0 + gCurrentWin->height - 2; y >= gCurrentWin->y0;
         y--) {
        bas0 = gFcioConfig->screenBase
             + (himemPtr)(gCurrentWin->x0 * 2 + (y * gScreenColumns * 2));
        bas1 = gFcioConfig->screenBase
             + (himemPtr)(gCurrentWin->x0 * 2 + ((y + 1) * gScreenColumns * 2));
        lcopy(bas0, bas1, gCurrentWin->width * 2);

        bas0 = gFcioConfig->colourBase
             + (himemPtr)(gCurrentWin->x0 * 2 + (y * gScreenColumns * 2));
        bas1 = gFcioConfig->colourBase
             + (himemPtr)(gCurrentWin->x0 * 2 + ((y + 1) * gScreenColumns * 2));
        lcopy(bas0, bas1, gCurrentWin->width * 2);
    }

    fc_line(0, 0, gCurrentWin->width, 32, gCurrentWin->textcolor);
}

void cr(void)
{
    gCurrentWin->xc = 0;
    gCurrentWin->yc++;
    if (gCurrentWin->yc > gCurrentWin->height - 1) {
        fc_scrollUp();
        gCurrentWin->yc = gCurrentWin->height - 1;
    }
}

void fc_plotPetsciiChar(byte x, byte y, byte c, byte color, byte exAttr)
{
    word adrOffset;
    adrOffset = (x * 2) + (y * 2 * gScreenColumns);
    lpoke(gFcioConfig->screenBase + adrOffset, c);
    lpoke(gFcioConfig->screenBase + adrOffset + 1, 0);
    lpoke(gFcioConfig->colourBase + adrOffset + 1, color | exAttr);
    lpoke(gFcioConfig->colourBase + adrOffset, 0);
}

byte fc_wherex(void)
{
    return gCurrentWin->xc;
}

byte fc_wherey(void)
{
    return gCurrentWin->yc;
}

void fc_setAutoCR(bool a)
{
    autoCR = a;
}

void fc_putc(char c)
{
    static char out;
    if (!c) {
        return;
    }
    if (c == '\n') {
        cr();
        return;
    }

    if (gCurrentWin->xc >= gCurrentWin->width) {
        return;
    }

    out = asciiToPetscii(c);

    fc_plotPetsciiChar(gCurrentWin->xc + gCurrentWin->x0,
        gCurrentWin->yc + gCurrentWin->y0, out, gCurrentWin->textcolor,
        gCurrentWin->extAttributes);
    gCurrentWin->xc++;

    if (autoCR) {
        if (gCurrentWin->xc >= gCurrentWin->width) {
            gCurrentWin->yc++;
            gCurrentWin->xc = 0;
            if (gCurrentWin->yc >= gCurrentWin->height) {
                gCurrentWin->yc = gCurrentWin->height - 1;
                fc_scrollUp();
            }
        }
    }

    if (csrflag) {
        fc_plotPetsciiChar(gCurrentWin->xc + gCurrentWin->x0,
            gCurrentWin->yc + gCurrentWin->y0, CURSOR_CHARACTER,
            gCurrentWin->textcolor, 16);
    }
}

void _debug_fc_puts(const char* s)
{
    const char* current = s;
    while (*current) {
        fc_putc(*current++);
    }
}

void fc_puts(const char* s)
{
    /* #ifdef DEBUG
    char out[16];
#endif */
    const char* current = s;
    while (*current) {
        fc_putc(*current++);
    }
    /* #ifdef DEBUG
    gCurrentWin->x0 = 0;
    gCurrentWin->y0 = 0;
    gCurrentWin->xc = gScreenColumns - 4;
    gCurrentWin->yc = 0;
    sprintf(out, "%x", _heapmaxavail());
    _debug_fc_puts(out);
#endif */
}

void fc_putsxy(byte x, byte y, char* s)
{
    fc_gotoxy(x, y);
    fc_puts(s);
}

void fc_putcxy(byte x, byte y, char c)
{
    fc_gotoxy(x, y);
    fc_putc(c);
}

void fc_cursor(byte onoff)
{
    csrflag = onoff;

    fc_plotPetsciiChar(gCurrentWin->xc + gCurrentWin->x0,
        gCurrentWin->yc + gCurrentWin->y0, (csrflag ? CURSOR_CHARACTER : 32),
        gCurrentWin->textcolor, (csrflag ? 16 : 0));
}

void fc_textcolor(byte c)
{
    gCurrentWin->textcolor = c;
}

void fc_gotoxy(byte x, byte y)
{
    gCurrentWin->xc = x;
    gCurrentWin->yc = y;
}

void fc_printf(const char* format, ...)
{
    char buf[160];
    va_list args;
    va_start(args, format);
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
    vsnprintf(buf, 159, format, args);
#pragma clang diagnostic pop
#else
    vsprintf(buf, format, args);
#endif
    va_end(args);
    fc_puts(buf);
}

void fc_clrscr(void)
{
    fc_block(0, 0, gCurrentWin->width, gCurrentWin->height, 32,
        gCurrentWin->textcolor);
    fc_gotoxy(0, 0);
}

void fc_resetwin(void)
{
    gCurrentWin = defaultWin;
    gCurrentWin->x0 = 0;
    gCurrentWin->y0 = 0;
    gCurrentWin->width = gScreenColumns;
    gCurrentWin->height = gScreenRows;
    gCurrentWin->xc = 0;
    gCurrentWin->yc = 0;
    gCurrentWin->extAttributes = 0;
    gCurrentWin->textcolor = 5;
}

void fc_setwin(textwin* aWin)
{
    gCurrentWin = aWin;
}

textwin* fc_makeWin(byte x0, byte y0, byte width, byte height)
{
    textwin* aWin;
    aWin = malloc(sizeof(textwin));
    aWin->x0 = x0;
    aWin->y0 = y0;
    aWin->width = width;
    aWin->height = height;
    aWin->xc = 0;
    aWin->yc = 0;
    aWin->extAttributes = 0;
    aWin->textcolor = 5; // because I like green. your mileage may vary.
    return aWin;
}

byte fc_kbhit(void)
{
    return PEEK(0xD610U);
}

byte fc_cgetc(void)
{
    byte k;
    while ((k = PEEK(0xD610U)) == 0)
        ;
    POKE(0xD610U, 0);
    return k;
}

void fc_emptyBuffer(void)
{
    while (fc_kbhit()) {
        fc_cgetc();
    }
}

char fc_getkey(void)
{
    fc_emptyBuffer();
    return fc_cgetc();
}

int fc_getnum(byte maxlen)
{
    int res;
    char* inptr;
    inptr = fc_input(maxlen);
    res = atoi(inptr);
    free(inptr);
    return res;
}

char* fc_input(byte maxlen)
{
    static byte len, ct;
    char current;
    char* ret;
    len = 0;
    ct = csrflag;
    fc_cursor(1);
    do {
        current = fc_cgetc();
        if (current != '\n') {
            if (current >= 32) {
                if (len < maxlen) {
                    // fix upper/lowercase
                    if (current >= 97) {
                        current -= 32;
                    }
                    else if (current >= 65) {
                        current += 32;
                    }
                    fcbuf[len] = current;
                    fcbuf[len + 1] = 0;
                    fc_putc(current);
                    ++len;
                }
            }
            else if (current == 20) {
                // del pressed
                if (len > 0) {
                    fc_cursor(0);
                    fc_gotoxy(gCurrentWin->xc - 1, gCurrentWin->yc);
                    fc_putc(' ');
                    fc_gotoxy(gCurrentWin->xc - 1, gCurrentWin->yc);
                    fc_cursor(1);
                    --len;
                    fcbuf[len] = 0;
                }
            }
        }
    } while (current != '\n');
    ret = (char*)malloc(++len);
    if (len == 1) {
        *ret = 0;
    }
    else {
        strncpy(ret, fcbuf, len);
    }
    fc_cursor(ct);
    return ret;
}

void fc_line(byte x, byte y, byte width, byte character, byte col)
{
    word bas;

    bas = (gCurrentWin->x0 + x) * 2
        + ((gCurrentWin->y0 + y) * gScreenColumns * 2);

    // use DMAgic to fill FCM screens with skip byte... PGS, I love you!
    lfill_skip(gFcioConfig->screenBase + bas, character, width, 2);
    lfill_skip(gFcioConfig->screenBase + bas + 1, 0, width, 2);
    lfill_skip(gFcioConfig->colourBase + bas, 0, width, 2);
    lfill_skip(gFcioConfig->colourBase + bas + 1, col, width, 2);

    return;
}

void fc_block(
    byte x0, byte y0, byte width, byte height, byte character, byte col)
{
    static byte y;
    for (y = 0; y < height; ++y) {
        fc_line(x0, y0 + y, width, character, col);
    }
}

void fc_center(byte x, byte y, byte width, char* text)
{
    static byte l;
    l = (byte)strlen(text);
    if (l >= width - 2) {
        fc_gotoxy(x, y);
        fc_puts(text);
        return;
    }
    fc_gotoxy((unsigned char)(-1 + x + width / 2 - l / 2), y);
    fc_puts(text);
}

void fc_setPalette(int num, byte red, byte green, byte blue)
{
    POKE(0xd100U + (unsigned int)num, fc_nyblswap(red));
    POKE(0xd200U + (unsigned int)num, fc_nyblswap(green));
    POKE(0xd300U + (unsigned int)num, fc_nyblswap(blue));
}

char fc_getkeyP(byte x, byte y, const char* prompt)
{
    fc_emptyBuffer();
    fc_gotoxy(x, y);
    fc_textcolor(COLOR_WHITE);
    fc_puts(prompt);
    return fc_cgetc();
}

void fc_hlinexy(byte x, byte y, byte width, byte lineChar)
{
    for (cgi = x; cgi < x + width; cgi++) {
        fc_plotExtChar(
            gCurrentWin->x0 + x + cgi, gCurrentWin->y0 + y, lineChar);
    }
}

void fc_vlinexy(byte x, byte y, byte height, byte lineChar)
{
    for (cgi = y; cgi < y + height; cgi++) {
        fc_plotExtChar(
            gCurrentWin->x0 + x, gCurrentWin->y0 + y + cgi, lineChar);
    }
}
