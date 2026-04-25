#include "modding.h"
#include "dbgui.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dll.h"

static s32 windowOpen = FALSE;
static s32 textID;
static s32 overrideLetterbox = FALSE;
static s32 letterboxOverride = 0;

RECOMP_CALLBACK(".", my_debug_menu_event) void subtitle_debug_menu_callback() {
    dbgui_menu_item("Subtitles", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void subtitle_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Subtitle Debug", &windowOpen)) {
            dbgui_checkbox("Override Letterbox", &overrideLetterbox);
            if (overrideLetterbox) {
                if (dbgui_input_int("Letterbox", &letterboxOverride)) {
                    if (letterboxOverride < 0) letterboxOverride = 0;
                    if (letterboxOverride > (128 - 6)) letterboxOverride = (128 - 6);
                }

                gDLL_2_Camera->vtbl->set_letterbox_goal(letterboxOverride, TRUE);
            }
            dbgui_textf("Current letterbox: %d", camera_get_letterbox());

            dbgui_set_next_item_width(100);
            if (dbgui_input_int("Gametext ID", &textID)) {
                if (textID < 0) textID = 0;
            }

            if (dbgui_button("Play")) {
                gDLL_22_Subtitles->vtbl->func_368(textID);
            }
        }
        dbgui_end();
    }
}
