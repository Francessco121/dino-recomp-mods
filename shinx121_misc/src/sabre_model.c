// #include "modding.h"
// #include "recompconfig.h"
// #include "common.h"
// #include "sys/objects.h"

// static s32 wasSabreModel = FALSE;

// typedef enum {
//     USE_SABRE_MODEL_ON,
//     USE_SABRE_MODEL_OFF,
// } UseSabreModel;

// RECOMP_CALLBACK("*", recomp_on_game_tick_start) void sabre_model_game_tick() {
//     Object *player = get_player();

//     if (player != NULL) {
//         // Force sabre over fox if enabled
//         s32 playerno = gDLL_29_Gplay->vtbl->get_playerno();
//         if (playerno == PLAYER_SABRE) {
//             s32 useSabreModel = recomp_get_config_u32("sabre_model") == USE_SABRE_MODEL_ON;

//             if (!useSabreModel) {
//                 if (wasSabreModel) {
//                     player->modelInstIdx = 0;
//                     wasSabreModel = FALSE;
//                 }
//             } else if (useSabreModel) {
//                 player->modelInstIdx = 2;
//                 wasSabreModel = TRUE;
//             }
//         }
//     }
// }