#include "modding.h"
#include "dbgui.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dll.h"

static s32 windowOpen = FALSE;

RECOMP_CALLBACK(".", my_debug_menu_event) void screen_debug_menu_callback() {
    dbgui_menu_item("Screens", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void screen_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Screens Debug", &windowOpen)) {
            static s32 enabled = FALSE;
            dbgui_checkbox("Show Screen", &enabled);
            static s32 screenno = 0;
            dbgui_input_int("Screen", &screenno);
            
            if (enabled) {
                gDLL_20_Screens->vtbl->show_screen(screenno);
            }
        }
        dbgui_end();
    }
}
