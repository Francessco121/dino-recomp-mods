#include "modding.h"
#include "dbgui.h"

#include "debug_menus.h"

RECOMP_DECLARE_EVENT(my_dbgui_event())

static void debug_menus(void) {
    anim_debug_menu_callback();
    bittable_debug_menu_callback();
    blocks_debug_menu_callback();
    curve_debug_menu_callback();
    gplay_debug_menu_callback();
    lighting_debug_menu_callback();
    map_debug_menu_callback();
    menu_debug_menu_callback();
    object_debug_menu_callback();
    partfx_debug_menu_callback();
    race_debug_menu_callback();
    screen_debug_menu_callback();
    shadow_debug_menu_callback();
    sound_test_menu_callback();
    subtitle_debug_menu_callback();
    task_debug_menu_callback();
    time_debug_menu_callback();
}

RECOMP_CALLBACK("*", recomp_on_dbgui) void my_dbgui_callback(void) {
    if (!dbgui_is_open()) return;

    if (dbgui_begin_main_menu_bar()) {
        if (dbgui_begin_menu("Debug")) {
            dbgui_separator_text("Debug Tools");
            debug_menus();
            dbgui_end_menu();
        }
        
        dbgui_end_main_menu_bar();
    }

    my_dbgui_event();
}
