#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dll.h"

static s32 windowOpen = FALSE;

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

RECOMP_CALLBACK(".", my_debug_menu_event) void gplay_debug_menu_callback() {
    dbgui_menu_item("Gplay", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void gplay_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Gplay Debug", &windowOpen)) {
            GameState* state = gDLL_29_Gplay->vtbl->get_state();
            if (dbgui_begin_tab_bar("tabs")) {
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
