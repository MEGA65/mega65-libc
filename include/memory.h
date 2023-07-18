#ifndef __MEGA65_MEMORY_H
#define __MEGA65_MEMORY_H

#include <stdint.h>

struct dmagic_dmalist {
    // Enhanced DMA options
    unsigned char option_0b;
    unsigned char option_80;
    unsigned char source_mb;
    unsigned char option_81;
    unsigned char dest_mb;
    unsigned char option_85;
    unsigned char dest_skip;
    unsigned char end_of_options;

    // F018B format DMA request
    unsigned char command;
    unsigned int count;
    unsigned int source_addr;
    unsigned char source_bank;
    unsigned int dest_addr;
    unsigned char dest_bank;
    unsigned char sub_cmd; // F018B subcmd
    unsigned int modulo;
};

#ifdef __clang__
extern volatile struct dmagic_dmalist dmalist;
extern volatile unsigned char dma_byte;
#else
extern struct dmagic_dmalist dmalist;
extern unsigned char dma_byte;
#endif

void mega65_io_enable(void);
unsigned char lpeek(long address);
unsigned char lpeek_debounced(long address);
unsigned char dma_peek(long address);
void lpoke(long address, unsigned char value);
void dma_poke(long address, unsigned char value);
void lcopy(long source_address, long destination_address, unsigned int count);
void lfill(long destination_address, unsigned char value, unsigned int count);
void lfill_skip(long destination_address, unsigned char value,
    unsigned int count, unsigned char skip);
#if defined(__CC65__)
#define POKE(X, Y) (*(uint8_t*)(X)) = Y
#define POKE16(X, Y) (*(uint16_t*)(X)) = Y
#define POKE32(X, Y) (*(uint32_t*)(X)) = Y
#define PEEK(X) (*(uint8_t*)(X))
#define PEEK16(X) (*(uint16_t*)(X))
#define PEEK32(X) (*(uint32_t*)(X))
#elif defined(__clang__)
#define POKE(X, Y) (*(volatile uint8_t*)(X)) = Y
#define POKE16(X, Y) (*(volatile uint16_t*)(X)) = Y
#define POKE32(X, Y) (*(volatile uint32_t*)(X)) = Y
#define PEEK(X) (*(volatile uint8_t*)(X))
#define PEEK16(X) (*(volatile uint16_t*)(X))
#define PEEK32(X) (*(volatile uint32_t*)(X))
#else
#define POKE(X, Y)
#define POKE16(X, Y)
#define POKE32(X, Y)
#define PEEK(X)
#define PEEK16(X)
#define PEEK32(X)
#endif

#endif
