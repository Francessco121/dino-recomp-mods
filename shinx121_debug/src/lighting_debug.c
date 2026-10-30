#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_common.h"
#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dll.h"
#include "sys/lfx.h"
#include "sys/lighting.h"
#include "sys/pi.h"

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

extern f32 D_80090C60;
extern f32 D_80090C64;
extern u8 D_80090C68;
extern u8 D_80090C6C;
extern u8 D_80090C70;
extern u8 D_80090C74;
extern u8 D_80090C78;
extern u8 D_80090C7C;
extern u8 D_80090C80;
extern u8 D_80090C84;
extern u8 D_80090C88;
extern u8 D_80090C8C;
extern u8 D_80090C90;
extern u8 D_80090C94;
extern u8 D_80090C98;
extern s8 D_80090C9C;
extern u8 D_80090CA0;
extern u8 gAmbientAdditiveSet;
extern s32 D_80090CA8;
extern f32 D_80090CAC;
extern u8 gInsideLighting;
extern f32 gInsideLightT;

extern u8 D_80090D18;
extern u8 D_80090D1C;
extern u8 D_80090D20;
extern Vec3s32 D_80090D24;

extern u8 gAmbientR;
extern u8 gAmbientG;
extern u8 gAmbientB;
extern f32 D_800B1834;
extern f32 D_800B1838;
extern f32 D_800B183C;
extern s8 D_800B1840;
extern s8 D_800B1841;
extern u8 D_800B1842;
extern u8 D_800B1843;
extern u8 D_800B1844;
extern u8 D_800B1845;
extern s8 D_800B1846;
extern u8 D_800B1847;

extern s16 gAmbientAdditiveR;
extern s16 gAmbientAdditiveG;
extern s16 gAmbientAdditiveB;

extern u8 *D_800B1848;
extern Unk800B1858 *D_800B1858;

static s32 windowOpen = FALSE;

static void recomp_debug_57_tab(void);
static void recomp_debug_58_tab(void);

