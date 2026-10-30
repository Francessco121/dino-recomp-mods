#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "3d.h"
#include "debug_menus.h"
#include "debug_common.h"

#include "PR/ultratypes.h"
#include "sys/map.h"
#include "sys/math.h"
#include "dll.h"

extern s16 gMobileMapID;
extern s16 gMobileMapUnknown;
extern s8 gMapType;
extern s32 gMapCurrentStreamCoordsX;
extern s32 gMapCurrentStreamCoordsZ;
extern f32 gWorldX;
extern f32 gWorldZ;
extern s8 gMapLayer;
extern u32 gTrackFlags;

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

static s32 windowOpen = FALSE;

void track_debug_menu_callback(void) {
    dbgui_menu_item("Track", &windowOpen);
}

static void debug_trackflag_bit(const char* label, s32 i) {
    s32 value = (gTrackFlags >> i) & 1;
    if (dbgui_checkbox(label, &value)) {
        if (value) {
            gTrackFlags |= (1 << i);
        } else {
            gTrackFlags &= ~(1 << i);
        }
    }
}

RECOMP_CALLBACK(".", my_dbgui_event) void track_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (dbgui_begin("Track Debug", &windowOpen)) {
            dbgui_textf("gWorldX: %f", gWorldX);
            dbgui_textf("gWorldZ: %f", gWorldZ);
            dbgui_textf("gMapCurrentStreamCoordsX: %d", gMapCurrentStreamCoordsX);
            dbgui_textf("gMapCurrentStreamCoordsZ: %d", gMapCurrentStreamCoordsZ);
            dbgui_push_item_width(100);
            if (dbgui_input_sbyte("gMapLayer", &gMapLayer)) {
                if (gMapLayer > 2) gMapLayer = 2;
                if (gMapLayer < -2) gMapLayer = -2;
                gTrackFlags |= TRACKFLAG_LAYER_CHANGED;
            }
            dbgui_textf("gMapType: %d", gMapType);
            dbgui_textf("gMobileMapID: %d", gMobileMapID);
            dbgui_textf("gMobileMapUnknown: %d", gMobileMapUnknown);
            if (dbgui_tree_node(recomp_sprintf_helper("gTrackFlags: 0x%X###gTrackFlags", gTrackFlags))) {
                dbgui_push_item_width(100);
                dbgui_input_uint_ext("gTrackFlags", &gTrackFlags, &hexInput);

                debug_trackflag_bit("4: Sky", 4);
                debug_trackflag_bit("5: Anti-Alias", 5);
                debug_trackflag_bit("6: Sky Objects", 6);
                debug_trackflag_bit("7: Shadows", 7);
                debug_trackflag_bit("8: Sphere Mapping", 8);
                debug_trackflag_bit("9: 0x200", 9);
                debug_trackflag_bit("13: Disable Z-buffer", 13);
                debug_trackflag_bit("15: Block Lighting", 15);
                debug_trackflag_bit("17: Sun Glare", 17);

                dbgui_tree_pop();
            }
        }
        dbgui_end();
    }
}
