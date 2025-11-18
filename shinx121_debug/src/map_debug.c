#include "modding.h"
#include "recomputils.h"
#include "dbgui.h"

#include "dlls/objects/210_player.h"
#include "game/objects/object.h"
#include "sys/fs.h"
#include "sys/memory.h"
#include "sys/map.h"
#include "sys/objects.h"
#include "common.h"
#include "functions.h"
#include "dll.h"

static s32 map_debug_window_open = FALSE;
static char mapNames[120][28];
static s32 loadedMapNames = FALSE;

static s32 mapID;
static s32 setupID;

RECOMP_CALLBACK(".", my_debug_menu_event) void map_debug_menu_callback() {
    dbgui_menu_item("Maps", &map_debug_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void map_debug_dbgui_callback() {
    if (map_debug_window_open) {
        if (!loadedMapNames) {
            void * mapInfo = read_alloc_file(MAPINFO_BIN, 0);
            for (s32 i = 0; i < 120; i++) {
                bcopy((void*)((u32)mapInfo + (0x20 * i)), mapNames[i], 28);
            }
            mmFree(mapInfo);

            loadedMapNames = TRUE;
        }

        if (dbgui_begin("Map Debug", &map_debug_window_open)) {
            Object *player = get_player();
            if (player != NULL) {
                s32 inMap = map_get_map_id_from_xz_ws(player->srt.transl.x, player->srt.transl.z);
                if (inMap >= 0 && inMap < 120) {
                    dbgui_textf("Player is currently in:\n[%d] %s", inMap, mapNames[inMap]);
                }
            }

            if (dbgui_begin_tab_bar("map_debug_tab_bar")) {
                if (dbgui_begin_tab_item("Loaded Maps", NULL)) {
                    for (s32 i = 0; i < 120; i++) {
                        if (gLoadedMapsDataTable[i] != NULL) {
                            dbgui_textf("%d: %s", i, mapNames[i]);
                        }
                    }
                    dbgui_end_tab_item();
                }

                if (dbgui_begin_tab_item("Map Setups", NULL)) {
                    dbgui_text("Setup Editor");
                    if (dbgui_input_int("Map ID", &mapID)) {
                        if (mapID < 0) mapID = 0;
                        if (mapID >= 120) mapID = 120 - 1;
                    }
                    if (dbgui_input_int("Setup ID", &setupID)) {
                        if (setupID < 0) setupID = 0;
                    }
                    if (dbgui_button("Set")) {
                        gDLL_29_Gplay->vtbl->set_map_setup(mapID, setupID);
                    }
                    dbgui_same_line();
                    dbgui_textf("%d -> %d (%s)", gDLL_29_Gplay->vtbl->get_map_setup(mapID), setupID, mapNames[mapID]);

                    dbgui_separator();

                    dbgui_text("Current Setup IDs");
                    if (dbgui_begin_child("setup_id_list")) {
                        for (s32 i = 0; i < 120; i++) {
                            if (dbgui_tree_node(recomp_sprintf_helper("[%d] %s: %d", i, mapNames[i], gDLL_29_Gplay->vtbl->get_map_setup(i)))) {
                                dbgui_text("Object Group Status");
                                for (s32 k = 0; k < 32; k++) {
                                    s32 status = gDLL_29_Gplay->vtbl->get_obj_group_status(i, k);
                                    if (dbgui_checkbox(recomp_sprintf_helper("%d", k), &status)) {
                                        gDLL_29_Gplay->vtbl->set_obj_group_status(i, k, status);
                                    }
                                    if (((k + 1) % 4) != 0) {
                                        dbgui_same_line();
                                    }
                                }
                                
                                dbgui_tree_pop();
                            }
                        }
                    }
                    dbgui_end_child();
                    dbgui_end_tab_item();
                }

                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}