static void core_tab(void) {
    dbgui_textf("gAmbientR: %d", gAmbientR);
    dbgui_textf("gAmbientG: %d", gAmbientG);
    dbgui_textf("gAmbientB: %d", gAmbientB);

    dbgui_textf("D_800B1834: %f", D_800B1834);
    dbgui_textf("D_800B1838: %f", D_800B1838);
    dbgui_textf("D_800B183C: %f", D_800B183C);

    dbgui_textf("D_800B1840: %d", D_800B1840);
    dbgui_textf("D_800B1841: %d", D_800B1841);
    dbgui_textf("D_800B1842: %d", D_800B1842);
    dbgui_textf("D_800B1843: %d", D_800B1843);
    dbgui_textf("D_800B1844: %d", D_800B1844);
    dbgui_textf("D_800B1845: %d", D_800B1845);
    dbgui_textf("D_800B1846: %d", D_800B1846);
    dbgui_textf("D_800B1847: %d", D_800B1847);

    dbgui_textf("gAmbientAdditiveR: %d", gAmbientAdditiveR);
    dbgui_textf("gAmbientAdditiveG: %d", gAmbientAdditiveG);
    dbgui_textf("gAmbientAdditiveB: %d", gAmbientAdditiveB);

    dbgui_separator();

    dbgui_textf("D_80090C60: %f", D_80090C60);
    dbgui_textf("D_80090C64: %f", D_80090C64);

    dbgui_textf("D_80090C68: %d", D_80090C68);
    dbgui_textf("D_80090C6C: %d", D_80090C6C);
    dbgui_textf("D_80090C70: %d", D_80090C70);
    dbgui_textf("D_80090C74: %d", D_80090C74);
    dbgui_textf("D_80090C78: %d", D_80090C78);
    dbgui_textf("D_80090C7C: %d", D_80090C7C);
    dbgui_textf("D_80090C80: %d", D_80090C80);
    dbgui_textf("D_80090C84: %d", D_80090C84);
    dbgui_textf("D_80090C88: %d", D_80090C88);
    dbgui_textf("D_80090C8C: %d", D_80090C8C);
    dbgui_textf("D_80090C90: %d", D_80090C90);
    dbgui_textf("D_80090C94: %d", D_80090C94);
    dbgui_textf("D_80090C98: %d", D_80090C98);
    dbgui_textf("D_80090C9C: %d", D_80090C9C);
    dbgui_textf("D_80090CA0: %d", D_80090CA0);
    dbgui_textf("gAmbientAdditiveSet: %d", gAmbientAdditiveSet);

    dbgui_textf("D_80090CA8: %d", D_80090CA8);
    dbgui_textf("D_80090CAC: %f", D_80090CAC);
    dbgui_input_byte("gInsideLighting", &gInsideLighting);
    dbgui_textf("gInsideLightT: %f", gInsideLightT);

    dbgui_textf("D_80090D18: %d", D_80090D18);
    dbgui_textf("D_80090D1C: %d", D_80090D1C);
    dbgui_textf("D_80090D20: %d", D_80090D20);

    dbgui_textf("D_80090D24: %d, %d, %d", D_80090D24.x, D_80090D24.y, D_80090D24.z);

    dbgui_separator_text("gModLightPalettes");

    for (s32 i = 0; i < 17; i++) {
        u8* colors = &D_800B1848[(256 * 3) * i];

        u8 r1 = colors[0];
        u8 g1 = colors[1];
        u8 b1 = colors[2];
        u8 r2 = colors[(256 * 3) - 3];
        u8 g2 = colors[(256 * 3) - 2];
        u8 b2 = colors[(256 * 3) - 1];

        switch (i - 1) {
            case -1:
                dbgui_text("base skylight:");
                break;
            case 0:
                dbgui_text("(unknown):");
                break;
            case 4:
                dbgui_text("newlfx:");
                break;
            case 11:
                dbgui_text("skylight:");
                break;
            case 15:
                dbgui_text("(unknown):");
                break;
        }

        dbgui_textf("[%d]: c1 %d,%d,%d | c2 %d,%d,%d", i - 1,
            r1, g1, b1, r2, g2, b2);
    }

    dbgui_separator();

    for (s32 i = 0; i < 30; i++) {
        if (dbgui_tree_node(recomp_sprintf_helper("D_800B1858[%d]", i))) {
            Unk800B1858* el = &D_800B1858[i];
            dbgui_textf("x: %d", el->x);
            dbgui_textf("y: %d", el->y);
            dbgui_textf("z: %d", el->z);
            dbgui_textf("unk6: %d", el->unk6);
            dbgui_textf("unk7: %d", el->unk7);
            dbgui_textf("unk8: %d", el->unk8);
            dbgui_textf("unk9: %d", el->unk9);
            dbgui_textf("unkA: %d", el->unkA);
            dbgui_textf("unkB: %d", el->unkB);
            dbgui_textf("unkC: %d", el->unkC);
            dbgui_textf("unkD: %d", el->unkD);
            dbgui_textf("unkE: %d", el->unkE);
            dbgui_textf("unkF: %d", el->unkF);
            dbgui_textf("pos: %f, %f, %f", el->pos.x, el->pos.y, el->pos.z);
            
            dbgui_tree_pop();
        }
    }
}

static void laction_setter(void) {
    static s32 index = 0;
    static s32 totalActions = -1;
    static LightAction action;

    if (totalActions == -1) {
        totalActions = piRomGetFileSize(LACTIONS_BIN) / sizeof(LightAction);
        piRomLoadSection(LACTIONS_BIN, &action, index * sizeof(LightAction), sizeof(LightAction));
    }

    dbgui_set_next_item_width(100);
    if (dbgui_input_int("Action", &index)) {
        if (index < 0) index = 0;
        if (index >= totalActions) index = totalActions - 1;
        piRomLoadSection(LACTIONS_BIN, &action, index * sizeof(LightAction), sizeof(LightAction));
    }

    dbgui_same_line();
    if (dbgui_button("Apply")) {
        lfxAction(NULL, NULL, index, 0, 0, NULL);
    }

    if (dbgui_tree_node("Action Details")) {
        dbgui_textf("timer: %d", action.unk0);
        dbgui_textf("unk2: %d", action.unk2);
        dbgui_textf("x: %d", action.unk4);
        dbgui_textf("y: %d", action.unk6);
        dbgui_textf("z: %d", action.unk8);
        dbgui_textf("unka: %d", action.unka);
        dbgui_textf("unkC: %d", action.unkC);
        dbgui_textf("id: %d", action.unke);
        dbgui_textf("slot: %d", action.unk10);
        dbgui_textf("flags: 0x%X", action.unk12);
        dbgui_textf("flags2: 0x%X", action.unk13);
        dbgui_textf("flags3: 0x%X", action.unk14);
        dbgui_textf("r1: %d", action.unk15);
        dbgui_textf("g1: %d", action.unk16);
        dbgui_textf("b1: %d", action.unk17);
        dbgui_textf("r2: %d", action.unk18);
        dbgui_textf("g2: %d", action.unk19);
        dbgui_textf("b2: %d", action.unk1a);
        dbgui_textf("type: %d", action.unk1b);
        dbgui_textf("unk1c: %d (bool)", action.unk1c);
        dbgui_textf("max: %d", action.unk1d);
        dbgui_textf("min: %d", action.unk1e);
        dbgui_textf("maxRange: %d", action.unk1f);
        dbgui_textf("minRange: %d", action.unk20);
        dbgui_textf("unk21: %d", action.unk21);
        dbgui_textf("speed: %d", action.unk22);
        dbgui_textf("unk23: %d (something rnd max)", action.unk23);
        dbgui_tree_pop();
    }
}

