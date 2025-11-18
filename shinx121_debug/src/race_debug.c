#include "dll.h"
#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"
#include "3d.h"

#include "PR/ultratypes.h"
#include "dlls/engine/4_race.h"
#include "sys/math.h"

extern SRT gCameraSRT;
extern f32 gWorldX;
extern f32 gWorldZ;

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

static s32 raceDebugWindowOpen = FALSE;
static f32 checkpointRenderDist = 1200;
static s32 filterByDist = FALSE;
static void debug_checkpoints(void);
static void debug_draw_checkpoints(void);
static void debug_racers(void);
static s32 hoveredIdx = -1;

RECOMP_CALLBACK(".", my_debug_menu_event) void race_debug_menu_callback() {
    dbgui_menu_item("Race", &raceDebugWindowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void race_debug_dbgui_callback() {
    if (raceDebugWindowOpen) {
        if (dbgui_begin("Race Debug", &raceDebugWindowOpen)) {
            if (dbgui_begin_tab_bar("race_tabs")) {
                if (dbgui_begin_tab_item("Checkpoints", NULL)) {
                    debug_checkpoints();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Racers", NULL)) {
                    debug_racers();
                    dbgui_end_tab_item();
                }
                dbgui_end_tab_bar();
            }
            
        }
        dbgui_end();

        debug_draw_checkpoints();
    }
}

#include "recomp/dlls/engine/4_race_recomp.h"

/*0x0*/ extern RaceStruct *_bss_0[10];
/*0x28*/ extern RaceStruct *_bss_28[10];
/*0x50*/ extern RaceStruct **_bss_50;
/*0x54*/ extern RaceStruct **_bss_54;
/*0x58*/ extern s16 _bss_58;
/*0x5A*/ extern s16 _bss_5A;
/*0x60*/ extern RaceCheckpoint _bss_60[200];
/*0x6A0*/ extern s32 _bss_6A0;

static void debug_checkpoints(void) {
    Vec3f camPos = {gCameraSRT.transl.x + gWorldX, gCameraSRT.transl.y, gCameraSRT.transl.z + gWorldZ};
    hoveredIdx = -1;

    dbgui_set_next_item_width(120);
    dbgui_input_float("3D Render Distance", &checkpointRenderDist);
    dbgui_checkbox("Filter List By Distance", &filterByDist);

    dbgui_textf("Loaded checkpoints (%d):", _bss_6A0);
    if (dbgui_begin_child("checkpoints")) {
        for (s32 i = 0; i < _bss_6A0; i++) {
            RaceCheckpoint *node = &_bss_60[i];
            RaceCheckpointSetup *setup = node->setup;

            if (filterByDist) {
                f32 dist = vec3_distance(&camPos, &setup->pos);
                if (dist > checkpointRenderDist) {
                    continue;
                }
            }

            if (dbgui_tree_node(recomp_sprintf_helper("[%d] Checkpoint 0x%X###%X", i, node->uID, node->uID))) {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }

                dbgui_textf("objId: 0x%X", setup->objId);
                dbgui_textf("quarterSize: 0x%X", setup->quarterSize);
                dbgui_textf("unk3: 0x%X", setup->unk3);
                dbgui_textf("unk4: 0x%X", setup->unk4);
                dbgui_textf("unk5: 0x%X", setup->unk5);
                dbgui_textf("unk6: 0x%X", setup->unk6);
                dbgui_textf("unk7: 0x%X", setup->unk7);
                dbgui_textf("x: %f", setup->pos.x);
                dbgui_textf("y: %f", setup->pos.y);
                dbgui_textf("z: %f", setup->pos.z);
                dbgui_textf("uID: 0x%X", setup->uID);
                for (s32 k = 0; k < 2; k++) {
                    dbgui_textf("unk18[%d]: 0x%X", k, setup->unk18[k]);
                }
                for (s32 k = 0; k < 2; k++) {
                    dbgui_textf("unk20[%d]: 0x%X", k, setup->unk20[k]);
                }
                dbgui_textf("unk28: 0x%X", setup->unk28);
                dbgui_textf("unk29: 0x%X", setup->unk29);
                dbgui_textf("unk2A: 0x%X", setup->unk2A);
                for (s32 k = 0; k < 2; k++) {
                    dbgui_textf("unk2B[%d]: 0x%X", k, setup->unk2B[k]);
                }
                for (s32 k = 0; k < 4; k++) {
                    dbgui_textf("unk2D[%d]: 0x%X", k, setup->unk2D[k]);
                }
                for (s32 k = 0; k < 4; k++) {
                    dbgui_textf("unk31[%d]: 0x%X", k, setup->unk31[k]);
                }
                dbgui_textf("unk35: 0x%X", setup->unk35);
                dbgui_textf("unk36: 0x%X", setup->unk36);
                dbgui_textf("unk37: 0x%X", setup->unk37);
                dbgui_textf("unk38: 0x%X", setup->unk38);
                dbgui_textf("unk39: 0x%X", setup->unk39);
                dbgui_textf("unk3A: 0x%X", setup->unk3A);
                dbgui_textf("unk3B: 0x%X", setup->unk3B);
                dbgui_textf("unk3C: 0x%X", setup->unk3C);
                dbgui_textf("unk3D: 0x%X", setup->unk3D);
                dbgui_textf("unk3E: 0x%X", setup->unk3E);
                dbgui_textf("unk3F: 0x%X", setup->unk3F);
                
                dbgui_tree_pop();
            } else {
                if (dbgui_is_item_hovered()) {
                    hoveredIdx = i;
                }
            }
        }
    }
    dbgui_end_child();
}

static void debug_draw_checkpoints(void) {
    Vec3f camPos = {gCameraSRT.transl.x + gWorldX, gCameraSRT.transl.y, gCameraSRT.transl.z + gWorldZ};

    for (s32 i = 0; i < _bss_6A0; i++) {
        RaceCheckpoint *node = &_bss_60[i];
        RaceCheckpointSetup *setup = node->setup;

        f32 dist = vec3_distance(&camPos, &setup->pos);
        if (dist > checkpointRenderDist) {
            continue;
        }

        for (s32 k = 0; k < 2; k++) {
            if (node->setup->unk20[k] == -1) {
                continue;
            }
            s32 _discard;
            RaceCheckpointSetup *next = (RaceCheckpointSetup*)gDLL_4_Race->vtbl->func8(node->setup->unk20[k], &_discard);
            if (next == NULL) {
                continue;
            }

            draw_3d_line(
                setup->pos.x, setup->pos.y, setup->pos.z, 
                next->pos.x, next->pos.y, next->pos.z, 
                hoveredIdx == i ? 0xFFFFFFFF : 0xFFFF00FF);
        }
        
        draw_3d_text(node->setup->pos.x, node->setup->pos.y, node->setup->pos.z, 
            recomp_sprintf_helper("[%d] Checkpoint 0x%X", i, node->uID),
            hoveredIdx == i ? 0xFFFFFFFF : 0xFF00FFFF);
    }
}

static void debug_racer(RaceStruct *racer) {
    dbgui_textf("unk0: %f", racer->unk0);
    dbgui_textf("unk4: %f", racer->unk4);
    dbgui_textf("unk8: %f", racer->unk8);
    dbgui_textf("unkC: %f", racer->unkC);
    dbgui_textf("unk10: 0x%X", racer->unk10);
    dbgui_textf("unk14: 0x%X", racer->unk14);
    dbgui_textf("unk18: 0x%X", racer->unk18);
    dbgui_textf("unk1C: %d", racer->unk1C);
    dbgui_textf("unk20: %d", racer->unk20);
}

static void debug_racers(void) {
    dbgui_push_str_id("_bss_54");
    dbgui_textf("_bss_54 (%d):", _bss_5A);
    for (s32 i = 0; i < _bss_5A; i++) {
        if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
            RaceStruct *racer = _bss_54[i];
            debug_racer(racer);

            dbgui_tree_pop();
        }
    }
    dbgui_pop_id();

    dbgui_push_str_id("_bss_50");
    dbgui_textf("_bss_50 (%d):", _bss_58);
    for (s32 i = 0; i < _bss_58; i++) {
        if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
            RaceStruct *racer = _bss_50[i];
            debug_racer(racer);

            dbgui_tree_pop();
        }
    }
    dbgui_pop_id();
}
