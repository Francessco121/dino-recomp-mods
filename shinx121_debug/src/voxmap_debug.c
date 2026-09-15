#include "3d.h"
#include "game/objects/object.h"
#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_common.h"
#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "sys/math.h"
#include "sys/voxmap.h"
#include "sys/objects.h"

VoxmapSlot* voxFindClosestSlot(Vec3s16 *position);
u8 *vox_func_80007D08(u8 *arg0, u8 *arg1, u8 *arg2, s32 arg3, s32 arg4, s32 arg5);

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

static s32 windowOpen = FALSE;
static VoxmapSlot* slot = NULL;
static f32 drawDist = 800;

static void voxmap_3d_debug(void) {
    if (slot == NULL || slot->data == NULL) return;

    Object* player = objGetPlayer();
    if (player == NULL) {
        return;
    }

    s32 height = slot->data->unkC - slot->data->unk4;

    for (s32 x = 0; x < 64; x++) {
        for (s32 z = 0; z < 64; z++) {
            for (s32 y = 0; y < height; y++) {
                s32 tempX = x >> 2;
                s32 tempZ = z >> 2;

                s32 val = slot->data->unk24[(y << 5) | ((tempZ * 2) + (tempX >> 3))];
                if ((val >> (tempX & 7)) & 1) {
                    u8* sp68 = vox_func_80007D08(slot->data->unk1C, slot->data->unk14, slot->data->unk24, tempX, y, tempZ);
                    if ((sp68[z & 3] >> ((x & 3) << 1)) & 3) {
                        Vec3s16 voxWorldPos;
                        voxWorldPos.x = x + slot->unk8;
                        voxWorldPos.y = y + slot->data->unk4;
                        voxWorldPos.z = z + slot->unkC;

                        Vec3f worldPos;
                        vox_func_80007E2C(&worldPos, &voxWorldPos);

                        f32 dist = vec3Distance(&player->globalPosition, &worldPos);
                        if (dist <= drawDist) {
                            draw_3d_sphere_ext(worldPos.x, worldPos.y, worldPos.z, 0.75f, 0xFFFFBBFF, 4, 1.0f, TRUE);
                        }
                    }
                }
            }
        }
    }
}

static void voxmap_debug(void) {
    dbgui_input_float("3D Draw Distance", &drawDist);

    Object* player = objGetPlayer();
    if (player == NULL) {
        return;
    }

    Vec3s16 voxpos;
    vox_func_80007EE0(&player->globalPosition, &voxpos);

    slot = voxFindClosestSlot(&voxpos);

    dbgui_textf("unk0: %d", slot->unk0);
    dbgui_textf("unk4: %d", slot->unk4);
    dbgui_textf("unk8: %d", slot->unk8);
    dbgui_textf("unkC: %d", slot->unkC);

    VoxmapSlotData* data = slot->data;
    if (data == NULL) {
        return;
    }
    dbgui_separator_text("data");

    dbgui_textf("pad0: %d", data->pad0);
    dbgui_textf("unk4: %d", data->unk4);
    dbgui_textf("pad8: %d", data->pad8);
    dbgui_textf("unkC: %d", data->unkC);
    dbgui_textf("pad10: %d", data->pad10);
    dbgui_textf("unk14: %p", data->unk14);
    dbgui_textf("pad18: %d", data->pad18);
    dbgui_textf("unk1C: %p", data->unk1C);
    dbgui_textf("pad20: %d", data->pad20);
    dbgui_textf("unk24: %p", data->unk24);

}

void voxmap_debug_menu_callback(void) {
    dbgui_menu_item("Voxmap", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void voxmap_debug_dbgui_callback(void) {
    slot = NULL;

    if (windowOpen) {
        if (dbgui_begin("Voxmap Debug", &windowOpen)) {
            voxmap_debug();
        }
        dbgui_end();

        voxmap_3d_debug();
    }
}
