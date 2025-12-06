#include "kd_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "dlls/engine/53.h"
#include "dlls/objects/210_player.h"
#include "dlls/objects/332_FXEmit.h"
#include "game/objects/object.h"
#include "sys/gfx/model.h"
#include "sys/objects.h"
#include "sys/objanim.h"
#include "sys/objtype.h"

typedef struct {
/*00:0*/ u32 pad0_0 : 8;
/*00:8*/ u32 animFinished : 1;
/*00:9*/ u32 rightWingOpened : 1;
/*00:10*/ u32 leftWingOpened : 1;
/*00:11*/ u32 rightPipeDetached: 1;
/*00:12*/ u32 leftPipeDetached: 1;
/*00:13*/ u32 hatchOpened: 1;
/*00:14*/ u32 flameDebounce : 1;
/*00:15*/ u32 health : 8;
/*00:23*/ u32 loadedTempDLL : 1;
/*00:24*/ u32 pad0_24: 8;
/*04*/ Object *player;
/*08*/ Object *unk8[2];
/*10*/ Object *unk10[6];
/*28*/ f32 animTickDelta; // anim progress per tick (60hz)
/*2C*/ s16 rightPipeYOffset;
/*2E*/ s16 leftPipeYOffset;
/*30*/ u16 rightAcidAttackTimer;
/*32*/ u16 leftAcidAttackTimer;
/*34*/ s16 flameAttackTimer;
/*36*/ u16 rightPipeTimer;
/*38*/ u16 leftPipeTimer;
/*3C*/ u32 soundHandle1;
/*40*/ u32 soundHandle2;
/*44*/ u32 soundHandle3;
/*48*/ Vec3f attachmentPositions[15];
/*FC*/ f32 playerStartY;
} KamerianBoss_Data;

void kd_debug_tab(Object *obj) {
    KamerianBoss_Data *kddata = obj->data;

    if (obj->modelInsts != NULL && obj->modelInsts[0] != NULL && obj->modelInsts[0]->model != NULL && obj->modelInsts[0]->model->hitSpheres != NULL) {
        ModelInstance *modelInst = obj->modelInsts[obj->modelInstIdx];
        Model *model = modelInst->model;

        if (dbgui_tree_node("hitSpheres")) {
            
            for (s32 i = 0; i < model->hitSphereCount; i++) {
                if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                    HitSphere *hs = &model->hitSpheres[i];

                    dbgui_textf("jointIndex: %d", hs->jointIndex);
                    dbgui_textf("unk2: %d", hs->unk2);
                    for (s32 offset = 0x4; offset < 0xA; offset++) {
                        dbgui_textf("unk%X: 0x%X", offset, ((u8*)hs)[offset]);
                    }
                    dbgui_textf("unkA: 0x%X", hs->unkA);
                    dbgui_textf("unkC: 0x%X", hs->unkC);
                    dbgui_textf("unkD: 0x%X", hs->unkD);

                    dbgui_tree_pop();
                }
            }
            
            dbgui_tree_pop();
        }
    }

    dbgui_separator();

    dbgui_textf("pad0_0: 0x%X", kddata->pad0_0);
    dbgui_textf("animFinished: 0x%X", kddata->animFinished);
    dbgui_textf("rightWingOpened: 0x%X", kddata->rightWingOpened);
    dbgui_textf("leftWingOpened: 0x%X", kddata->leftWingOpened);
    dbgui_textf("rightPipeDetached: 0x%X", kddata->rightPipeDetached);
    dbgui_textf("leftPipeDetached: 0x%X", kddata->leftPipeDetached);
    dbgui_textf("hatchOpened: 0x%X", kddata->hatchOpened);
    dbgui_textf("flameDebounce: 0x%X", kddata->flameDebounce);
    dbgui_textf("health: 0x%X", kddata->health);
    dbgui_textf("loadedTempDLL: 0x%X", kddata->loadedTempDLL);
    dbgui_textf("pad0_24: 0x%X", kddata->pad0_24);
    dbgui_textf("player: %p", kddata->player);
    if (dbgui_tree_node("unk8")) {
        for (s32 i = 0; i < 2; i++) {
            dbgui_textf("[%d] %p", i, kddata->unk8[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("unk10")) {
        for (s32 i = 0; i < 6; i++) {
            dbgui_textf("[%d] %p", i, kddata->unk10[i]);
        }
        
        dbgui_tree_pop();
    }
    dbgui_input_float("animTickDelta", &kddata->animTickDelta);
    dbgui_textf("rightPipeYOffset: %d", kddata->rightPipeYOffset);
    dbgui_textf("leftPipeYOffset: %d", kddata->leftPipeYOffset);
    dbgui_textf("rightAcidAttackTimer: %d", kddata->rightAcidAttackTimer);
    dbgui_textf("leftAcidAttackTimer: %d", kddata->leftAcidAttackTimer);
    dbgui_textf("flameAttackTimer: %d", kddata->flameAttackTimer);
    dbgui_textf("rightPipeTimer: %d", kddata->rightPipeTimer);
    dbgui_textf("leftPipeTimer: %d", kddata->leftPipeTimer);
    dbgui_textf("soundHandle1: 0x%X", kddata->soundHandle1);
    dbgui_textf("soundHandle2: 0x%X", kddata->soundHandle2);
    dbgui_textf("soundHandle3: 0x%X", kddata->soundHandle3);
    if (dbgui_tree_node("attachmentPositions")) {
        for (s32 i = 0; i < 15; i++) {
            dbgui_textf("[%d] %f,%f,%f", i, 
                kddata->attachmentPositions[i].x, kddata->attachmentPositions[i].y, kddata->attachmentPositions[i].z);
        }
        
        dbgui_tree_pop();
    }
    dbgui_textf("playerStartY: %f", kddata->playerStartY);
}
