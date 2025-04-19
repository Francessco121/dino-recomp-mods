#include "modding.h"
#include "imports.h"
#include "common.h"
#include "sys/math.h"

extern SRT gCameraSRT;
extern MtxF gViewMtx;
extern f32 gWorldX;
extern f32 gWorldZ;
extern f32 gAspect;

extern u32 func_800038DC(f32 x, f32 y, f32 z, f32 *ox, f32 *oy, u8 param_6);

static f32 vec2_length(f32 x, f32 y) {
    return sqrtf(x * x + y * y);
}

static void draw_line(f32 x1, f32 y1, f32 x2, f32 y2, u32 color) {
    DbgUiLine line = {
        .p1 = {
            .x = x1,
            .y = y1
        },
        .p2 = {
            .x = x2,
            .y = y2
        },
        .color = color,
        .thickness = 1.0f
    };
    recomp_dbgui_foreground_line(&line);
}

static s32 world_to_dbgui_coords(f32 x, f32 y, f32 z, f32 *o_sx, f32 *o_sy) {
    // World -> screen position
    f32 sx, sy;
    if (!func_800038DC(x - gWorldX, y, z - gWorldZ, &sx, &sy, 1)) {
        // Offscreen
        return FALSE;
    }

    // Correct for recomp screen aspect ratio difference
    f32 screen_width, screen_height;
    recomp_dbgui_get_display_size(&screen_width, &screen_height);

    u32 wh = get_some_resolution_encoded();
    u32 fb_width = (f32)(wh & 0xffff);
    u32 fb_height = (f32)(wh >> 16);

    f32 correct_screen_width = (screen_height * gAspect);

    sx = (sx / fb_width) * correct_screen_width;
    sy = (sy / fb_height) * screen_height;

    f32 offset_x = (screen_width - correct_screen_width) / 2.0f;
    sx += offset_x;

    // Output
    *o_sx = sx;
    *o_sy = sy;

    return TRUE;
}

void draw_3d_text(f32 x, f32 y, f32 z, const char *text, u32 color) {
    f32 sx, sy;
    if (world_to_dbgui_coords(x, y, z, &sx, &sy)) {
        DbgUiPos text_pos = {
            .x = sx,
            .y = sy
        };
        recomp_dbgui_foreground_text(&text_pos, color, text);
    }
}

void draw_3d_cube(f32 x, f32 y, f32 z, f32 size, u32 color) {
    Vec3f cube_points[8] = {
        {
            .x = x - size,
            .y = y - size,
            .z = z - size,
        },
        {
            .x = x + size,
            .y = y - size,
            .z = z - size,
        },
        {
            .x = x - size,
            .y = y + size,
            .z = z - size,
        },
        {
            .x = x + size,
            .y = y + size,
            .z = z - size,
        },

        {
            .x = x - size,
            .y = y - size,
            .z = z + size,
        },
        {
            .x = x + size,
            .y = y - size,
            .z = z + size,
        },
        {
            .x = x - size,
            .y = y + size,
            .z = z + size,
        },
        {
            .x = x + size,
            .y = y + size,
            .z = z + size,
        }
    };

    for (u32 i = 0; i < 8; i++) {
        if (!world_to_dbgui_coords(cube_points[i].x, cube_points[i].y, cube_points[i].z, 
            &cube_points[i].x, &cube_points[i].y)) {
            return;
        }
    }

    draw_line(cube_points[0].x, cube_points[0].y, cube_points[1].x, cube_points[1].y, color);
    draw_line(cube_points[0].x, cube_points[0].y, cube_points[2].x, cube_points[2].y, color);
    draw_line(cube_points[1].x, cube_points[1].y, cube_points[3].x, cube_points[3].y, color);
    draw_line(cube_points[2].x, cube_points[2].y, cube_points[3].x, cube_points[3].y, color);
    draw_line(cube_points[4].x, cube_points[4].y, cube_points[5].x, cube_points[5].y, color);
    draw_line(cube_points[4].x, cube_points[4].y, cube_points[6].x, cube_points[6].y, color);
    draw_line(cube_points[5].x, cube_points[5].y, cube_points[7].x, cube_points[7].y, color);
    draw_line(cube_points[6].x, cube_points[6].y, cube_points[7].x, cube_points[7].y, color);
    draw_line(cube_points[0].x, cube_points[0].y, cube_points[4].x, cube_points[4].y, color);
    draw_line(cube_points[1].x, cube_points[1].y, cube_points[5].x, cube_points[5].y, color);
    draw_line(cube_points[2].x, cube_points[2].y, cube_points[6].x, cube_points[6].y, color);
    draw_line(cube_points[3].x, cube_points[3].y, cube_points[7].x, cube_points[7].y, color);
}

