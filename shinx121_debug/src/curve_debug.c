#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "3d.h"
#include "debug_common.h"
#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "dlls/engine/26_curves.h"
#include "sys/curves.h"
#include "sys/math.h"
#include "dll.h"
#include "macros.h"

extern SRT gCameraSRT;
extern f32 gWorldX;
extern f32 gWorldZ;

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

enum CurveDebugFilterType {
    DBG_CURVE_FILTER_NONE,
    DBG_CURVE_FILTER_TYPE,
    DBG_CURVE_FILTER_FLIGHT_GROUP,
    DBG_CURVE_FILTER_UID
};

enum CurveDebugDisplayType {
    DBG_CURVE_DISPLAY_TYPE,
    DBG_CURVE_DISPLAY_FLIGHT_GROUP,
    DBG_CURVE_DISPLAY_CREATE_POINT
};

static const char* filterTypeNames[] = {
    "None",
    "Type",
    "Flight Group",
    "UID"
};


static const char* displayTypeNames[] = {
    "Type",
    "Flight Group",
    "Create Point"
};

static s32 curveDebugWindowOpen = FALSE;
static s32 draw3DCurves = FALSE;
static f32 curveRenderDist = 800;
static s32 filterType = DBG_CURVE_FILTER_NONE;
static s32 filterByDist = FALSE;
static s32 curveTypeFilter = 0;
static s32 flightGroupFilter = 0;
static u32 uidFilter = 0;
static s32 displayType = DBG_CURVE_DISPLAY_TYPE;
static s32 hideLabels = FALSE;
static s32 hoveredIdx = -1;
static u32 hoveredUID = -1;
static Vec3f camPos;

static void debug_draw_curves(void);
static void debug_draw_3d_curves(void);

