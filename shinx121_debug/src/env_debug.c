#include "modding.h"
#include "dbgui.h"

#include "debug_common.h"
#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "recomputils.h"

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

// size: 0x318
typedef struct {
/*000*/ s32 unk0;
/*004*/ u16 unk4;
/*006*/ u16 unk6;
/*008*/ s32 unk8;
/*00C*/ s32 unkC;
/*010*/ u8 _unk10[0x14 - 0x10];
/*014*/ s32 fogMin;
/*018*/ s32 fogMax;
/*01C*/ s32 unk1C;
/*020*/ s32 unk20;
/*024*/ s32 fogR;
/*028*/ s32 fogG;
/*02C*/ s32 fogB;
/*030*/ s32 unk30;
/*034*/ s32 unk34;
/*038*/ s32 unk38;
/*03C*/ s32 unk3C;
/*040*/ s32 unk40;
/*044*/ s32 unk44;
/*048*/ s32 unk48;
/*04C*/ u8 _unk4C[0x58 - 0x4C];
/*058*/ f32 unk58;
/*05C*/ f32 unk5C;
/*060*/ f32 unk60;
/*064*/ f32 unk64;
/*068*/ f32 unk68;
/*06C*/ f32 unk6C;
        // 11 values each (11 r, 11 g, 11 b)
/*070*/ f32 colorSpline[33];
/*0F4*/ f32 colorSplineTarget[33];
/*178*/ f32 colorSplineRate[33];
        // 11 values each (11 min, 11 max)
/*1FC*/ f32 posSpline[22];
/*254*/ f32 posSplineTarget[22];
/*2AC*/ f32 posSplineRate[22];
/*304*/ f32 unk304;
/*308*/ f32 unk308;
/*30C*/ f32 unk30C;
/*310*/ f32 unk310; // fog alpha? sorta?
/*314*/ s8 unk314;
/*315*/ s8 unk315;
/*316*/ s8 unk316;
/*317*/ s8 unk317;
} BSS0;

extern BSS0* __dll8_bss_0[2];

static s32 windowOpen = FALSE;