void draw_3d_box(MtxF *mtx, f32 width, f32 height, f32 depth, u32 color) {
    width /= 2.0f;
    height /= 2.0f;
    depth /= 2.0f;

    Vec3f cube_points[8] = {
        {
            .x = -width,
            .y = -height,
            .z = -depth,
        },
        {
            .x = width,
            .y = -height,
            .z = -depth,
        },
        {
            .x = -width,
            .y = height,
            .z = -depth,
        },
        {
            .x = width,
            .y = height,
            .z = -depth,
        },

        {
            .x = -width,
            .y = -height,
            .z = depth,
        },
        {
            .x = width,
            .y = -height,
            .z = depth,
        },
        {
            .x = -width,
            .y = height,
            .z = depth,
        },
        {
            .x = width,
            .y = height,
            .z = depth,
        }
    };

    for (u32 i = 0; i < 8; i++) {
        f32 tx, ty, tz;
        vec3_transform(mtx, cube_points[i].x, cube_points[i].y, cube_points[i].z, &tx, &ty, &tz);
        
        cube_points[i].x = tx;
        cube_points[i].y = ty + height;
        cube_points[i].z = tz;

        if (!world_to_dbgui_coords(cube_points[i].x, cube_points[i].y, cube_points[i].z, 
            &cube_points[i].x, &cube_points[i].y)) {
            return;
        }
    }

    draw_line(cube_points[0].x, cube_points[0].y, cube_points[1].x, cube_points[1].y, color);
    draw_line(cube_points[0].x, cube_points[0].y, cube_points[2].x, cube_points[2].y, color);
    draw_line(cube_points[1].x, cube_points[1].y, cube_points[3].x, cube_points[3].y, color);
    draw_line(cube_points[2].x, cube_points[2].y, cube_points[3].x, cube_points[3].y, color);
    draw_line(cube_points[4].x, cube_points[4].y, cube_points[5].x, cube_points[5].y, color);
    draw_line(cube_points[4].x, cube_points[4].y, cube_points[6].x, cube_points[6].y, color);
    draw_line(cube_points[5].x, cube_points[5].y, cube_points[7].x, cube_points[7].y, color);
    draw_line(cube_points[6].x, cube_points[6].y, cube_points[7].x, cube_points[7].y, color);
    draw_line(cube_points[0].x, cube_points[0].y, cube_points[4].x, cube_points[4].y, color);
    draw_line(cube_points[1].x, cube_points[1].y, cube_points[5].x, cube_points[5].y, color);
    draw_line(cube_points[2].x, cube_points[2].y, cube_points[6].x, cube_points[6].y, color);
    draw_line(cube_points[3].x, cube_points[3].y, cube_points[7].x, cube_points[7].y, color);
}

void draw_3d_sphere(f32 x, f32 y, f32 z, f32 radius, u32 color) {
    Vec3f offset = { .x = radius, .y = 0, .z = 0 };

    // TODO: not sure this math is perfect... looks pretty close tho
    SRT cam_srt;
    cam_srt.transl.x = 0;
    cam_srt.transl.y = 0;
    cam_srt.transl.z = 0;
    cam_srt.scale = 1.0f;
    cam_srt.yaw = gCameraSRT.yaw;
    cam_srt.pitch = -gCameraSRT.pitch;
    cam_srt.roll = gCameraSRT.roll;
    MtxF cam_inv_mtx;
    matrix_from_srt_reversed(&cam_inv_mtx, &cam_srt);

    Vec3f transformed;
    vec3_transform_no_translate(&cam_inv_mtx, &offset, &transformed);

    f32 center_x, center_y;
    if (!world_to_dbgui_coords(x, y, z, &center_x, &center_y)) {
        return;
    }

    f32 edge_x, edge_y;
    if (!world_to_dbgui_coords(x + transformed.x, y + transformed.y, z + transformed.z, &edge_x, &edge_y)) {
        return;
    }
    
    f32 transformed_radius = vec2_length(
        edge_x - center_x,
        edge_y - center_y
    );

    DbgUiCircle circle = {
        .center = {
            .x = center_x,
            .y = center_y
        },
        .radius = transformed_radius,
        .color = color,
        .numSegments = 0,
        .thickness = 1.0f
    };
    recomp_dbgui_foreground_circle(&circle);
}
