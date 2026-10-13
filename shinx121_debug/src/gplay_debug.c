#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "sys/map.h"
#include "sys/objects.h"
#include "dll.h"

static s32 windowOpen = FALSE;
static s32 gotoRestartPoint = FALSE;
static s32 gotoSavePoint = FALSE;

RECOMP_HOOK("mainHandleMapChange") void mainHandleMapChange_gplay_debug_hook(void) {
    // Note: Must be done as late as possible to avoid swapping game state while objects
    //       are still running their control/update logic.
    if (gotoRestartPoint) {
        dll_gplay->restart_goto();
    } else if (gotoSavePoint) {
        dll_gplay->start_loaded_game();
    }

    gotoRestartPoint = FALSE;
    gotoSavePoint = FALSE;
}

static void general_tab(void) {
    s32 restartIsSet = dll_gplay->restart_is_set();
    dbgui_textf("Restart Point Set: %s", restartIsSet ? "Yes" : "No");
    if (!restartIsSet) {
        dbgui_begin_disabled(TRUE);
    }
    if (dbgui_button("Go To Restart Point")) {
        gotoRestartPoint = TRUE;
    }
    if (!restartIsSet) {
        dbgui_end_disabled();
    }
    if (dbgui_button("Go To Save Point")) {
        gotoSavePoint = TRUE;
    }

    if (dbgui_tree_node("Danger Zone")) {
        if (dbgui_button("Set Restart Point")) {
            Object* player = objGetPlayer();
            if (player != NULL) {
                dll_gplay->restart_set(&player->srt.transl, player->srt.yaw, mapGetLayer());
            }
        }
        if (!restartIsSet) {
            dbgui_begin_disabled(TRUE);
        }
        if (dbgui_button("Clear Restart Point")) {
            dll_gplay->restart_clear();
        }
        if (!restartIsSet) {
            dbgui_end_disabled();
        }
        dbgui_tree_pop();
    }

    if (dbgui_tree_node("Super Danger Zone")) {
        if (dbgui_button("Set Save Point")) {
            Object* player = objGetPlayer();
            if (player != NULL) {
                dll_gplay->savepoint(&player->srt.transl, player->srt.yaw, 0, mapGetLayer());
            }
        }
        dbgui_tree_pop();
    }

    if (dbgui_tree_node("Save")) {
        if (dbgui_button("Save Game")) {
            dll_gplay->save_game();
        }
        dbgui_tree_pop();
    }
}

static void removed_saved_obj(Savefile *savefile, s32 idx) {
    for (s32 i = idx; i < (100 - 1); i++) {
        SavedObject *curr = &savefile->savedObjects[i];
        SavedObject *next = &savefile->savedObjects[i + 1];

        curr->uID = next->uID;
        curr->mapID = next->mapID;
        curr->_unk6 = next->_unk6;
        curr->x = next->x;
        curr->y = next->y;
        curr->z = next->z;
    }
    
    savefile->numSavedObjects--;
}

static void saved_objects_tab(GameState* state) {
    if (dbgui_begin_child("scroll")) {
        s32 delete = -1;
        for (s32 i = 0; i < state->save.file.numSavedObjects; i++) {
            SavedObject* obj = &state->save.file.savedObjects[i];
            dbgui_push_str_id(recomp_sprintf_helper("%d", i));
            dbgui_textf("savedObjects[%d]:", i);
            dbgui_same_line();
            if (dbgui_button("Delete")) {
                delete = i;
            }
            dbgui_textf("  UID 0x%X", obj->uID);
            dbgui_textf("  Map: %d", obj->mapID);
            dbgui_textf("  Pos: %f, %f, %f", obj->x, obj->y, obj->z);
            dbgui_pop_id();
        }
        if (delete >= 0) {
            removed_saved_obj(&state->save.file, delete);
        }
    }
    dbgui_end_child();
}

static void time_saves_tab(GameState* state) {
    if (dbgui_begin_child("scroll")) {
        for (s32 i = 0; i < state->save.file.timeSaveCount; i++) {
            TimeSave* timeSave = &state->save.file.timeSaves[i];
            dbgui_push_str_id(recomp_sprintf_helper("%d", i));
            dbgui_textf("timeSaves[%d]:", i);
            dbgui_same_line();
            if (dbgui_button("Delete")) {
                timeSave->time = 0;
            }
            dbgui_textf("  UID 0x%X", timeSave->uid);
            dbgui_textf("  Time: %.0f (%.0f left)", timeSave->time, timeSave->time - state->save.file.timePlayed);
            dbgui_pop_id();
        }
    }
    dbgui_end_child();
}

void gplay_debug_menu_callback(void) {
    dbgui_menu_item("Gplay", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void gplay_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (dbgui_begin("Gplay Debug", &windowOpen)) {
            GameState* state = gDLL_29_Gplay->vtbl->get_state();
            if (dbgui_begin_tab_bar("tabs")) {
                if (dbgui_begin_tab_item("General", NULL)) {
                    general_tab();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Saved Objects", NULL)) {
                    saved_objects_tab(state);
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Time Saves", NULL)) {
                    time_saves_tab(state);
                    dbgui_end_tab_item();
                }
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}