void curve_debug_menu_callback(void) {
    dbgui_menu_item("Curves", &curveDebugWindowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void curve_debug_dbgui_callback(void) {
    camPos.x = gCameraSRT.transl.x + gWorldX;
    camPos.y = gCameraSRT.transl.y;
    camPos.z = gCameraSRT.transl.z + gWorldZ;

    hoveredIdx = -1;
    hoveredUID = -1;

    if (curveDebugWindowOpen) {
        if (dbgui_begin("Curve Debug", &curveDebugWindowOpen)) {
            debug_draw_curves();
            
        }
        dbgui_end();
    }

    debug_draw_3d_curves();
}

#include "recomp/dlls/engine/26_curves_recomp.h"

extern CurveSetup *_bss_0;
extern CurveSetup *_bss_4;
extern CurveNode _bss_8[1300];
extern s32 _bss_28A8;

static const char* debug_curve_string(s32 index, CurveNode* node, CurveSetup* setup, _Bool isList) {
    if (isList) {
        switch (displayType) {
        case DBG_CURVE_DISPLAY_TYPE:
        default:
            return recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X###%X", index, setup->curveType, node->uID, node->uID);
        case DBG_CURVE_DISPLAY_FLIGHT_GROUP:
            return recomp_sprintf_helper("[%d] Curve (%d) 0x%X###%X", index, setup->base_type22.unk4, node->uID, node->uID);
        case DBG_CURVE_DISPLAY_CREATE_POINT:
            return recomp_sprintf_helper("[%d] Curve (%d) 0x%X###%X", index, setup->type15.unk34, node->uID, node->uID);
        }
    } else {
        switch (displayType) {
        case DBG_CURVE_DISPLAY_TYPE:
        default:
            return recomp_sprintf_helper("[%d] Curve (0x%X) 0x%X", index, setup->curveType, node->uID);
        case DBG_CURVE_DISPLAY_FLIGHT_GROUP:
            return recomp_sprintf_helper("[%d] Curve (%d) 0x%X", index, setup->base_type22.unk4, node->uID);
        case DBG_CURVE_DISPLAY_CREATE_POINT:
            return recomp_sprintf_helper("[%d] Curve (%d) 0x%X", index, setup->type15.unk34, node->uID);
        }
    }
}

static _Bool debug_curve_matches_filter(CurveSetup* setup, _Bool isList) {
    switch (filterType) {
    case DBG_CURVE_FILTER_TYPE:
        if (setup->curveType != curveTypeFilter) {
            return FALSE;
        }
        break;
    case DBG_CURVE_FILTER_FLIGHT_GROUP:
        if (setup->curveType != 0x22 || setup->base_type22.unk4 != flightGroupFilter) {
            return FALSE;
        }
        break;
    case DBG_CURVE_FILTER_UID:
        if (setup->uID != uidFilter) {
            return FALSE;
        }
        break;
    }

    if (filterByDist || !isList) {
        f32 dist = vec3Distance(&camPos, &setup->pos);
        if (dist > curveRenderDist) {
            return FALSE;
        }
    }

    return TRUE;
}

static void debug_draw_curves(void) {
    dbgui_checkbox("Draw 3D Curves", &draw3DCurves);
    if (draw3DCurves) {
        dbgui_same_line();
        dbgui_checkbox("Hide 3D Labels", &hideLabels);
    }
    dbgui_set_next_item_width(120);
    dbgui_input_float("3D Render Distance", &curveRenderDist);
    dbgui_checkbox("Filter List By Distance", &filterByDist);

    dbgui_set_next_item_width(120);
    if (dbgui_begin_combo("Filter By", filterTypeNames[filterType])) {
        for (s32 i = 0; i < ARRAYCOUNT_S(filterTypeNames); i++) {
            if (dbgui_selectable(filterTypeNames[i], filterType == i)) {
                filterType = i;

                if (displayType == DBG_CURVE_DISPLAY_FLIGHT_GROUP && (filterType != DBG_CURVE_FILTER_TYPE || curveTypeFilter != 0x22)) {
                    displayType = DBG_CURVE_DISPLAY_TYPE;
                }
                if (displayType == DBG_CURVE_DISPLAY_CREATE_POINT && (filterType != DBG_CURVE_FILTER_TYPE || curveTypeFilter != 0x15)) {
                    displayType = DBG_CURVE_DISPLAY_TYPE;
                }
            }
        }
        dbgui_end_combo();
    }
    switch (filterType) {
    case DBG_CURVE_FILTER_TYPE:
        dbgui_set_next_item_width(120);
        dbgui_input_int_ext("Type", &curveTypeFilter, &hexInput);
        break;
    case DBG_CURVE_FILTER_FLIGHT_GROUP:
        dbgui_set_next_item_width(120);
        dbgui_input_int("Flight Group", &flightGroupFilter);
        break;
    case DBG_CURVE_FILTER_UID:
        dbgui_set_next_item_width(120);
        dbgui_input_uint_ext("UID", &uidFilter, &hexInput);
        break;
    }

    dbgui_set_next_item_width(120);
    if (dbgui_begin_combo("Display", displayTypeNames[displayType])) {
        for (s32 i = 0; i < ARRAYCOUNT_S(displayTypeNames); i++) {
            if (dbgui_selectable(displayTypeNames[i], displayType == i)) {
                displayType = i;

                if (displayType == DBG_CURVE_DISPLAY_FLIGHT_GROUP && (filterType != DBG_CURVE_FILTER_TYPE || curveTypeFilter != 0x22)) {
                    filterType = DBG_CURVE_FILTER_TYPE;
                    curveTypeFilter = 0x22;
                }
                if (displayType == DBG_CURVE_DISPLAY_CREATE_POINT && (filterType != DBG_CURVE_FILTER_TYPE || curveTypeFilter != 0x15)) {
                    filterType = DBG_CURVE_FILTER_TYPE;
                    curveTypeFilter = 0x15;
                }
            }
        }
        dbgui_end_combo();
    }

    dbgui_textf("Loaded curves (%d):", _bss_28A8);
    if (dbgui_begin_child("curves")) {
        for (s32 i = 0; i < _bss_28A8; i++) {
            CurveNode *node = &_bss_8[i];
            CurveSetup *setup = node->setup;

            if (!debug_curve_matches_filter(setup, TRUE)) {
                continue;
            }

            if (dbgui_tree_node(debug_curve_string(i, node, setup, TRUE))) {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                    hoveredUID = setup->uID;
                }

                dbgui_push_item_width(140);
                dbgui_textf("objId: 0x%X", setup->objId);
                dbgui_textf("quarterSize: 0x%X", setup->quarterSize);
                dbgui_input_byte_ext("unk3", &setup->unk3, &hexInput);
                if (setup->curveType == 0x22) {
                    dbgui_input_ushort_ext("flightGroup", &setup->base_type22.unk4, &hexInput);
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
                    dbgui_input_short_ext("createPointID", &setup->type15.unk34, &hexInput);
                } else if (setup->curveType == 0x1D) {
                    dbgui_input_byte_ext("_unk30", &setup->type1D._unk30[0], &hexInput);
                    dbgui_input_byte_ext("_unk31", &setup->type1D._unk30[1], &hexInput);
                    dbgui_input_byte_ext("_unk32", &setup->type1D._unk30[2], &hexInput);
                    dbgui_input_byte_ext("_unk33", &setup->type1D._unk30[3], &hexInput);
                    dbgui_input_byte_ext("unk34", &setup->type1D.unk34, &hexInput);
                } else if (setup->curveType == 0x1E) {
                    dbgui_input_byte_ext("_unk30", &setup->type1D._unk30[0], &hexInput);
                    dbgui_input_byte_ext("_unk31", &setup->type1D._unk30[1], &hexInput);
                    dbgui_input_byte_ext("_unk32", &setup->type1D._unk30[2], &hexInput);
                    dbgui_input_byte_ext("_unk33", &setup->type1D._unk30[3], &hexInput);
                    dbgui_input_short_ext("unk34[0]", &((s16*)&setup->type1D.unk34)[0], &hexInput);
                    dbgui_input_short_ext("unk34[1]", &((s16*)&setup->type1D.unk34)[1], &hexInput);
                    dbgui_input_short_ext("unk34[2]", &((s16*)&setup->type1D.unk34)[2], &hexInput);
                    dbgui_input_short_ext("unk34[3]", &((s16*)&setup->type1D.unk34)[3], &hexInput);
                } else if (setup->curveType == 0x22) {
                    dbgui_input_short_ext("activeBit", &setup->type22.activeBit, &hexInput);
                    dbgui_input_short_ext("usedBit", &setup->type22.usedBit, &hexInput);
                } else if (setup->curveType == 0x24) {
                    dbgui_input_short_ext("activeBit", &setup->type24.activeBit, &hexInput);
                    dbgui_input_short_ext("usedBit", &setup->type24.usedBit, &hexInput);
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
                    hoveredUID = setup->uID;
                }
            }
        }
    }
    dbgui_end_child();
}

static void debug_draw_3d_curves(void) {
    if (!draw3DCurves) {
        return;
    }
    
    for (s32 i = 0; i < _bss_28A8; i++) {
        CurveNode *node = &_bss_8[i];
        CurveSetup *setup = node->setup;

        if (!debug_curve_matches_filter(setup, FALSE)) {
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
                (hoveredIdx == i || other->uID == hoveredUID) ? 0xFFFFFFFF : 0xFFFF00FF);
        }
        
        if (!hideLabels) {
            draw_3d_text(node->setup->pos.x, node->setup->pos.y, node->setup->pos.z, 
                debug_curve_string(i, node, setup, FALSE),
                hoveredIdx == i ? 0xFFFFFFFF : 0xFF00FFFF);
        }
    }
}
