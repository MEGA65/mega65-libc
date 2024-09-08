/**
 * @brief Random number functions
 * @file random.h
 *
 *  All MEGA65 models have a thermal noise register from the FPGA that
 *  we can use to get some entropy from.
 *  We can then use the hardware multiplier to get random values within
 *  arbitrary range without needing to resort to a modulus operator.
 */
#ifndef __MEGA65_RANDOM_H
#define __MEGA65_RANDOM_H

#include <stdint.h>

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/**
 * @brief 32-bit random number using MEGA65 hardware entropy
 * @param range Maximum value in range
 */
uint32_t random32(uint32_t range);

/**
 * @brief 16-bit random number using MEGA65 hardware entropy
 * @param range Maximum value in range
 */
uint16_t random16(uint16_t range);

/**
 * @brief 8-bit random number using MEGA65 hardware entropy
 * @param range Maximum value in range
 */
uint8_t random8(uint8_t range);

/**
 * @brief Seed the Xorshift algorithm
 * @param seed Seed value. If zero, then random value is used.
 */
void srand(uint32_t seed);

/**
 * @brief 8-bit random number using the Xorshift algorithm
 * @param range Maximum value in range
 */
uint8_t rand8(uint8_t range);

/**
 * @brief 16-bit random number using the Xorshift algorithm
 * @param range Maximum value in range
 */
uint16_t rand16(uint16_t range);

/**
 * @brief 32-bit random number using the Xorshift algorithm
 * @param range Maximum value in range
 */
uint32_t rand32(uint32_t range);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_RANDOM_H
