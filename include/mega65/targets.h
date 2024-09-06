/**
 * @file targets.h
 * @brief Target detection, e.g. Nexys4, Mega65R3, etc.
 */

#ifndef __MEGA65_TARGETS_H
#define __MEGA65_TARGETS_H

#include <stdint.h>

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

enum
#ifdef __clang__
    : uint8_t
#endif
{
    TARGET_UNKNOWN = 0,            //!< Unknown target
    TARGET_MEGA65R1 = 1,           //!< MEGA65 board revision 1
    TARGET_MEGA65R2 = 2,           //!< MEGA65 board revision 2
    TARGET_MEGA65R3 = 3,           //!< MEGA65 board revision 3
    TARGET_MEGA65R4 = 4,           //!< MEGA65 board revision 4
    TARGET_MEGA65R5 = 5,           //!< MEGA65 board revision 5
    TARGET_MEGA65R6 = 6,           //!< MEGA65 board revision 6
    TARGET_MEGAPHONER1 = 0x21,     //!< MEGAphone revision 1
    TARGET_MEGAPHONER4 = 0x22,     //!< MEGAphone revision 4
    TARGET_NEXYS4 = 0x40,          //!< Nexys4 board
    TARGET_NEXYS4DDR = 0x41,       //!< Nexys4 DDR board
    TARGET_NEXYS4DDRWIDGET = 0x42, //!< Nexys4 DDR board with Widget
    TARGET_QMTECHA100T = 0x60,     //!< QMTECH A100T board
    TARGET_QMTECHA200T = 0x61,     //!< QMTECH A200T board
    TARGET_QMTECHA325T = 0x62,     //!< QMTECH A325T board
    TARGET_WUKONG = 0xFD,          //!< Wukong A100T board
    TARGET_SIMULATION = 0xFE,      //!< Simulation
    TARGET_EMULATION = 0xFF        //!< Emulator, e.g. Xemu
};

/**
 * @brief Detect target
 */
uint8_t detect_target(void);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_TARGETS_H
