/**
 * @file targets.h
 * @brief Target detection, e.g. Nexys4, Mega65R3, etc.
 */

#ifndef __MEGA65_TARGETS_H
#define __MEGA65_TARGETS_H

#include <stdint.h>

#define TARGET_UNKNOWN 0            //!< Unknown target
#define TARGET_MEGA65R1 1           //!< MEGA65 board revision 1
#define TARGET_MEGA65R2 2           //!< MEGA65 board revision 2
#define TARGET_MEGA65R3 3           //!< MEGA65 board revision 3
#define TARGET_MEGAPHONER1 0x21     //!< MEGAphone revision 1
#define TARGET_MEGAPHONER4 0x22     //!< MEGAphone revision 4
#define TARGET_NEXYS4 0x40          //!< Nexys4 board
#define TARGET_NEXYS4DDR 0x41       //!< Nexys4 DDR board
#define TARGET_NEXYS4DDRWIDGET 0x42 //!< Nexys4 DDR board with Widget
#define TARGET_WUKONG 0xFD          //!< Wukong board
#define TARGET_SIMULATION 0xFE      //!< Simulation
#define TARGET_EMULATION 0xFF       //!< Emulator, e.g. Xemu

/**
 * @brief Detect target
 */
uint8_t detect_target(void);

#endif // __MEGA65_TARGETS_H
