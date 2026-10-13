#include "cfcloudbaby_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/engine/26_curves.h"
#include "sys/objexpr.h"

typedef struct {
/*00*/ ObjSetup base;
       // Considers things (the player, beans) to be close below this distance.
/*18*/ s16 closeMaxDist;
       // Can be rescued by the player below this distance.
/*1A*/ s16 rescueMaxDist;
/*1C*/ u8 initialState;
/*1D*/ u8 yaw8;
       // Set when initially rescued.
/*1E*/ s16 rescuedGamebit;
/*20*/ u8 initialState2;
       // Set when the rescued timer completes and is considered 
       // to be in the throne room now.
/*22*/ s16 reachedPerchGamebit;
} CFCloudBaby_Setup;

typedef struct {
    Vec3f unk0;
    Vec3f unkC;
    Vec3f unk18;
    Vec3f unk24;
    f32 tValue;
    f32 unk34;
} CFCloudBaby_Data_4;

typedef struct {
    s32 rescuedTimer;
    CFCloudBaby_Data_4 unk4;
    HeadAnimation unk3C;
    HeadAnimation unk60;
    f32 beanXDir;
    f32 beanZDir;
    f32 unk8C;
    f32 animChange;
    f32 beanScale;
    s32 dualInitialState;
    s32 unk9C;
    s32 unkA0;
    s32 attackingPlayer;
    s32 unkA8;
    s32 state;
    u8 prevState;
    s32 distracted;
    s16 origYaw;
    s16 unkBA;
    u8 _unkBC[0xFC - 0xBC];
    Object* bean;
    Vec3f savedPos; // from before being distracted
    UnkCurvesStruct unk10C;
    u8 flags;
} CFCloudBaby_Data;

void cfcloudbaby_debug_tab(Object *obj) {
    CFCloudBaby_Data *objdata = obj->data;
    CFCloudBaby_Data_4 *objdata4 = &objdata->unk4;
    CFCloudBaby_Setup *objsetup = (CFCloudBaby_Setup*)obj->setup;

    if (dbgui_begin_tab_bar("cfcloudbaby_tabs")) {
        if (dbgui_begin_tab_item("CFCloudBaby Data", NULL)) {
            dbgui_textf("rescuedTimer: %d", objdata->rescuedTimer);

            dbgui_textf("unk4.unk0: %f,%f,%f", objdata4->unk0.x, objdata4->unk0.y, objdata4->unk0.z);
            dbgui_textf("unk4.unkC: %f,%f,%f", objdata4->unkC.x, objdata4->unkC.y, objdata4->unkC.z);
            dbgui_textf("unk4.unk18: %f,%f,%f", objdata4->unk18.x, objdata4->unk18.y, objdata4->unk18.z);
            dbgui_textf("unk4.unk24: %f,%f,%f", objdata4->unk24.x, objdata4->unk24.y, objdata4->unk24.z);
            dbgui_textf("unk4.tValue: %f", objdata4->tValue);
            dbgui_textf("unk4.unk34: %f", objdata4->unk34);
            
            dbgui_textf("beanXDir: %f", objdata->beanXDir);
            dbgui_textf("beanZDir: %f", objdata->beanZDir);
            dbgui_textf("unk8C: %f", objdata->unk8C);
            dbgui_textf("animChange: %f", objdata->animChange);
            dbgui_textf("beanScale: %f", objdata->beanScale);
            dbgui_textf("dualInitialState: %d", objdata->dualInitialState);
            dbgui_textf("unk9C: %d", objdata->unk9C);
            dbgui_textf("unkA0: %d", objdata->unkA0);
            dbgui_textf("attackingPlayer: %d", objdata->attackingPlayer);
            dbgui_textf("unkA8: %d", objdata->unkA8);
            dbgui_input_int("state", &objdata->state);
            dbgui_textf("prevState: %d", objdata->prevState);
            dbgui_input_int("distracted", &objdata->distracted);
            dbgui_textf("origYaw: %d", objdata->origYaw);
            dbgui_textf("unkBA: %d", objdata->unkBA);
            if (objdata->bean != NULL) {
                if (dbgui_tree_node("bean")) {
                    object_edit_contents(objdata->bean);
                    dbgui_tree_pop();
                }
            } else {
                dbgui_text("bean: (null)");
            }
            dbgui_textf("savedPos: %f,%f,%f", objdata->savedPos.x, objdata->savedPos.y, objdata->savedPos.z);
            dbgui_textf("flags: 0x%X", objdata->flags);

            dbgui_end_tab_item();
        }

        if (dbgui_begin_tab_item("Obj Setup", NULL)) {
            dbgui_textf("closeMaxDist: %d", objsetup->closeMaxDist);
            dbgui_textf("rescueMaxDist: %d", objsetup->rescueMaxDist);
            dbgui_textf("initialState: %d", objsetup->initialState);
            dbgui_textf("yaw8: %d", objsetup->yaw8);
            dbgui_textf("rescuedGamebit: 0x%X", objsetup->rescuedGamebit);
            dbgui_textf("initialState2: %d", objsetup->initialState2);
            dbgui_textf("reachedPerchGamebit: 0x%X", objsetup->reachedPerchGamebit);

            dbgui_end_tab_item();
        }

        dbgui_end_tab_bar();
    }
}
