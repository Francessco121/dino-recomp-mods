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
                f32 dist = vec3_distance(&camPos, (Vec3f*)&setup->base.x);
                if (dist > curveRenderDist) {
                    continue;
                }
            }

            if (dbgui_tree_node(recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X###%X", i, setup->unk19, node->uID, node->uID))) {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }

                dbgui_textf("objId: 0x%X", setup->base.objId);
                dbgui_textf("quarterSize: 0x%X", setup->base.quarterSize);
                dbgui_textf("setup: 0x%X", setup->base.setup);
                if (setup->unk19 == 0x22) {
                    dbgui_textf("unk4: 0x%X", setup->base.curve_type22.unk4);
                    dbgui_textf("unk6: 0x%X", setup->base.curve_type22.unk6);
                    dbgui_textf("unk7: 0x%X", setup->base.curve_type22.unk7);
                } else if (setup->unk19 == 0x26) {
                    dbgui_textf("unk4: 0x%X", setup->base.curve_type26.unk4);
                    dbgui_textf("unk5: 0x%X", setup->base.curve_type26.unk5);
                    dbgui_textf("unk6: 0x%X", setup->base.curve_type26.unk6);
                    dbgui_textf("unk7: 0x%X", setup->base.curve_type26.unk7);
                }
                dbgui_textf("x: %f", setup->base.x);
                dbgui_textf("y: %f", setup->base.y);
                dbgui_textf("z: %f", setup->base.z);
                dbgui_textf("uID: 0x%X", setup->base.uID);

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
                    dbgui_textf("unk30: 0x%X", setup->type15.unk30);
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
                    dbgui_textf("unk34: 0x%X", setup->type26.unk34);
                    dbgui_textf("unk35: 0x%X", setup->type26.unk35);
                    dbgui_textf("unk36: 0x%X", setup->type26.unk36);
                    dbgui_textf("unk37: 0x%X", setup->type26.unk37);
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

        f32 dist = vec3_distance(&camPos, (Vec3f*)&setup->base.x);
        if (dist > curveRenderDist) {
            continue;
        }

        for (s32 k = 0; k < 4; k++) {
            CurveSetup *other = gDLL_26_Curves->vtbl->curves_func_39c(node->setup->unk1C[k]);
            if (other == NULL) {
                continue;
            }

            draw_3d_line(
                setup->base.x, setup->base.y, setup->base.z, 
                other->base.x, other->base.y, other->base.z, 
                hoveredIdx == i ? 0xFFFFFFFF : 0xFFFF00FF);
        }
        
        draw_3d_text(node->setup->base.x, node->setup->base.y, node->setup->base.z, 
            recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X", i, setup->unk19, node->uID),
            hoveredIdx == i ? 0xFFFFFFFF : 0xFF00FFFF);
    }
}
