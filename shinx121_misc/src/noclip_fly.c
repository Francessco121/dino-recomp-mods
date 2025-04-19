#include "modding.h"
#include "imports.h"
#include "common.h"
#include "sys/controller.h"
#include "sys/objects.h"
#include "sys/math.h"

extern SRT gCameraSRT;
extern MtxF gViewMtx;

const s32 FLY_COOLDOWN = 10;

s32 flying_cooldown = 0;
Vec3f fly_position;
s32 fly_enabled = TRUE;

RECOMP_CALLBACK(".", my_cheats_menu_event) void noclip_fly_cheats_menu_callback() {
    recomp_dbgui_menu_item("Allow Noclip", &fly_enabled);
}

RECOMP_CALLBACK("*", recomp_on_game_tick) void noclip_fly_cheats_game_tick() {
    Object *player = get_player();

    if (player != NULL) {
        // Noclip fly
        u16 buttons = gContPads[gVirtualContPortMap[0]].button;
        if ((buttons & L_TRIG) && fly_enabled && recomp_dbgui_is_enabled()) {
            if (flying_cooldown != FLY_COOLDOWN) {
                fly_position = player->srt.transl;
            }

            const float SPEED = 10.0f;

            s8 jx = get_joystick_x(0);
            s8 jy = get_joystick_y(0);

            Vec3f fly_dir;

            fly_dir.x = (jx / 127.0f);
            fly_dir.z = -(jy / 127.0f);

            if (buttons & U_JPAD) {
                fly_dir.y = 1;
            } else if (buttons & D_JPAD) {
                fly_dir.y = -1;
            }

            vec3_normalize(&fly_dir);
            
            fly_dir.x *= SPEED;
            fly_dir.y *= SPEED;
            fly_dir.z *= SPEED;

            SRT cam_srt = {
                .transl.x = 0,
                .transl.y = 0,
                .transl.z = 0,
                .scale = 1.0f,
                .yaw = gCameraSRT.yaw,
                .pitch = 0,
                .roll = 0
            };
            MtxF cam_mtx;
            matrix_from_srt_reversed(&cam_mtx, &cam_srt);

            Vec3f fly_dir_transformed;
            vec3_transform_no_translate(&cam_mtx, &fly_dir, &fly_dir_transformed);

            fly_position.x += fly_dir_transformed.x;
            fly_position.y += fly_dir_transformed.y;
            fly_position.z += fly_dir_transformed.z;

            player->srt.transl = fly_position;
            player->positionMirror = fly_position;
            player->positionMirror2 = fly_position;
            player->positionMirror3 = fly_position;

            *((u16*)((u32)player + 0x0360)) = 28; // anim
            player->unk_0x36 = 0x80;
            player->animTimer = 0.0f;

            flying_cooldown = FLY_COOLDOWN;
        } else if (flying_cooldown > 0) {
            flying_cooldown--;
            *((u16*)((u32)player + 0x0360)) = 1;
            player->animTimer = 0.0f;
            player->unk_0x36 = 0xFF;
        }
    }
}
