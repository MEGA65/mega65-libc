/**
 * @file memory.h
 * @brief Memory access functions
 *
 * This contains simple POKE and PEEK functions, as well as DMA functions for
 * copying and filling memory.
 */
#ifndef __MEGA65_MEMORY_H
#define __MEGA65_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define DMA_COPY_CMD 0x00; //!< DMA copy command
#define DMA_MIX_CMD 0x01;  //!< DMA mix command (unimplemented)
#define DMA_SWAP_CMD 0x02; //!< DMA swap command (unimplemented)
#define DMA_FILL_CMD 0x03; //!< DMA fill command

#define DMA_LINEAR_ADDR 0x00; //!< DMA linear (normal) addressing mode
#define DMA_MODULO_ADDR 0x01; //!< DMA modulo (rectangular) addressing mode
#define DMA_HOLD_ADDR 0x02;   //!< DMA hold (constant address) addressing mode
#define DMA_XYMOD_ADDR                                                         \
    0x03; //!< DMA XY MOD (bitmap rectangular) addressing mode (unimplemented)

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/**
 * @brief DMA list structure
 */
struct dmagic_dmalist {
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
    uint8_t command; //!< Command (LSB), e.g. DMA_COPY_CMD, DMA_FILL_CMD, etc.
    uint16_t count;  //!< Number of bytes to copy
    uint16_t source_addr; //!< Source address
    uint8_t source_bank;  //!< Source bank and flags
    uint16_t dest_addr;   //!< Destination address
    uint8_t dest_bank;    //!< Destination bank and flags
    uint8_t sub_cmd;      //!< Command (MSB) or F018B subcmd
    uint16_t modulo;      //!< Modulo mode
};

#ifdef __clang__
extern volatile struct dmagic_dmalist dmalist;
extern volatile uint8_t dma_byte;
#else
extern struct dmagic_dmalist dmalist;
extern uint8_t dma_byte;
#endif

/**
 * @brief Enable MEGA65 features when in C64 mode
 *
 * Sets the speed to 40 MHz, enables the IO and VIC-IV areas.
 */
void mega65_io_enable(void);

/**
 * @brief Peek a byte from the given address
 * @param address 28-bit address
 * @return uint8_t with the value at the given address
 */
#ifdef __clang__
__attribute__((leaf))
#endif
uint8_t
lpeek(uint32_t address);

#ifdef __llvm__
/**
 * @brief Inlined version of `lpeek()`
 * @param ADDRESS 28-bit address; must be a compile-time constant.
 * @returns Value at address
 * @warning This function is experimental and may change/be removed in future
 * versions
 */
inline uint8_t lpeek_i(const uint32_t ADDRESS)
{
    // Helper to (dis)assemble 32-bit int; optimizes out fully
    const union {
        uint32_t value;
        uint8_t byte[4];
    } adr = { ADDRESS };

    uint8_t value;

    __attribute__((leaf)) __asm__ volatile(
        "ldz #%4        \n" // complete Q=AXYZ register
        "stq __rc2      \n" // Q -> rc2-5
        "ldz #0         \n"
        "lda [__rc2], z \n"
        : "=a"(value)
        : "a"(adr.byte[0]), "x"(adr.byte[1]), "y"(adr.byte[2]), "i"(adr.byte[3])
        : "rc2", "rc3", "rc4", "rc5", "p");

    return value;
}
#endif

uint8_t lpeek_debounced(uint32_t address);

/**
 * @brief Peek a byte from the given address using DMA copy
 * @param address 28-bit address
 * @return Single byte from the given address
 * @note This is slower than lpeek()
 */
uint8_t dma_peek(uint32_t address);

/**
 * @brief Poke a byte to the given address
 * @param address 28-bit address
 * @param value Single byte to write to the given address
 */
#ifdef __clang__
__attribute__((leaf))
#endif
void lpoke(uint32_t address, uint8_t value);

/**
 * @brief Poke a byte to the given address using DMA copy
 * @param address 28-bit address
 * @param value Single byte to write to the given address
 * @note This is slower than lpoke()
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

/// Poke a byte to the given address
#define POKE(X, Y) (*(volatile uint8_t*)(X)) = Y
/// Poke two bytes to the given address
#define POKE16(X, Y) (*(volatile uint16_t*)(X)) = Y
/// Poke four bytes to the given address
#define POKE32(X, Y) (*(volatile uint32_t*)(X)) = Y
/// Peek a byte from the given address
#define PEEK(X) (*(volatile uint8_t*)(X))
/// Peek two bytes from the given address
#define PEEK16(X) (*(volatile uint16_t*)(X))
/// Peek four bytes from the given address
#define PEEK32(X) (*(volatile uint32_t*)(X))

#ifdef __clang__
/// Peek eight bytes from the given address
#define PEEK64(X) (*(volatile uint64_t*)(X))
#endif

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_MEMORY_H
