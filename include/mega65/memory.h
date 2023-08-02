#ifndef __MEGA65_MEMORY_H
#define __MEGA65_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define DMA_COPY_CMD 0x00;
#define DMA_FILL_CMD 0x03;

/**
 * @brief DMA list structure
 */
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

/**
 * @brief Enable Mega65 mode
 *
 * This does nothing unless compiling for a C64 target, i.e.
 * if the `__C64__` macro is defined.
 */
void mega65_io_enable(void);

/**
 * @brief Peek a byte from the given address
 * @param address 28-bit address
 * @return uint8_t with the value at the given address
 */
uint8_t lpeek(uint32_t address);
uint8_t lpeek_debounced(uint32_t address);

/**
 * @brief Peek a byte from the given address using DMA copy
 * @param address 28-bit address
 * @return Single byte from the given address
 */
uint8_t dma_peek(uint32_t address);

/**
 * @brief Poke a byte to the given address
 * @param address 28-bit address
 * @param value Single byte to write to the given address
 */
void lpoke(uint32_t address, uint8_t value);

/**
 * @brief Poke a byte to the given address using DMA copy
 * @param address 28-bit address
 * @param value Single byte to write to the given address
 */
void dma_poke(uint32_t address, uint8_t value);

/**
 * @brief Copy a block of memory using DMA
 * @param source_address 28-bit address to copy from
 * @param destination_address 28-bit address to copy to
 * @param count Number of bytes to copy
 */
void lcopy(uint32_t source_address, uint32_t destination_address, size_t count);

/**
 * @brief Fill a block of memory with a single byte using DMA
 * @param destination_address Start address (28-bit)
 * @param value Fill value
 * @param count Number of bytes to fill
 */
void lfill(uint32_t destination_address, uint8_t value, size_t count);

/**
 * @brief Fill a block of memory with a single byte using DMA with a step
 * @param destination_address Start address (28-bit)
 * @param value Fill value
 * @param count Number of bytes to fill
 * @param skip Skip every n bytes
 */
void lfill_skip(
    uint32_t destination_address, uint8_t value, size_t count, uint8_t skip);

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
