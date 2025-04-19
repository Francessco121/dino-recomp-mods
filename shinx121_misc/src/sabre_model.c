#include "modding.h"
#include "imports.h"
#include "common.h"
#include "sys/objects.h"

static s32 use_sabre_model = TRUE;
static s32 was_sabre_model = TRUE;

RECOMP_CALLBACK(".", my_enhancements_menu_event) void sabre_model_enhancements_menu_callback() {
    recomp_dbgui_menu_item("Use Sabre Model", &use_sabre_model);
}

RECOMP_CALLBACK("*", recomp_on_game_tick_start) void sabre_model_game_tick() {
    Object *player = get_player();

    if (player != NULL) {
        // Force sabre over fox if enabled
        s32 character = gDLL_29_gplay->exports->func_E90();
        if (character == CHARACTER_SABRE) {
            if (!use_sabre_model) {
                if (was_sabre_model) {
                    player->modelInstIdx = 0;
                    was_sabre_model = FALSE;
                }
            } else if (use_sabre_model) {
                player->modelInstIdx = 2;
                was_sabre_model = TRUE;
            }
        }
    }
}