#include "bwlog_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/objects/210_player.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "sys/print.h"
#include "dll.h"
#include "functions.h"

typedef struct {
    DLL27_Data unk0;
    Vec3f unk260[2];
    Vec3f unk278[2];
    f32 unk290[2];
    f32 unk298[2];
    Vec4f unk2A0;
    f32 unk2B0;
    f32 unk2B4;
    f32 unk2B8;
    f32 unk2BC;
    f32 unk2C0;
    f32 unk2C4;
    f32 unk2C8;
    f32 unk2CC;
    f32 unk2D0[2];
    f32 unk2D8[2];
    f32 unk2E0[2];
    u8 _unk2E8[0x2F8 - 0x2E8];
    f32 unk2F8;
    f32 unk2FC;
    f32 unk300[2];
    f32 unk308;
    f32 unk30C;
    f32 unk310;
    u32 unk314;
    s32 unk318;
    u16 unk31C[2];
    u16 unk320;
    s16 unk322;
    s16 unk324;
    s16 unk326;
    s16 unk328;
    u8 unk32A;
    u8 unk32B;
    u8 unk32C;
    u8 unk32D;
    u8 unk32E;
    u8 _unk32E[0x338 - 0x32F];
    Object *unk338; // dockpoint
} BWlog_Data;

void bwlog_debug_tab(Object *obj) {
    BWlog_Data *objdata = obj->data;

    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk260[%d]: %f,%f,%f", i, objdata->unk260[i].x, objdata->unk260[i].y, objdata->unk260[i].z);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk278[%d]: %f,%f,%f", i, objdata->unk278[i].x, objdata->unk278[i].y, objdata->unk278[i].z);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk290[%d]: %f", i, objdata->unk290[i]);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk298[%d]: %f", i, objdata->unk298[i]);
    }
    dbgui_textf("unk2A0: %f,%f,%f,%f", objdata->unk2A0.x, objdata->unk2A0.y, objdata->unk2A0.z, objdata->unk2A0.w);
    dbgui_textf("unk2B0: %f", objdata->unk2B0);
    dbgui_textf("unk2B4: %f", objdata->unk2B4);
    dbgui_textf("unk2B8: %f", objdata->unk2B8);
    dbgui_textf("unk2BC: %f", objdata->unk2BC);
    dbgui_textf("unk2C0: %f", objdata->unk2C0);
    dbgui_textf("unk2C4: %f", objdata->unk2C4);
    dbgui_textf("unk2C8: %f", objdata->unk2C8);
    dbgui_textf("unk2CC: %f", objdata->unk2CC);
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk2D0[%d]: %f", i, objdata->unk2D0[i]);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk2D8[%d]: %f", i, objdata->unk2D8[i]);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk2E0[%d]: %f", i, objdata->unk2E0[i]);
    }
    dbgui_textf("unk2F8: %f", objdata->unk2F8);
    dbgui_textf("unk2FC: %f", objdata->unk2FC);
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk300[%d]: %f", i, objdata->unk300[i]);
    }
    dbgui_textf("unk308: %f", objdata->unk308);
    dbgui_textf("unk30C: %f", objdata->unk30C);
    dbgui_textf("unk310: %f", objdata->unk310);
    dbgui_textf("unk314: 0x%X", objdata->unk314);
    dbgui_textf("unk318: %d", objdata->unk318);
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk31C[%d]: 0x%X", i, objdata->unk31C[i]);
    }
    dbgui_textf("unk320: 0x%X", objdata->unk320);
    dbgui_textf("unk322: %d", objdata->unk322);
    dbgui_textf("unk324: %d", objdata->unk324);
    dbgui_textf("unk326: %d", objdata->unk326);
    dbgui_textf("unk328: %d", objdata->unk328);
    dbgui_textf("unk32A: %d", objdata->unk32A);
    dbgui_textf("unk32B: %d", objdata->unk32B);
    dbgui_textf("unk32C: %d", objdata->unk32C);
    dbgui_textf("unk32D: %d", objdata->unk32D);
    dbgui_textf("unk32E: %d", objdata->unk32E);
    if (objdata->unk338 != NULL) {
        if (dbgui_tree_node("unk338")) {
            object_edit_contents(objdata->unk338);
            dbgui_tree_pop();
        }
    } else {
        dbgui_text("unk338: (null)");
    }
}
