#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dlls/engine/29_gplay.h"
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

static void state_tab_player_location(PlayerLocation* loc) {
    dbgui_textf("pos: %f, %f, %f\n", loc->vec.x, loc->vec.y, loc->vec.z);
    dbgui_textf("rotationY: %d\n", loc->rotationY);
    dbgui_textf("mapLayer: %d\n", loc->mapLayer);
}

static void state_tab_lactions(PlayerLightActions* acts) {
    dbgui_textf("skylights: %d, %d, %d, %d\n", acts->skylights[0], acts->skylights[1], acts->skylights[2], acts->skylights[3]);
    dbgui_textf("trklights: %d, %d\n", acts->trklights[0], acts->trklights[1]);
    dbgui_textf("unkC: %d\n", acts->unkC);
    dbgui_textf("newlfx: %d\n", acts->newlfx);
    dbgui_textf("flags: 0x%X\n", acts->flags);
    dbgui_textf("cam: %d\n", acts->cam);
}

static void state_tab_env_actions(PlayerEnvActions* acts) {
    dbgui_textf("unk0: %f\n", acts->unk0);
    dbgui_textf("unk4: %d\n", acts->unk4);
    dbgui_textf("unk6: %d\n", acts->unk6);
    dbgui_textf("unk8: %d\n", acts->unk8);
    dbgui_textf("unkA: %d\n", acts->unkA);
    dbgui_textf("unkC: %d\n", acts->unkC);
    dbgui_textf("unkE: %d\n", acts->unkE);

    dbgui_textf("unk10: %d, %d, %d\n", acts->unk10, acts->unk14, acts->unk18);
    dbgui_textf("unk1C: %d, %d, %d\n", acts->unk1C, acts->unk20, acts->unk24);
    dbgui_textf("unk28: %d, %d, %d\n", acts->unk28, acts->unk2C, acts->unk30);

    dbgui_textf("unk3C: 0x%X\n", acts->unk3C);
    dbgui_textf("unk3D: %d\n", acts->unk3D);
    dbgui_textf("unk3E: %d\n", acts->unk3E);
    dbgui_textf("unk3F: %d\n", acts->unk3F);
}

static void state_tab_music_actions(PlayerMusicActions* acts) {
    dbgui_textf("actionNums (ambience): %d, %d\n", acts->actionNums[0], acts->actionNums[1]);
    dbgui_textf("actionNums (music): %d, %d\n", acts->actionNums[2], acts->actionNums[3]);
}

static void state_tab(GameState *state) {
    if (dbgui_begin_tab_bar("player_tabs")) {
        for (s32 playerno = 0; playerno < 2; playerno++) {
            const char* tabName = playerno == PLAYER_SABRE ? "Sabre": "Krystal";
            if (dbgui_begin_tab_item(tabName, NULL)) {
                if (dbgui_tree_node("Player Location")) {
                    state_tab_player_location(&state->save.map.playerLocations[playerno]);
                    dbgui_tree_pop();
                }
                if (dbgui_tree_node("Light Actions")) {
                    state_tab_lactions(&state->save.map.lightActions[playerno]);
                    dbgui_tree_pop();
                }
                if (dbgui_tree_node("Env Actions")) {
                    state_tab_env_actions(&state->save.map.envActions[playerno]);
                    dbgui_tree_pop();
                }
                if (dbgui_tree_node("Music Actions")) {
                    state_tab_music_actions(&state->save.map.musicActions[playerno]);
                    dbgui_tree_pop();
                }
                dbgui_end_tab_item();
            }
        }
        dbgui_end_tab_bar();
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
                if (dbgui_begin_tab_item("State", NULL)) {
                    state_tab(state);
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
