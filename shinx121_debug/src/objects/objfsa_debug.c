#include "objfsa_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_dll_ids.h"
#include "object_debug.h"
#include "../3d.h"

#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "dlls/engine/18_objfsa.h"

ObjFSA_Data *objfsa_debug_get_data(Object *obj) {
    switch (obj->def->dllID) {
        case DLL_ID_Player:
        case DLL_ID_KT_Rex:
            return (ObjFSA_Data*)obj->data;
        default:
            return NULL;
    }
}

void objfsa_debug_tab(Object *obj, ObjFSA_Data *fsa) {
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
}