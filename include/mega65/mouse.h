/**
 * @file mouse.h
 * @brief Simple mouse support for the Mega65 libC
 */

/*  Simple mouse support for the Mega65 libC

    Copyright (c) 2020 Paul Gardner-Stephen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Version   0.1
    Date      2020-07-01
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
