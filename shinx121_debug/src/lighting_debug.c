#include "modding.h"
#include "dbgui.h"

#include "debug_common.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dll.h"
#include "sys/segment_1D900.h"

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
extern u8 D_80090CA4;
extern s32 D_80090CA8;
extern f32 D_80090CAC;
extern u8 BYTE_80090cb0;
extern f32 D_80090CB4;

extern u8 D_80090D18;
extern u8 D_80090D1C;
extern u8 D_80090D20;
extern Vec3s32 D_80090D24;

extern u8 D_800B1830;
extern u8 D_800B1831;
extern u8 D_800B1832;
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

extern s16 D_800B1850;
extern s16 D_800B1852;
extern s16 D_800B1854;

static s32 windowOpen = FALSE;

RECOMP_CALLBACK(".", my_debug_menu_event) void lighting_debug_menu_callback() {
    dbgui_menu_item("Lighting", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void lighting_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Lighting Debug", &windowOpen)) {
            dbgui_textf("D_800B1830: %d", D_800B1830);
            dbgui_textf("D_800B1831: %d", D_800B1831);
            dbgui_textf("D_800B1832: %d", D_800B1832);

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

            dbgui_textf("D_800B1850: %d", D_800B1850);
            dbgui_textf("D_800B1852: %d", D_800B1852);
            dbgui_textf("D_800B1854: %d", D_800B1854);

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
            dbgui_textf("D_80090CA4: %d", D_80090CA4);

            dbgui_textf("D_80090CA8: %d", D_80090CA8);
            dbgui_textf("D_80090CAC: %f", D_80090CAC);
            dbgui_input_byte("BYTE_80090cb0", &BYTE_80090cb0);
            dbgui_textf("D_80090CB4: %f", D_80090CB4);

            dbgui_textf("D_80090D18: %d", D_80090D18);
            dbgui_textf("D_80090D1C: %d", D_80090D1C);
            dbgui_textf("D_80090D20: %d", D_80090D20);

            dbgui_textf("D_80090D24: %d, %d, %d", D_80090D24.x, D_80090D24.y, D_80090D24.z);
        }
        dbgui_end();
    }
}
