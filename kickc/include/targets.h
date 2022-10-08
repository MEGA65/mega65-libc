#ifndef _TARGETS_H_
#define _TARGETS_H_

#define TARGET_UNKNOWN 0
#define TARGET_MEGA65R2 1
#define TARGET_MEGAPHONER1 0x21
#define TARGET_MEGAPHONER4 0x22

unsigned char detect_target(void);

#endif
