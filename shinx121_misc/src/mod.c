#include "modding.h"
#include "imports.h"

RECOMP_DECLARE_EVENT(my_debug_menu_event())
RECOMP_DECLARE_EVENT(my_cheats_menu_event())
RECOMP_DECLARE_EVENT(my_enhancements_menu_event())
RECOMP_DECLARE_EVENT(my_dbgui_event())

RECOMP_CALLBACK("*", recomp_on_dbgui) void my_dbgui_callback() {
    if (!recomp_dbgui_is_open()) return;

    if (recomp_dbgui_begin_main_menu_bar()) {
        if (recomp_dbgui_begin_menu("Debug")) {
            my_debug_menu_event();
            recomp_dbgui_end_menu();
        }
        if (recomp_dbgui_begin_menu("Cheats")) {
            my_cheats_menu_event();
            recomp_dbgui_end_menu();
        }
        if (recomp_dbgui_begin_menu("Enhancements")) {
            my_enhancements_menu_event();
            recomp_dbgui_end_menu();
        }
        
        recomp_dbgui_end_main_menu_bar();
    }

    my_dbgui_event();
}
