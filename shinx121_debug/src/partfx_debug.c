#include "dll.h"
#include "modding.h"
#include "recomputils.h"
#include "dbgui.h"

#include "PR/os.h"
#include "dlls/objects/210_player.h"
#include "dlls/engine/17_partfx.h"
#include "sys/math.h"
#include "sys/objects.h"
#include "sys/main.h"
#include "macros.h"

extern void bzero_recomp(void *, int);
#define bzero bzero_recomp

static const DbgUiInputIntOptions hexInput = {
    .step = 1,
    .stepFast = 0x10,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

static s32 windowOpen = FALSE;
static s32 id;

static union {
    f32 floats[3];
    u8 bytes[3];
    s32 bool;
} extra;
static u32 extraType = 1;
static const char *extraTypeNames[] = {"Null", "Float", "Byte", "Bool"};
static s32 count = 1;
static SRT srt = { .scale = 1.0f };
static s32 arg3 = 0x200000;
static s8 arg4 = -1;
static s32 delay = 10;
static s32 timer = 0;
static s32 looping = 0;
static s32 useSRT = TRUE;

RECOMP_CALLBACK(".", my_debug_menu_event) void partfx_debug_menu_callback() {
    dbgui_menu_item("Particles", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void partfx_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Particle Debug", &windowOpen)) {
            Object *player = get_player();

            dbgui_input_int_ext("ID", &id, &hexInput);
           
            if (dbgui_begin_combo("Extra Datatype", extraTypeNames[extraType])) {
                for (u32 i = 0; i < ARRAYCOUNT(extraTypeNames); i++) {
                    if (dbgui_selectable(extraTypeNames[i], i == extraType)) {
                        extraType = i;
                        bzero(&extra, sizeof(extra));
                    }
                }
                dbgui_end_combo();
            }

            if (extraType == 1) {
                for (s32 i = 0; i < 3; i++) {
                    dbgui_input_float(recomp_sprintf_helper("extra[%d]", i), &extra.floats[i]);
                }
            } else if (extraType == 2) {
                for (s32 i = 0; i < 3; i++) {
                    s32 byte = extra.bytes[i];
                    if (dbgui_input_int(recomp_sprintf_helper("extra[%d]", i), &byte)) {
                        extra.bytes[i] = byte;
                    }
                }
            } else if (extraType == 3) {
                s32 checked = extra.bool != 0 ? 1 : 0;
                if (dbgui_checkbox("extra", &checked)) {
                    extra.bool = checked ? 1 : 0;
                }
            }

            dbgui_input_int_ext("Arg3", &arg3, &hexInput);
            {
                s32 byte = arg4;
                if (dbgui_input_int("Arg4", &byte)) {
                    arg4 = byte;
                }
            }

            dbgui_checkbox("Use SRT", &useSRT);

            if (useSRT) {
                if (dbgui_tree_node("SRT")) {
                    dbgui_input_float("srt.x", &srt.transl.x);
                    dbgui_input_float("srt.y", &srt.transl.y);
                    dbgui_input_float("srt.z", &srt.transl.z);
                    
                    {
                        s32 rot = srt.yaw;
                        if (dbgui_input_int_ext("srt.yaw", &rot, &hexInput)) {
                            CIRCLE_WRAP(rot);
                            srt.yaw = rot;
                        }
                    }
                    {
                        s32 rot = srt.pitch;
                        if (dbgui_input_int_ext("srt.pitch", &rot, &hexInput)) {
                            CIRCLE_WRAP(rot);
                            srt.pitch = rot;
                        }
                    }
                    {
                        s32 rot = srt.roll;
                        if (dbgui_input_int_ext("srt.roll", &rot, &hexInput)) {
                            CIRCLE_WRAP(rot);
                            srt.roll = rot;
                        }
                    }

                    dbgui_input_float("srt.scale", &srt.scale);

                    dbgui_tree_pop();
                }
            }

            if (dbgui_input_int("Count", &count)) {
                if (count < 1) count = 1;
            }

            timer -= gUpdateRateF;

            if (dbgui_button("Spawn") || (looping && timer <= 0)) {
                for (s32 i = 0; i < count; i++) {
                    gDLL_17_partfx->vtbl->spawn(player, id, useSRT ? &srt : NULL, arg3, arg4, extraType == 0 ? NULL : &extra);
                }
            }

            if (timer <= 0) {
                timer += delay;
            }

            if (dbgui_input_int("Delay", &delay)) {
                if (delay < 0) delay = 0;
            }

            if (looping) {
                if (dbgui_button("Stop")) {
                    looping = FALSE;
                }
            } else {
                if (dbgui_button("Start")) {
                    looping = TRUE;
                    timer = 0;
                }
            }
        }
        dbgui_end();
    }
}