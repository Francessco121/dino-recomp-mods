#pragma once

#include "PR/ultratypes.h"
#include "sys/math.h"

void real3d_draw_sphere(f32 x, f32 y, f32 z, f32 scale, u32 argb);
void real3d_draw_rect(SRT* srt, f32 width, f32 height, u32 argb);
void real3d_draw_cylinder(SRT* srt, f32 width, f32 height, u32 argb);
void real3d_draw_box(SRT* srt, f32 width, f32 height, f32 depth, u32 argb);