static void fog_tab_state(BSS0* state) {
    dbgui_textf("unk0: %d", state->unk0);
    dbgui_input_ushort_ext("unk4", &state->unk4, &hexInput);
    dbgui_input_ushort_ext("unk6", &state->unk6, &hexInput);
    dbgui_textf("unk8: %d", state->unk8);
    dbgui_textf("unkC: %d", state->unkC);
    dbgui_textf("fogMin: %d", state->fogMin);
    dbgui_textf("fogMax: %d", state->fogMax);
    dbgui_textf("unk1C: %d", state->unk1C);
    dbgui_textf("unk20: %d", state->unk20);
    dbgui_textf("fogR: %d", state->fogR);
    dbgui_textf("fogG: %d", state->fogG);
    dbgui_textf("fogB: %d", state->fogB);
    dbgui_textf("unk30: %d", state->unk30);
    dbgui_textf("unk34: %d", state->unk34);
    dbgui_textf("unk38: %d", state->unk38);
    dbgui_textf("unk3C: %d", state->unk3C);
    dbgui_textf("unk40: %d", state->unk40);
    dbgui_textf("unk44: %d", state->unk44);
    dbgui_textf("unk48: %d", state->unk48);
    dbgui_textf("unk58: %f", state->unk58);
    dbgui_textf("unk5C: %f", state->unk5C);
    dbgui_textf("unk60: %f", state->unk60);
    dbgui_textf("unk64: %f", state->unk64);
    dbgui_textf("unk68: %f", state->unk68);
    dbgui_textf("unk6C: %f", state->unk6C);
    // color spline
    if (dbgui_tree_node("Color Spline (R)")) {
        for (s32 i = 0; i < 11; i++) {
            dbgui_input_float(recomp_sprintf_helper("[%d]", i), &state->colorSpline[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Color Spline (G)")) {
        for (s32 i = 11; i < 22; i++) {
            dbgui_input_float(recomp_sprintf_helper("[%d]", i), &state->colorSpline[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Color Spline (B)")) {
        for (s32 i = 22; i < 33; i++) {
            dbgui_input_float(recomp_sprintf_helper("[%d]", i), &state->colorSpline[i]);
        }
        dbgui_tree_pop();
    }
    // color spline target
    if (dbgui_tree_node("Color Spline Target (R)")) {
        for (s32 i = 0; i < 11; i++) {
            dbgui_textf("[%d] %f", i, state->colorSplineTarget[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Color Spline Target (G)")) {
        for (s32 i = 11; i < 22; i++) {
            dbgui_textf("[%d] %f", i, state->colorSplineTarget[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Color Spline Target (B)")) {
        for (s32 i = 22; i < 33; i++) {
            dbgui_textf("[%d] %f", i, state->colorSplineTarget[i]);
        }
        dbgui_tree_pop();
    }
    // color spline rate
    if (dbgui_tree_node("Color Spline Rate (R)")) {
        for (s32 i = 0; i < 11; i++) {
            dbgui_textf("[%d] %f", i, state->colorSplineRate[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Color Spline Rate (G)")) {
        for (s32 i = 11; i < 22; i++) {
            dbgui_textf("[%d] %f", i, state->colorSplineRate[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Color Spline Rate (B)")) {
        for (s32 i = 22; i < 33; i++) {
            dbgui_textf("[%d] %f", i, state->colorSplineRate[i]);
        }
        dbgui_tree_pop();
    }
    // pos spline
    if (dbgui_tree_node("Position Spline (Min)")) {
        for (s32 i = 0; i < 11; i++) {
            dbgui_input_float(recomp_sprintf_helper("[%d]", i), &state->posSpline[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Position Spline (Max)")) {
        for (s32 i = 11; i < 22; i++) {
            dbgui_input_float(recomp_sprintf_helper("[%d]", i), &state->posSpline[i]);
        }
        dbgui_tree_pop();
    }
    // pos spline target
    if (dbgui_tree_node("Position Spline Target (Min)")) {
        for (s32 i = 0; i < 11; i++) {
            dbgui_textf("[%d] %f", i, state->posSplineTarget[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Position Spline Target (Max)")) {
        for (s32 i = 11; i < 22; i++) {
            dbgui_textf("[%d] %f", i, state->posSplineTarget[i]);
        }
        dbgui_tree_pop();
    }
    // pos spline rate
    if (dbgui_tree_node("Position Spline Rate (Min)")) {
        for (s32 i = 0; i < 11; i++) {
            dbgui_textf("[%d] %f", i, state->posSplineRate[i]);
        }
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("Position Spline Rate (Max)")) {
        for (s32 i = 11; i < 22; i++) {
            dbgui_textf("[%d] %f", i, state->posSplineRate[i]);
        }
        dbgui_tree_pop();
    }
    dbgui_textf("unk304: %f", state->unk304);
    dbgui_textf("unk308: %f", state->unk308);
    dbgui_textf("unk30C: %f", state->unk30C);
    dbgui_textf("unk310: %f", state->unk310);
    dbgui_textf("unk314: %d", state->unk314);
    dbgui_textf("unk315: %d", state->unk315);
    dbgui_textf("unk316: %d", state->unk316);
    dbgui_textf("unk317: %d", state->unk317);
}

static void fog_tab(void) {
    if (dbgui_begin_tab_bar("###fog_state_tabs")) {
        if (dbgui_begin_tab_item("0", NULL)) {
            fog_tab_state(__dll8_bss_0[0]);
            dbgui_end_tab_item();
        }
        if (dbgui_begin_tab_item("1", NULL)) {
            fog_tab_state(__dll8_bss_0[1]);
            dbgui_end_tab_item();
        }
        dbgui_end_tab_bar();
    }
}

void env_debug_menu_callback(void) {
    dbgui_menu_item("Environment", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void env_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (dbgui_begin("Environment Debug", &windowOpen)) {
            if (dbgui_begin_tab_bar("###tabs")) {
                if (dbgui_begin_tab_item("Fog", NULL)) {
                    fog_tab();
                    dbgui_end_tab_item();
                }
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}
