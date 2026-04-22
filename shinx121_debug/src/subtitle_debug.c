#include "modding.h"
#include "dbgui.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dll.h"

static s32 windowOpen = FALSE;
static s32 textID;

extern s16 gLetterboxTarget;

RECOMP_CALLBACK(".", my_debug_menu_event) void subtitle_debug_menu_callback() {
    dbgui_menu_item("Subtitles", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void subtitle_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Subtitle Debug", &windowOpen)) {

            s32 letterbox = gLetterboxTarget;
            if (dbgui_input_int("Letterbox", &letterbox)) {
                if (letterbox < 0) letterbox = 0;
                gLetterboxTarget = letterbox;
            }

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