void lighting_debug_menu_callback(void) {
    dbgui_menu_item("Lighting", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void lighting_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Lighting Debug", &windowOpen)) {
            if (dbgui_tree_node("Light Actions")) {
                laction_setter();
                dbgui_tree_pop();
            }
            if (dbgui_begin_tab_bar("tabs")) {
                if (dbgui_begin_tab_item("Core", NULL)) {
                    core_tab();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("DLL 57", NULL)) {
                    recomp_debug_57_tab();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("DLL 58", NULL)) {
                    recomp_debug_58_tab();
                    dbgui_end_tab_item();
                }
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}

RECOMP_HOOK("lfxAction") void lfxAction_hook(Object* arg0, Object* arg1, u16 actionIndex, s8 arg3, s32 arg4, SRT* arg5) {
    const char* arg0Name = arg0 == NULL ? "(null)" : (arg0->def == NULL ? "(unk)" : arg0->def->name);
    const char* arg1Name = arg1 == NULL ? "(null)" : (arg1->def == NULL ? "(unk)" : arg1->def->name);
    
    recomp_printf("lfxAction(%s, %s, %d, %d, %d, %p)\n", arg0Name, arg1Name, actionIndex, arg3, arg4, arg5);
}

#include "recomp/dlls/engine/57_recomp.h"

typedef struct {
    s32 r;
    s32 g;
    s32 b;
} Rgb32;

extern Rgb32 data_0[2];
extern f32 sTValue[2];

extern Rgb32 bss_0[2];
extern Rgb32 bss_18[2];

static void recomp_debug_57_tab(void) {
    dbgui_textf("data_0[0]: %d,%d,%d", data_0[0].r, data_0[0].g, data_0[0].b);
    dbgui_textf("data_0[1]: %d,%d,%d", data_0[1].r, data_0[1].g, data_0[1].b);

    dbgui_textf("sTValue[0]: %f", sTValue[0]);
    dbgui_textf("sTValue[1]: %f", sTValue[1]);

    dbgui_textf("bss_0[0]: %d,%d,%d", bss_0[0].r, bss_0[0].g, bss_0[0].b);
    dbgui_textf("bss_0[1]: %d,%d,%d", bss_0[1].r, bss_0[1].g, bss_0[1].b);

    dbgui_textf("bss_18[0]: %d,%d,%d", bss_18[0].r, bss_18[0].g, bss_18[0].b);
    dbgui_textf("bss_18[1]: %d,%d,%d", bss_18[1].r, bss_18[1].g, bss_18[1].b);

    if (dbgui_tree_node("Edit")) {
        static LightAction action = {
            .unk10 = 0,
            .unk12 = 1,
            .unk1b = 2,
            .unk1e = 0 // none
        };

        dbgui_push_item_width(120);

        if (dbgui_tree_node("Copy")) {
            static s32 copyIdx = 0;
            dbgui_input_int("Action", &copyIdx);

            if (dbgui_button("Copy From")) {
                s32 totalActions = piRomGetFileSize(LACTIONS_BIN) / sizeof(LightAction);
                if (copyIdx >= 0 && copyIdx < totalActions) {
                    LightAction loadedAction;
                    piRomLoadSection(LACTIONS_BIN, &loadedAction, copyIdx * sizeof(LightAction), sizeof(LightAction));
                    if (loadedAction.unk1b == 2) {
                        bcopy(&loadedAction, &action, sizeof(action));
                        action.unk1e = 0;
                    }
                }
            }
            dbgui_tree_pop();
        }

        if (dbgui_input_ushort("unk10  slot", &action.unk10)) {
            if (action.unk10 > 10) action.unk10 = 10;
        }
        dbgui_input_byte_ext("unk12  flags", &action.unk12, &hexInput);
        dbgui_separator();
        dbgui_input_byte("unk15  r1", &action.unk15);
        dbgui_input_byte("unk16  g1", &action.unk16);
        dbgui_input_byte("unk17  b1", &action.unk17);
        dbgui_separator();
        dbgui_input_byte("unk18  r2", &action.unk18);
        dbgui_input_byte("unk19  g2", &action.unk19);
        dbgui_input_byte("unk1a  b2", &action.unk1a);
        dbgui_separator();
        dbgui_input_byte("unk1d  max", &action.unk1d);
        dbgui_input_byte("unk1e  min", &action.unk1e);
        dbgui_input_byte("unk22  speed", &action.unk22);
        dbgui_input_byte("unk23", &action.unk23);
        dbgui_pop_item_width();

        if (dbgui_button("Apply")) {
            gDLL_57->vtbl->func1(&action);
        }
    }
}

#include "recomp/dlls/engine/58_recomp.h"

typedef struct {
    u8 _unk0[0x8 - 0x0];
    f32 tValue;
    f32 rate;
    u8 r1;
    u8 g1;
    u8 b1;
    u8 r2;
    u8 g2;
    u8 b2;
    u8 _unk16[0x18 - 0x16];
} UnkLight;

extern UnkLight bss_0[4 * 2];

static void recomp_debug_58_tab(void) {
    for (s32 i = 0; i < 8; i++) {
        if (dbgui_tree_node(recomp_sprintf_helper("bss_0[%d]", i))) {
            UnkLight* light = &bss_0[i];
            dbgui_textf("tValue: %f", light->tValue);
            dbgui_textf("rate: %f", light->rate);
            dbgui_textf("r1: %d", light->r1);
            dbgui_textf("g1: %d", light->g1);
            dbgui_textf("b1: %d", light->b1);
            dbgui_textf("r2: %d", light->r2);
            dbgui_textf("g2: %d", light->g2);
            dbgui_textf("b2: %d", light->b2);
            
            dbgui_tree_pop();
        }
    }

    if (dbgui_tree_node("Edit")) {
        static LightAction action = {
            .unk10 = 0,
            .unk14 = 1,
            .unk1b = 5,
            .unk1e = 0 // none
        };

        dbgui_push_item_width(120);

        if (dbgui_tree_node("Copy")) {
            static s32 copyIdx = 0;
            dbgui_input_int("Action", &copyIdx);

            if (dbgui_button("Copy From")) {
                s32 totalActions = piRomGetFileSize(LACTIONS_BIN) / sizeof(LightAction);
                if (copyIdx >= 0 && copyIdx < totalActions) {
                    LightAction loadedAction;
                    piRomLoadSection(LACTIONS_BIN, &loadedAction, copyIdx * sizeof(LightAction), sizeof(LightAction));
                    if (loadedAction.unk1b == 5) {
                        bcopy(&loadedAction, &action, sizeof(action));
                        action.unk1e = 0;
                    }
                }
            }
            dbgui_tree_pop();
        }

        dbgui_input_ushort("timer", &action.unk0);
        if (dbgui_input_ushort("slot", &action.unk10)) {
            if (action.unk10 > 3) action.unk10 = 3;
        }
        dbgui_input_byte_ext("flags3", &action.unk14, &hexInput);
        dbgui_separator();
        dbgui_input_byte("r1", &action.unk15);
        dbgui_input_byte("g1", &action.unk16);
        dbgui_input_byte("b1", &action.unk17);
        dbgui_separator();
        dbgui_input_byte("r2", &action.unk18);
        dbgui_input_byte("g2", &action.unk19);
        dbgui_input_byte("b2", &action.unk1a);
        dbgui_pop_item_width();

        if (dbgui_button("Apply")) {
            gDLL_58->vtbl->func1(&action);
        }
    }
}
