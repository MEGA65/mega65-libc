/*
MIT License

Copyright (c) 2023 The MEGA65 Community

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @file math.h
 * @brief Math functions for the MEGA65
 *
 * This provides multiplication and division functions
 * using the MEGA65 CPU math acceleration registers.
 */

#ifndef __MEGA65_MATH_H
#define __MEGA65_MATH_H

#include <mega65/memory.h>
#include <stdint.h>

#define DIVBUSY 0xD70F
#define DIVOUT 0xD768
#define DIVOUT_WHOLE 0xD768 + 4
#define MULTINA 0xD770
#define MULTINB 0xD774
#define MULTOUT 0xD778

/**
 * @brief Multiply two 32-bit unsigned integers and return 32-bit value
 * @param a First 32-bit value
 * @param b Second 32-bit value
 * @return uint32_t Truncated 32-bit product of a and b
 */
uint32_t umul32(const uint32_t a, const uint32_t b);

/**
 * @brief Divide two 32-bit unsigned integers and return 32-bit whole part
 * @param a First 32-bit value
 * @param b Second 32-bit value
 * @return uint32_t 32-bit whole part of a/b
 */
uint32_t udiv32(const uint32_t a, const uint32_t b);

/**
 * @brief Divide two 16-bit unsigned integers and return 16-bit whole part
 * @param a First 16-bit value
 * @param b Second 16-bit value
 * @return uint16_t 16-bit whole part of a/b
 */
uint16_t udiv16(const uint16_t a, const uint16_t b);

/**
 * @brief Multiply two 16-bit unsigned integers and return a 16-bit value
 * @param a First 16-bit value
 * @param b Second 16-bit value
 * @return Truncated 16-bit product of a and b
 */
uint16_t umul16(const uint16_t a, const uint16_t b);

/**
 * @brief Multiply two 8-bit unsigned integers and return an 8-bit value
 * @param a First 8-bit value
 * @param b Second 8-bit value
 * @return Truncated 8-bit product of a and b
 */
#ifdef __clang__
inline uint8_t umul8(const uint8_t a, const uint8_t b)
{
    POKE(MULTINA, a);
    POKE(MULTINB, b);
    return PEEK(MULTOUT);
}
#else
uint8_t umul8(const uint8_t a, const uint8_t b);
#endif

#endif // __MEGA65_MATH_H
