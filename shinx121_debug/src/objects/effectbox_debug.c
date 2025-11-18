#include "effectbox_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "../3d.h"

#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "recomputils.h"
#include "sys/math.h"

typedef struct {
    ObjSetup base;
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E;
    u8 unk1F;
    s16 unk20;
    u8 unk22;
} EffectBox_Setup;

void draw_effectbox(Object *obj, u32 color) {
    Vec3f position = obj->positionMirror3;
    EffectBox_Setup *setup = (EffectBox_Setup*)obj->setup;

    f32 width = setup->unk1A * 2;
    f32 height = setup->unk1B * 2;
    f32 depth = setup->unk1C * 2;

    SRT srt = {
        .yaw = ((u16)setup->unk18) * 0x100,
        .pitch = ((u16)setup->unk19) * -0x100,
        .roll = 0,
        .flags = 0,
        .scale = 1.0f,
        .transl = position
    };

    MtxF mtx;
    matrix_from_srt(&mtx, &srt);

    draw_3d_box(&mtx, width, height, depth, color);
}
