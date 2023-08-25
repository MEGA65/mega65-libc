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

#include <mega65/math.h>

uint32_t umul32(const uint32_t a, const uint32_t b)
{
    POKE32(MULTINA, a);
    POKE32(MULTINB, b);
    return PEEK32(MULTOUT);
}

uint32_t udiv32(const uint32_t a, const uint32_t b)
{
    POKE32(MULTINA, a);
    POKE32(MULTINB, b);
    while (PEEK(DIVBUSY) != 0) { };
    return PEEK32(DIVOUT_WHOLE);
}

uint16_t udiv16(const uint16_t a, const uint16_t b)
{
    POKE16(MULTINA, a);
    POKE16(MULTINB, b);
    POKE16(MULTINA + 2, 0);
    POKE16(MULTINB + 2, 0);
    while (PEEK(DIVBUSY) != 0) { };
    return PEEK16(DIVOUT_WHOLE);
}

uint16_t umul16(const uint16_t a, const uint16_t b)
{
    POKE16(MULTINA, a);
    POKE16(MULTINB, b);
    return PEEK16(MULTOUT);
}

#ifndef __clang__
uint8_t umul8(const uint8_t a, const uint8_t b)
{
    POKE(MULTINA, a);
    POKE(MULTINB, b);
    return PEEK(MULTOUT);
}
#endif
