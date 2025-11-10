#include "modding.h"
#include "recompconfig.h"

#include "common.h"
#include "dlls/objects/210_player.h"
#include "sys/joypad.h"
#include "sys/objects.h"
#include "sys/math.h"

extern SRT gCameraSRT;
extern MtxF gViewMtx;

const s32 FLY_COOLDOWN = 10;

s32 flying_cooldown = 0;
Vec3f fly_position;

typedef enum {
    ALLOW_NOCLIP_ON,
    ALLOW_NOCLIP_OFF,
} AllowNoclip;

RECOMP_CALLBACK("*", recomp_on_game_tick) void noclip_fly_cheats_game_tick() {
    Object *player = get_player();

    if (player != NULL) {
        Player_Data *playerdata = (Player_Data*)player->data;

        // Noclip fly
        u16 buttons = gContPads[gVirtualContPortMap[0]].button;
        if ((buttons & L_TRIG) && recomp_get_config_u32("noclip") == ALLOW_NOCLIP_ON && player->parent == NULL) {
            if (flying_cooldown != FLY_COOLDOWN) {
                fly_position = player->srt.transl;
            }

            const float SPEED = 5.0f;

            s8 jx = joy_get_stick_x(0);
            s8 jy = joy_get_stick_y(0);

            Vec3f fly_dir;

            fly_dir.x = (jx / 127.0f);
            fly_dir.z = -(jy / 127.0f);

            if (buttons & U_JPAD) {
                fly_dir.y = 1;
            } else if (buttons & D_JPAD) {
                fly_dir.y = -1;
            }

            vec3_normalize(&fly_dir);
            
            fly_dir.x *= SPEED * gUpdateRateF;
            fly_dir.y *= SPEED * gUpdateRateF;
            fly_dir.z *= SPEED * gUpdateRateF;

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

            playerdata->unk0.animState = 28;
            player->opacity = 0x80;
            player->animProgress = 0.0f;

            flying_cooldown = FLY_COOLDOWN;
        } else if (flying_cooldown > 0) {
            flying_cooldown--;
            playerdata->unk0.animState = 1;
            player->animProgress = 0.0f;
            player->opacity = 0xFF;
        }
    }
}
