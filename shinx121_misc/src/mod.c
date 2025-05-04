#include "modding.h"
#include "dbgui.h"

RECOMP_DECLARE_EVENT(my_debug_menu_event())
RECOMP_DECLARE_EVENT(my_cheats_menu_event())
RECOMP_DECLARE_EVENT(my_enhancements_menu_event())
RECOMP_DECLARE_EVENT(my_dbgui_event())

RECOMP_CALLBACK("*", recomp_on_dbgui) void my_dbgui_callback() {
    if (!dbgui_is_open()) return;

    if (dbgui_begin_main_menu_bar()) {
        if (dbgui_begin_menu("Debug")) {
            my_debug_menu_event();
            dbgui_end_menu();
        }
        if (dbgui_begin_menu("Cheats")) {
            my_cheats_menu_event();
            dbgui_end_menu();
        }
        if (dbgui_begin_menu("Enhancements")) {
            my_enhancements_menu_event();
            dbgui_end_menu();
        }
        
        dbgui_end_main_menu_bar();
    }

    my_dbgui_event();
}
