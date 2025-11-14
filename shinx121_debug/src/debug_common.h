#pragma once

#include "dbgui.h"

#include "PR/ultratypes.h"
#include "game/objects/object.h"
#include "sys/math.h"

extern void bcopy_recomp(const void *src, void *dst, int length);
#define bcopy bcopy_recomp

extern void bzero_recomp(void *, int);
#define bzero bzero_recomp

// TODO: clean up struct in decomp and remove this one
typedef struct{
/*00*/    s16 Ax;
/*02*/    s16 Bx;
/*04*/    s16 Ay;
/*06*/    s16 By;
/*08*/    s16 Az;
/*0a*/    s16 Bz;
/*0c*/    union { //height can be treated as a single value "(heightA << 8) + heightB", depending on top settings bit
    struct {
        s8 heightA; 
        s8 heightB;
    };
    s16 height;
};
/*0e*/    s8 settingsA;
/*0f*/    s8 settingsB;
/*10*/    s16 animatorID;
/*12*/    u8 _pad[2];
} HitsLineCustom;

void dbgui_input_byte(const char *label, u8 *value);
s32 dbgui_input_byte_ext(const char *label, u8 *value, const DbgUiInputIntOptions *options);
s32 dbgui_input_sbyte(const char *label, s8 *value);
s32 dbgui_input_sbyte_ext(const char *label, s8 *value, const DbgUiInputIntOptions *options);
void dbgui_input_short(const char *label, s16 *value);
void dbgui_input_short_ext(const char *label, s16 *value, const DbgUiInputIntOptions *options);
void dbgui_input_uint(const char *label, u32 *value);
void dbgui_input_vec3f(const char *label, Vec3f *value);
void dbgui_input_obj_position(const char *label, Object *value);
