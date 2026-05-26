#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"
#include "3d.h"
#include "debug_common.h"

#include "PR/ultratypes.h"
#include "dlls/engine/26_curves.h"
#include "sys/curves.h"
#include "sys/math.h"
#include "dll.h"

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

static void debug_draw_curves(void) {
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

            if (filterByType && setup->curveType != curveTypeFilter) {
                continue;
            }

            if (filterByDist) {
                f32 dist = vec3_distance(&camPos, &setup->pos);
                if (dist > curveRenderDist) {
                    continue;
                }
            }

            if (dbgui_tree_node(recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X###%X", i, setup->curveType, node->uID, node->uID))) {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }

                dbgui_push_item_width(140);
                dbgui_textf("objId: 0x%X", setup->objId);
                dbgui_textf("quarterSize: 0x%X", setup->quarterSize);
                dbgui_input_byte_ext("unk3", &setup->unk3, &hexInput);
                if (setup->curveType == 0x22) {
                    dbgui_input_ushort_ext("unk4", &setup->base_type22.unk4, &hexInput);
                    dbgui_input_sbyte_ext("unk6", &setup->base_type22.unk6, &hexInput);
                    dbgui_input_sbyte_ext("unk7", &setup->base_type22.unk7, &hexInput);
                } else if (setup->curveType == 0x26) {
                    dbgui_input_sbyte_ext("unk4", &setup->base_type26.unk4, &hexInput);
                    dbgui_input_sbyte_ext("unk5", &setup->base_type26.unk5, &hexInput);
                    dbgui_input_sbyte_ext("unk6", &setup->base_type26.unk6, &hexInput);
                    dbgui_input_sbyte_ext("unk7", &setup->base_type26.unk7, &hexInput);
                }
                dbgui_input_float("x", &setup->pos.x);
                dbgui_input_float("y", &setup->pos.y);
                dbgui_input_float("z", &setup->pos.z);
                dbgui_textf("uID: 0x%X", setup->uID);

                dbgui_input_sbyte_ext("unk18", &setup->unk18, &hexInput);
                dbgui_textf("curveType: 0x%X", setup->curveType);
                dbgui_input_sbyte_ext("unk1A", &setup->unk1A, &hexInput);
                dbgui_input_sbyte_ext("unk1B", &setup->unk1B, &hexInput);
                for (s32 k = 0; k < 4; k++) {
                    dbgui_input_int_ext(recomp_sprintf_helper("links[%d]", k), &setup->links[k], &hexInput);
                }
                dbgui_input_sbyte_ext("unk2C", &setup->unk2C, &hexInput);
                dbgui_input_sbyte_ext("unk2D", &setup->unk2D, &hexInput);
                dbgui_input_byte_ext("unk2E", &setup->unk2E, &hexInput);
                dbgui_input_sbyte_ext("unk2F", &setup->unk2F, &hexInput);
                if (setup->curveType == 0x15) {
                    dbgui_input_int_ext("unk30", &setup->type15.pad30, &hexInput);
                    dbgui_input_short_ext("unk34", &setup->type15.unk34, &hexInput);
                } else if (setup->curveType == 0x1D) {
                    dbgui_input_byte_ext("_unk30", &setup->type1D._unk30[0], &hexInput);
                    dbgui_input_byte_ext("_unk31", &setup->type1D._unk30[1], &hexInput);
                    dbgui_input_byte_ext("_unk32", &setup->type1D._unk30[2], &hexInput);
                    dbgui_input_byte_ext("_unk33", &setup->type1D._unk30[3], &hexInput);
                    dbgui_input_byte_ext("unk34", &setup->type1D.unk34, &hexInput);
                } else if (setup->curveType == 0x22) {
                    dbgui_input_short_ext("unk30", &setup->type22.unk30, &hexInput);
                    dbgui_input_short_ext("usedBit", &setup->type22.usedBit, &hexInput);
                } else if (setup->curveType == 0x24) {
                    dbgui_input_short_ext("unk30", &setup->type24.unk30, &hexInput);
                    dbgui_input_short_ext("unk32", &setup->type24.unk32, &hexInput);
                } else if (setup->curveType == 0x26) {
                    dbgui_input_sbyte_ext("unk30", &setup->type26.unk30, &hexInput);
                    dbgui_input_sbyte_ext("unk31", &setup->type26.unk31, &hexInput);
                    dbgui_input_sbyte_ext("unk32", &setup->type26.unk32, &hexInput);
                    dbgui_input_sbyte_ext("unk33", &setup->type26.unk33, &hexInput);
                    for (s32 k = 0; k < 4; k++) {
                        for (s32 j = 0; j < 4; j++) {
                            dbgui_input_sbyte(recomp_sprintf_helper("unk34[%d][%d]: %d", k, j), &setup->type26.unk34[k][j]);
                        }
                    }
                }
                dbgui_pop_item_width();
                
                dbgui_tree_pop();
            } else {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }
            }
        }
    }
    dbgui_end_child();


    for (s32 i = 0; i < _bss_28A8; i++) {
        CurveNode *node = &_bss_8[i];
        CurveSetup *setup = node->setup;

        if (filterByType && setup->curveType != curveTypeFilter) {
            continue;
        }

        f32 dist = vec3_distance(&camPos, &setup->pos);
        if (dist > curveRenderDist) {
            continue;
        }

        for (s32 k = 0; k < 4; k++) {
            CurveSetup *other = gDLL_26_Curves->vtbl->func_39C(node->setup->links[k]);
            if (other == NULL) {
                continue;
            }

            draw_3d_line(
                setup->pos.x, setup->pos.y, setup->pos.z, 
                other->pos.x, other->pos.y, other->pos.z, 
                hoveredIdx == i ? 0xFFFFFFFF : 0xFFFF00FF);
        }
        
        draw_3d_text(node->setup->pos.x, node->setup->pos.y, node->setup->pos.z, 
            recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X", i, setup->curveType, node->uID),
            hoveredIdx == i ? 0xFFFFFFFF : 0xFF00FFFF);
    }
}
