#include "dll.h"
#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"
#include "3d.h"

#include "PR/ultratypes.h"
#include "dlls/engine/26_curves.h"
#include "sys/curves.h"
#include "sys/math.h"

extern SRT gCameraSRT;
extern f32 gWorldX;
extern f32 gWorldZ;

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

static s32 curveDebugWindowOpen = FALSE;
static f32 curveRenderDist = 800;
static s32 filterByType = FALSE;
static s32 filterByDist = FALSE;
static s32 curveTypeFilter = 0;
static void debug_draw_curves();

RECOMP_CALLBACK(".", my_debug_menu_event) void curve_debug_menu_callback() {
    dbgui_menu_item("Curves", &curveDebugWindowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void curve_debug_dbgui_callback() {
    if (curveDebugWindowOpen) {
        if (dbgui_begin("Curve Debug", &curveDebugWindowOpen)) {
            debug_draw_curves();
            
        }
        dbgui_end();
    }
}

#include "recomp/dlls/engine/26_curves_recomp.h"

extern CurveSetup *_bss_0;
extern CurveSetup *_bss_4;
extern CurveNode _bss_8[1300];
extern s32 _bss_28A8;

static void debug_draw_curves() {
    s32 hoveredIdx = -1;
    Vec3f camPos = {gCameraSRT.transl.x + gWorldX, gCameraSRT.transl.y, gCameraSRT.transl.z + gWorldZ};

    dbgui_set_next_item_width(120);
    dbgui_input_float("3D Render Distance", &curveRenderDist);
    dbgui_checkbox("Filter List By Distance", &filterByDist);

    dbgui_checkbox("Filter Types", &filterByType);
    if (filterByType) {
        dbgui_input_int_ext("Type", &curveTypeFilter, &hexInput);
    }

    dbgui_textf("Loaded curves (%d):", _bss_28A8);
    if (dbgui_begin_child("curves")) {
        for (s32 i = 0; i < _bss_28A8; i++) {
            CurveNode *node = &_bss_8[i];
            CurveSetup *setup = node->setup;

            if (filterByType && setup->unk19 != curveTypeFilter) {
                continue;
            }

            if (filterByDist) {
                f32 dist = vec3_distance(&camPos, &setup->pos);
                if (dist > curveRenderDist) {
                    continue;
                }
            }

            if (dbgui_tree_node(recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X###%X", i, setup->unk19, node->uID, node->uID))) {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }

                dbgui_textf("objId: 0x%X", setup->objId);
                dbgui_textf("quarterSize: 0x%X", setup->quarterSize);
                dbgui_textf("unk3: 0x%X", setup->unk3);
                if (setup->unk19 == 0x22) {
                    dbgui_textf("unk4: 0x%X", setup->base_type22.unk4);
                    dbgui_textf("unk6: 0x%X", setup->base_type22.unk6);
                    dbgui_textf("unk7: 0x%X", setup->base_type22.unk7);
                } else if (setup->unk19 == 0x26) {
                    dbgui_textf("unk4: 0x%X", setup->base_type26.unk4);
                    dbgui_textf("unk5: 0x%X", setup->base_type26.unk5);
                    dbgui_textf("unk6: 0x%X", setup->base_type26.unk6);
                    dbgui_textf("unk7: 0x%X", setup->base_type26.unk7);
                }
                dbgui_textf("x: %f", setup->pos.x);
                dbgui_textf("y: %f", setup->pos.y);
                dbgui_textf("z: %f", setup->pos.z);
                dbgui_textf("uID: 0x%X", setup->uID);

                dbgui_textf("unk18: 0x%X", setup->unk18);
                dbgui_textf("unk19: 0x%X", setup->unk19);
                dbgui_textf("unk1A: 0x%X", setup->unk1A);
                dbgui_textf("unk1B: 0x%X", setup->unk1B);
                for (s32 k = 0; k < 4; k++) {
                    dbgui_textf("unk1C[%d]: 0x%X", k, setup->unk1C[k]);
                }
                dbgui_textf("unk2C: 0x%X", setup->unk2C);
                dbgui_textf("unk2D: 0x%X", setup->unk2D);
                dbgui_textf("unk2E: 0x%X", setup->unk2E);
                dbgui_textf("unk2F: 0x%X", setup->unk2F);
                if (setup->unk19 == 0x15) {
                    dbgui_textf("unk30: 0x%X", setup->type15.pad30);
                    dbgui_textf("unk34: 0x%X", setup->type15.unk34);
                } else if (setup->unk19 == 0x22) {
                    dbgui_textf("unk30: 0x%X", setup->type22.unk30);
                    dbgui_textf("usedBit: 0x%X", setup->type22.usedBit);
                } else if (setup->unk19 == 0x24) {
                    dbgui_textf("unk30: 0x%X", setup->type24.unk30);
                    dbgui_textf("unk32: 0x%X", setup->type24.unk32);
                } else if (setup->unk19 == 0x26) {
                    dbgui_textf("unk30: 0x%X", setup->type26.unk30);
                    dbgui_textf("unk31: 0x%X", setup->type26.unk31);
                    dbgui_textf("unk32: 0x%X", setup->type26.unk32);
                    dbgui_textf("unk33: 0x%X", setup->type26.unk33);
                    for (s32 k = 0; k < 4; k++) {
                        for (s32 j = 0; j < 4; j++) {
                            dbgui_textf("unk34[%d][%d]: %d", k, j, setup->type26.unk34[k][j]);
                        }
                    }
                }
                
                dbgui_tree_pop();
            } else {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }
            }
        }

        dbgui_end_child();
    }


    for (s32 i = 0; i < _bss_28A8; i++) {
        CurveNode *node = &_bss_8[i];
        CurveSetup *setup = node->setup;

        if (filterByType && setup->unk19 != curveTypeFilter) {
            continue;
        }

        f32 dist = vec3_distance(&camPos, &setup->pos);
        if (dist > curveRenderDist) {
            continue;
        }

        for (s32 k = 0; k < 4; k++) {
            CurveSetup *other = gDLL_26_Curves->vtbl->func_39C(node->setup->unk1C[k]);
            if (other == NULL) {
                continue;
            }

            draw_3d_line(
                setup->pos.x, setup->pos.y, setup->pos.z, 
                other->pos.x, other->pos.y, other->pos.z, 
                hoveredIdx == i ? 0xFFFFFFFF : 0xFFFF00FF);
        }
        
        draw_3d_text(node->setup->pos.x, node->setup->pos.y, node->setup->pos.z, 
            recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X", i, setup->unk19, node->uID),
            hoveredIdx == i ? 0xFFFFFFFF : 0xFF00FFFF);
    }
}
