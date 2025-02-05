#include "modding.h"
#include "imports.h"
#include "common.h"

const s32 FLY_COOLDOWN = 10;
s32 flyingCooldown = 0;
Vec3f flyPosition;
s32 flyEnabled = TRUE;

s32 charStatsWindowOpen = FALSE;

static void character_stats_window(s32 *open) {
    TActor *player = get_player();
    if (mod_dbgui_begin("Character Stats", open)) {
        if (player == NULL) {
            mod_dbgui_text("<no character>");
        } else {
            mod_dbgui_textf("Position: %d,%d,%d", 
                (s32)player->srt.transl.x, (s32)player->srt.transl.y, (s32)player->srt.transl.z);

            mod_dbgui_textf("Rotation (y,p,r): %d,%d,%d", 
                player->srt.yaw, player->srt.pitch, player->srt.roll);
            
            mod_dbgui_textf("Scale: %f", player->srt.scale);

            mod_dbgui_textf("Flags: %d", player->srt.flags);

            mod_dbgui_textf("Speed: %d,%d,%d", 
                (s32)player->speed.x, (s32)player->speed.y, (s32)player->speed.z);
            
            mod_dbgui_textf("unk_0x34: %u", player->unk_0x34);
            mod_dbgui_textf("matrixIdx: %d", player->matrixIdx);
            mod_dbgui_textf("unk_0x36: %u", player->unk_0x36);
            mod_dbgui_textf("unk_0x37: %u", player->unk_0x37);
            mod_dbgui_textf("unk_0x38: %u", player->unk_0x38);
            mod_dbgui_textf("objId: %u", player->objId);
            mod_dbgui_textf("unk0x46: %d", player->unk0x46);
            mod_dbgui_textf("ptr0x48: %p", player->ptr0x48);
            mod_dbgui_textf("curAnimId: %d", player->curAnimId);
            mod_dbgui_textf("unk_0xa2: %d", player->unk_0xa2);
            mod_dbgui_textf("modelInstIdx: %d", player->modelInstIdx);
            mod_dbgui_textf("unk0xae: %d", player->unk0xae);
            mod_dbgui_textf("unk0xaf: %d", player->unk0xaf);
            mod_dbgui_textf("unk0xb0: %u", player->unk0xb0);
            mod_dbgui_textf("unk0xc4: %u", player->unk0xc4);
            mod_dbgui_textf("unk0xdc: %d", player->unk0xdc);
        }
    }
    mod_dbgui_end();
}

RECOMP_CALLBACK("*", recomp_on_dbgui) void my_dbgui_callback() {
    if (!mod_dbgui_is_open()) return;

    if (mod_dbgui_begin_main_menu_bar()) {
        if (mod_dbgui_begin_menu("Debug")) {
            mod_dbgui_menu_item("Character Stats", &charStatsWindowOpen);
            mod_dbgui_end_menu();
        }
        if (mod_dbgui_begin_menu("Cheats")) {
            mod_dbgui_menu_item("Allow Noclip", &flyEnabled);
            mod_dbgui_end_menu();
        }
        
        mod_dbgui_end_main_menu_bar();
    }

    if (charStatsWindowOpen) {
        character_stats_window(&charStatsWindowOpen);
    }
}

RECOMP_CALLBACK("*", recomp_on_game_tick) void my_game_tick_callback() {
    TActor *player = get_player();

    if (player != NULL) {
        // Force sabre over fox
        s32 character = gDLL_29_gplay->exports->func_E90();
        if (character == 0) {
            player->modelInstIdx = 2;
        }

        // Noclip fly
        u16 buttons = get_masked_buttons(0);
        if ((buttons & L_TRIG) && flyEnabled) {
            if (flyingCooldown != FLY_COOLDOWN) {
                flyPosition = player->srt.transl;
            }

            const float SPEED = 15.0f;

            s8 jx = get_joystick_x(0);
            s8 jy = get_joystick_y(0);

            flyPosition.x += (jx / 127.0f) * SPEED;
            flyPosition.z += (jy / 127.0f) * SPEED;

            if (buttons & U_JPAD) {
                flyPosition.y += SPEED;
            } else if (buttons & D_JPAD) {
                flyPosition.y -= SPEED;
            }

            player->srt.transl = flyPosition;
            player->positionMirror = flyPosition;
            player->positionMirror2 = flyPosition;
            player->positionMirror3 = flyPosition;

            *((u16*)((u32)player + 0x0360)) = 28; // anim
            player->unk_0x36 = 0x80;
            player->animTimer = 0.0f;

            flyingCooldown = FLY_COOLDOWN;
        } else if (flyingCooldown > 0) {
            flyingCooldown--;
            *((u16*)((u32)player + 0x0360)) = 1;
            player->animTimer = 0.0f;
            player->unk_0x36 = 0xFF;
        }
    }
}
