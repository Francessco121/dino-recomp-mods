#include "kt_rex_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/engine/18_objfsa.h"
#include "dlls/engine/33.h"
#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "sys/generic_stack.h"
#include "sys/print.h"

typedef struct {
/*00*/ DLL33_ObjSetup base;
/*38*/ f32 unk38[3];
/*44*/ u16 unk44[3];
/*4A*/ u16 unk4A[4];
/*52*/ u8 unk52[4];
/*56*/ u8 unk56[4];
} KTrex_ObjSetup;

typedef struct {
    GenericStack *stack;
    f32 unk4;
    f32 unk8;
    s32 unkC;
    f32 unk10[4];
    f32 unk20[4];
    f32 unk30[4];
    f32 unk40[4];
    f32 unk50[4];
    f32 unk60[4];
    f32 unk70[4];
    f32 unk80[4];
    f32 unk90[4];
    f32 unkA0[4];
    f32 unkB0[4];
    f32 unkC0[4];
    f32 *unkD0;
    f32 *unkD4;
    f32 *unkD8;
    f32 *unkDC;
    f32 *unkE0;
    f32 *unkE4;
    f32 unkE8;
    f32 unkEC;
    f32 unkF0;
    f32 unkF4;
    s16 unkF8;
    u16 unkFA;
    u8 unkFC;
    u8 unkFD;
    u8 unkFE;
    u8 unkFF;
    u8 unk100;
    u8 unk101;
    u8 unk102;
    s8 unk103;
    s32 unk104;
    u8 unk108;
    SRT unk10C;
    SRT unk124;
    SRT unk13C;
    SRT unk154;
    Vec3f unk16C;
} KTrex_Data;

