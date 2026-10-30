#include "dll.h"
#include "modding.h"
#include "dbgui.h"

#include "debug_menus.h"
#include "debug_common.h"

#include "PR/ultratypes.h"
#include "dlls/engine/7_newday.h"

static const DbgUiInputIntOptions hourInput = {
    .step = 1,
    .stepFast = 6
};
static const DbgUiInputIntOptions minuteInput = {
    .step = 1,
    .stepFast = 10
};
static const DbgUiInputIntOptions nostepInput = {
    .step = 0,
    .stepFast = 0
};


#define DAYTIME 18000.0f //5am
#define NIGHTTIME 75600.0f //9pm
#define NOON 12.0 * 60.0 * 60.0
#define SECONDS_IN_DAY 86400.0f
#define SECONDS_IN_HOUR (60 * 60)
#define SECONDS_IN_MINUTE (60)
#define TICKS_IN_HOUR (SECONDS_IN_HOUR * 60)
#define TICKS_IN_MINUTE (SECONDS_IN_MINUTE * 60)
#define TICKS_IN_SECOND (60)

static s32 windowOpen = FALSE;

static void recomp_time_debug(void);

void time_debug_menu_callback(void) {
    dbgui_menu_item("Time", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void time_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (dbgui_begin("Time Debug", &windowOpen)) {
            recomp_time_debug();
        }
        dbgui_end();
    }
}

#include "recomp/dlls/engine/7_newday_recomp.h"

// size: 0x118
typedef struct {
/*000*/ DLTri *unk0;
/*004*/ Vtx *unk4;
/*008*/ Texture *unk8;
/*00C*/ Texture *unkC;
/*010*/ Texture *unk10;
/*014*/ Texture *unk14;
/*018*/ Texture *unk18;
/*01C*/ Texture *unk1C;
/*020*/ s32 unk20;
/*024*/ u8 _unk20[8];
/*02C*/ f32 unk2C;
/*030*/ f32 unk30;
/*034*/ f32 unk34;
/*038*/ u8 _unk38[8];
/*040*/ f32 unk40;
/*044*/ f32 unk44[3][7]; // splines
/*098*/ u8 _unk98[0xc0 - 0x98];
/*0C0*/ f32 timeSeconds; //time of day (seconds)
/*0C4*/ f32 unkC4;
/*0C8*/ f32 unkC8;
/*0CC*/ s32 unkCC;
/*0D0*/ s32 unkD0;
/*0D4*/ s32 unkD4;
/*0D8*/ s32 unkD8;
/*0DC*/ s32 unkDC;
/*0E0*/ s32 unkE0;
/*0E4*/ s32 unkE4;
/*0E8*/ s32 unkE8;
/*0EC*/ s32 unkEC[8];
/*10C*/ s32 unk10C;
/*110*/ u8 unk110;
/*111*/ u8 unk111;
/*112*/ u8 unk112;
/*113*/ u8 unk113;
/*114*/ u8 unk114;
/*115*/ u8 _unk115[3];
} NewDayStruct;

extern s32 data_4;
extern f32 data_5C;

extern NewDayStruct* bss_30[2];

static s32 inputMinute;
static s32 inputHour;
static s32 inputTimeSeconds;

static void recomp_time_debug(void) {
    f32 time = 0;
    gDLL_7_Newday->vtbl->func4(&time);

    s16 timeHour;
    s16 timeMinute;
    s16 timeSecond;
    gDLL_7_Newday->vtbl->convert_ticks_to_real_time(time * TICKS_IN_SECOND, &timeHour, &timeMinute, &timeSecond);

    dbgui_textf("Time: %02d:%02d:%02d (%.2f)", timeHour, timeMinute, timeSecond, time);

    f32 func8Time;
    s32 night = gDLL_7_Newday->vtbl->func8(&func8Time);

    dbgui_textf("Nighttime: %s", night ? "Yes" : "No");

    if (dbgui_button("Set To Day")) {
        gDLL_7_Newday->vtbl->func9(DAYTIME);
    }
    dbgui_same_line();
    if (dbgui_button("Set To Noon")) {
        gDLL_7_Newday->vtbl->func9(NOON);
    }
    if (dbgui_button("Set To Sunset")) {
        gDLL_7_Newday->vtbl->func9(65000.0f);
    }
    dbgui_same_line();
    if (dbgui_button("Set To Night")) {
        gDLL_7_Newday->vtbl->func9(NIGHTTIME);
    }

    dbgui_set_next_item_width(80);
    if (dbgui_input_int_ext("##hour", &inputHour, &hourInput)) {
        if (inputHour < 0) inputHour = 23;
        if (inputHour > 23) inputHour = 0;
    }
    dbgui_same_line();
    dbgui_text(":");
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_int_ext("##minute", &inputMinute, &minuteInput)) {
        if (inputMinute < 0) inputMinute = 59;
        if (inputMinute > 59) inputMinute = 0;
    }
    dbgui_same_line();
    if (dbgui_button("Set Time")) {
        gDLL_7_Newday->vtbl->func9(inputHour * SECONDS_IN_HOUR + inputMinute * SECONDS_IN_MINUTE);
    }

    if (bss_30[0] != NULL) {
        dbgui_textf("1 real second = %.2f game second(s)\n", bss_30[0]->unkC4);
    }

    if (dbgui_tree_node("Advanced")) {
        dbgui_textf("data_5C: %f", data_5C);
        dbgui_textf("func8_time: %f", func8Time);

        dbgui_set_next_item_width(60);
        if (dbgui_input_int_ext("##seconds", &inputTimeSeconds, &nostepInput)) {
            if (inputTimeSeconds < 0) inputTimeSeconds = 0;
            if (inputTimeSeconds > SECONDS_IN_DAY) inputTimeSeconds = SECONDS_IN_DAY;
        }
        dbgui_same_line();
        if (dbgui_button("Set Time (raw seconds)")) {
            gDLL_7_Newday->vtbl->func9(inputTimeSeconds);
        }
        if (bss_30[0] != NULL) {
            dbgui_set_next_item_width(120);
            if (dbgui_input_int("timeRateTicks", &bss_30[0]->unkDC)) {
                bss_30[0]->unkC4 = (f32) bss_30[0]->unkDC / 60.0f;
            }
        }

        dbgui_tree_pop();
    }
}
