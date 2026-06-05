#include "modding.h"
#include "dbgui.h"

#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "sys/menu.h"

static s32 menu_debug_window_open = FALSE;
static s32 menu = -1;

void menu_debug_menu_callback(void) {
    dbgui_menu_item("Menu", &menu_debug_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void menu_debug_dbgui_callback(void) {
    if (menu_debug_window_open) {
        if (dbgui_begin("Menu Debug", &menu_debug_window_open)) {
            if (menu == -1) {
                menu = menu_get_current();
            }

            dbgui_textf("Current menu: %d", menu_get_current());
            
            if (dbgui_input_int("Menu", &menu)) {
                if (menu < 0) menu = 0;
                if (menu > 17) menu = 17;
            }

            if (dbgui_button("Set")) {
                menu_set(menu);
            }
            
        }
        dbgui_end();
    }
}