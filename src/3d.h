#pragma once

#include <PR/ultratypes.h>

void draw_3d_text(f32 x, f32 y, f32 z, const char *text, u32 color);
void draw_3d_cube(f32 x, f32 y, f32 z, f32 size, u32 color);
void draw_3d_box(f32 x, f32 y, f32 z, f32 width, f32 height, f32 depth, u32 color);
void draw_3d_sphere(f32 x, f32 y, f32 z, f32 radius, u32 color);
