/**
 * @file mouse.h
 * @brief Simple mouse support for the Mega65 libC
 * @author Paul Gardner-Stephen
 * @date 2020
 */

#ifndef __MEGA65_MOUSE_H
#define __MEGA65_MOUSE_H

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

void mouse_warp_to(unsigned short x, unsigned short y);
void mouse_update_position(unsigned short* mx, unsigned short* my);
void mouse_update_pointer(void);
char mouse_clicked(void);
void mouse_clip_position(void);
void mouse_bind_to_sprite(unsigned char sprite_num);
void mouse_set_bounding_box(
    unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_MOUSE_H