void kt_rex_debug_tab(Object *obj) {
    DLL33_Data *dll33Data = obj->data;
    ObjFSA_Data *fsa = &dll33Data->fsa;
    KTrex_Data *ktdata = dll33Data->unk3F4;
    if (ktdata == NULL) {
        return;
    }

    if (dbgui_begin_tab_bar("ktrex_tabs")) {
        if (dbgui_begin_tab_item("Obj Setup", NULL)) {
            KTrex_ObjSetup *objsetup = (KTrex_ObjSetup*)obj->setup;

            dbgui_textf("unk38: %f,%f,%f", objsetup->unk38[0], objsetup->unk38[1], objsetup->unk38[2]);
            dbgui_textf("unk44: %d,%d,%d", objsetup->unk44[0], objsetup->unk44[1], objsetup->unk44[2]);
            dbgui_textf("unk4A: %d,%d,%d,%d", objsetup->unk4A[0], objsetup->unk4A[1], objsetup->unk4A[2],  objsetup->unk4A[3]);
            dbgui_textf("unk52: %d,%d,%d,%d", objsetup->unk52[0], objsetup->unk52[1], objsetup->unk52[2],  objsetup->unk52[3]);
            dbgui_textf("unk56: %d,%d,%d,%d", objsetup->unk56[0], objsetup->unk56[1], objsetup->unk56[2],  objsetup->unk56[3]);

            dbgui_end_tab_item();
        }
        if (dbgui_begin_tab_item("Obj FSA Data", NULL)) {
            dbgui_textf("flags: 0x%X", fsa->flags);
            dbgui_textf("unk264: %d", fsa->unk264);
            dbgui_textf("unk266: %d", fsa->unk266);
            dbgui_textf("logicState: %d", fsa->logicState);
            dbgui_textf("prevLogicState: %d", fsa->prevLogicState);
            dbgui_textf("animState: %d", fsa->animState);
            dbgui_textf("prevAnimState: %d", fsa->prevAnimState);
            dbgui_textf("unk270: %d", fsa->unk270);
            dbgui_textf("enteredAnimState: %d", fsa->enteredAnimState);
            dbgui_textf("enteredLogicState: %d", fsa->enteredLogicState);
            if (fsa->unk274 != NULL) {
                dbgui_textf("unk274: %f,%f,%f", fsa->unk274->x, fsa->unk274->y, fsa->unk274->z);
            } else {
                dbgui_text("unk274: (null)");
            }
            dbgui_textf("unk278: %f", fsa->unk278);
            dbgui_textf("unk27C: %f", fsa->unk27C);
            dbgui_textf("unk280: %f", fsa->unk280);
            dbgui_textf("unk284: %f", fsa->unk284);
            dbgui_textf("unk288: %f", fsa->unk288);
            dbgui_textf("speed: %f", fsa->speed);
            dbgui_textf("unk290: %f", fsa->unk290);
            dbgui_textf("unk294: %f", fsa->unk294);
            dbgui_textf("animTickDelta: %f", fsa->animTickDelta);
            dbgui_textf("unk29C: %f", fsa->unk29C);
            dbgui_textf("unk2A0: %f", fsa->unk2A0);
            dbgui_textf("unk2A4: %f", fsa->unk2A4);
            dbgui_textf("unk2A8: %f", fsa->unk2A8);
            dbgui_textf("unk2AC: %f", fsa->unk2AC);
            dbgui_textf("unk2B0: %f", fsa->unk2B0);
            dbgui_textf("unk2B4: %f", fsa->unk2B4);
            dbgui_textf("targetDist: %f", fsa->targetDist);

            if (fsa->target != NULL) {
                if (dbgui_tree_node("target")) {
                    object_edit_contents(fsa->target);
                    dbgui_tree_pop();
                }
            } else {
                dbgui_text("target: (null)");
            }

            dbgui_textf("unk2EC: %f,%f,%f", fsa->unk2EC.x, fsa->unk2EC.y, fsa->unk2EC.z);
            dbgui_textf("unk2F8: %f", fsa->unk2F8);
            dbgui_textf("animExitAction: %p", fsa->animExitAction);

            dbgui_textf("unk304: %d", fsa->unk304);
            dbgui_textf("unk308: %d", fsa->unk308);
            dbgui_textf("unk30C: %d", fsa->unk30C);
            dbgui_textf("unk310: %d", fsa->unk310);
            dbgui_textf("logicStateTime: %d", fsa->logicStateTime);
            dbgui_textf("unk324: %d", fsa->unk324);
            dbgui_textf("unk326: %d", fsa->unk326);
            dbgui_textf("unk328: %d", fsa->unk328);
            dbgui_textf("unk32A: %d", fsa->unk32A);
            dbgui_textf("animStateTime: %d", fsa->animStateTime);
            dbgui_textf("unk32E: %d", fsa->unk32E);
            dbgui_textf("unk330: %d", fsa->unk330);
            dbgui_textf("unk334: %d", fsa->unk334);
            dbgui_textf("unk338: %d", fsa->unk338);
            dbgui_textf("unk339: %d", fsa->unk339);
            dbgui_textf("unk33A: %d", fsa->unk33A);
            dbgui_textf("unk33D: %d", fsa->unk33D);
            dbgui_textf("unk33F: %d", fsa->unk33F);
            dbgui_textf("unk340: %d", fsa->unk340);
            dbgui_textf("unk341: %d", fsa->unk341);
            dbgui_textf("unk342: %d", fsa->unk342);
            dbgui_textf("unk343: %d", fsa->unk343);
            dbgui_textf("unk348: %d", fsa->unk348);
            dbgui_textf("unk34A: %d", fsa->unk34A);

            dbgui_end_tab_item();
        }
        if (dbgui_begin_tab_item("KTrex Data", NULL)) {
            dbgui_textf("objfsa logic state: %d", fsa->logicState);
            dbgui_textf("objfsa anim state: %d", fsa->animState);

            if (dbgui_tree_node("stateStack")) {
                s32 top = ktdata->stack->top;
                for (s32 i = 0; i < ktdata->stack->count; i++) {
                    top -= 1;
                    if (top < 0) {
                        top = ktdata->stack->capacity - 1;
                    }

                    s32 *ptr = (void*)((u32)ktdata->stack->data + top * ktdata->stack->elementSize);

                    dbgui_textf("[%d] %d", i, *ptr);
                    
                }
                
                dbgui_tree_pop();
            }

            dbgui_textf("unk4: %f", ktdata->unk4);
            dbgui_textf("unk8: %f", ktdata->unk8);
            dbgui_textf("unkC: %d", ktdata->unkC);
            if (dbgui_tree_node("unk10")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk10[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk20")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk20[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk30")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk30[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk40")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk40[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk50")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk50[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk60")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk60[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk70")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk70[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk80")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk80[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unk90")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unk90[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unkA0")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unkA0[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unkB0")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unkB0[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("unkC0")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->unkC0[i]);
                }
                
                dbgui_tree_pop();
            }
            dbgui_textf("unkD0: %p", ktdata->unkD0);
            dbgui_textf("unkD4: %p", ktdata->unkD4);
            dbgui_textf("unkD8: %p", ktdata->unkD8);
            dbgui_textf("unkDC: %p", ktdata->unkDC);
            dbgui_textf("unkE0: %p", ktdata->unkE0);
            dbgui_textf("unkE4: %p", ktdata->unkE4);
            dbgui_textf("unkE8: %f", ktdata->unkE8);
            dbgui_textf("unkEC: %f", ktdata->unkEC);
            dbgui_textf("unkF0: %f", ktdata->unkF0);
            dbgui_textf("unkF4: %f", ktdata->unkF4);
            dbgui_textf("unkF8: %d", ktdata->unkF8);
            dbgui_textf("unkFA: 0x%X", ktdata->unkFA);
            dbgui_textf("__unkFA.0: %d", ktdata->unkFA & 1);
            dbgui_textf("__unkFA.1-2: %d", (ktdata->unkFA >> 1) & 3);
            dbgui_textf("__unkFA.3: %d", (ktdata->unkFA & 8) != 0 ? 1 : 0);
            dbgui_textf("unkFC: %d", ktdata->unkFC);
            dbgui_textf("unkFD: %d", ktdata->unkFD);
            dbgui_textf("unkFE: %d", ktdata->unkFE);
            dbgui_textf("unkFF: 0x%X", ktdata->unkFF);
            dbgui_textf("unk100: 0x%X", ktdata->unk100);
            dbgui_textf("unk101: %d", ktdata->unk101);
            dbgui_textf("unk102: %d", ktdata->unk102);
            dbgui_textf("unk103: %d", ktdata->unk103);
            dbgui_textf("unk104: %d", ktdata->unk104);
            dbgui_textf("unk108: %d", ktdata->unk108);
            dbgui_textf("unk10C: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk10C.yaw, ktdata->unk10C.pitch, ktdata->unk10C.roll,
                ktdata->unk10C.transl.x, ktdata->unk10C.transl.y, ktdata->unk10C.transl.z,
                ktdata->unk10C.scale, ktdata->unk10C.flags);
            dbgui_textf("unk124: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk124.yaw, ktdata->unk124.pitch, ktdata->unk124.roll,
                ktdata->unk124.transl.x, ktdata->unk124.transl.y, ktdata->unk124.transl.z,
                ktdata->unk124.scale, ktdata->unk124.flags);
            dbgui_textf("unk13C: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk13C.yaw, ktdata->unk13C.pitch, ktdata->unk13C.roll,
                ktdata->unk13C.transl.x, ktdata->unk13C.transl.y, ktdata->unk13C.transl.z,
                ktdata->unk13C.scale, ktdata->unk13C.flags);
            dbgui_textf("unk154: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk154.yaw, ktdata->unk154.pitch, ktdata->unk154.roll,
                ktdata->unk154.transl.x, ktdata->unk154.transl.y, ktdata->unk154.transl.z,
                ktdata->unk154.scale, ktdata->unk154.flags);
            dbgui_textf("unk16C: %f,%f,%f", ktdata->unk16C.x, ktdata->unk16C.y, ktdata->unk16C.z);

            dbgui_end_tab_item();
        }

        dbgui_end_tab_bar();
    }
}
