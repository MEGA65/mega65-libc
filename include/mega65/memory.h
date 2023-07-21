#ifndef __MEGA65_MEMORY_H
#define __MEGA65_MEMORY_H

#include <stdint.h>

#ifdef __clang__
struct __attribute__((__packed__)) dmagic_dmalist {
#else
struct dmagic_dmalist {
#endif
    // Enhanced DMA options
    uint8_t option_0b;
    uint8_t option_80;
    uint8_t source_mb;
    uint8_t option_81;
    uint8_t dest_mb;
    uint8_t option_85;
    uint8_t dest_skip;
    uint8_t end_of_options;

    // F018B format DMA request
    uint8_t command;
    uint16_t count;
    uint16_t source_addr;
    uint8_t source_bank;
    uint16_t dest_addr;
    uint8_t dest_bank;
    uint8_t sub_cmd; // F018B subcmd
    uint16_t modulo;
};

#ifdef __clang__
extern volatile struct dmagic_dmalist dmalist;
extern volatile uint8_t dma_byte;
#else
extern struct dmagic_dmalist dmalist;
extern uint8_t dma_byte;
#endif

void mega65_io_enable(void);
uint8_t lpeek(uint32_t address);
uint8_t lpeek_debounced(uint32_t address);
uint8_t dma_peek(uint32_t address);
void lpoke(uint32_t address, uint8_t value);
void dma_poke(uint32_t address, uint8_t value);
void lcopy(
    uint32_t source_address, uint32_t destination_address, uint16_t count);
void lfill(uint32_t destination_address, uint8_t value, uint16_t count);
void lfill_skip(
    uint32_t destination_address, uint8_t value, uint16_t count, uint8_t skip);

#ifdef __clang__
#define POKE(X, Y) (*(volatile uint8_t*)(X)) = Y
#define POKE16(X, Y) (*(volatile uint16_t*)(X)) = Y
#define POKE32(X, Y) (*(volatile uint32_t*)(X)) = Y
#define PEEK(X) (*(volatile uint8_t*)(X))
#define PEEK16(X) (*(volatile uint16_t*)(X))
#define PEEK32(X) (*(volatile uint32_t*)(X))
#else
#define POKE(X, Y) (*(uint8_t*)(X)) = Y
#define POKE16(X, Y) (*(uint16_t*)(X)) = Y
#define POKE32(X, Y) (*(uint32_t*)(X)) = Y
#define PEEK(X) (*(uint8_t*)(X))
#define PEEK16(X) (*(uint16_t*)(X))
#define PEEK32(X) (*(uint32_t*)(X))
#endif

#endif // __MEGA65_MEMORY_H
