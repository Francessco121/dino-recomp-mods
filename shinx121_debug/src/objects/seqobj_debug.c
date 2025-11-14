#include "seqobj_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/objects/214_animobj.h"
#include "sys/main.h"
#include "sys/objtype.h"
#include "dll.h"

typedef struct {
/*00*/ ObjSetup base;
/*18*/ s16 gamebitPlay;        //The sequence will play when this gamebit is set
/*1A*/ s16 gamebitFinished;    //This gamebit will be set when the sequence has played
/*1C*/ u8 rotate;
/*1D*/ u8 playbackOptions;
/*1E*/ s8 seqIndex;            //The index of the sequence in the Object.bin entry's sequence list
/*1F*/ s8 modelInstIdx;        //Choose between 3D models, visible when debugging (usually a clapperboard)
/*20*/ s16 unk20;
/*22*/ u16 unk22;
/*24*/ u8 warpID;              //Optionally warp the player
} SeqObj_Setup;

typedef struct {
    u8 flags;
    s8 finished;
} SeqObj_Data;

void seqobj_debug_tab(Object *obj) {
    SeqObj_Data *objdata = obj->data;
    SeqObj_Setup *objsetup = (SeqObj_Setup*)obj->setup;

    if (dbgui_button("Play")) {
        gDLL_3_Animation->vtbl->func17(objsetup->seqIndex, obj, -1);
    }
    if (objsetup->unk20 != 0) {
        dbgui_same_line();
        if (dbgui_button("Play (alt)")) {
            if (objsetup->playbackOptions & 0x10) {
                gDLL_3_Animation->vtbl->func17(objsetup->seqIndex, obj, objsetup->unk22);
            } else {
                gDLL_3_Animation->vtbl->func17(objsetup->seqIndex, obj, 1);
            }
        }
    }

    dbgui_text("Note: Playing a SEQOBJ will not adjust gamebits.");

    dbgui_separator();

    if (dbgui_begin_tab_bar("seqobj_tabs")) {
        if (dbgui_begin_tab_item("Obj Setup", NULL)) {
            dbgui_textf("gamebitPlay: 0x%X", objsetup->gamebitPlay);
            dbgui_textf("gamebitFinished: 0x%X", objsetup->gamebitFinished);
            dbgui_textf("rotate: 0x%X", objsetup->rotate);
            dbgui_textf("playbackOptions: 0x%X", objsetup->playbackOptions);
            dbgui_textf("seqIndex: %d", objsetup->seqIndex);
            dbgui_textf("modelInstIdx: %d", objsetup->modelInstIdx);
            dbgui_textf("unk20: %d", objsetup->unk20);
            dbgui_textf("unk22: 0x%X", objsetup->unk22);
            dbgui_textf("warpID: 0x%X", objsetup->warpID);

            dbgui_end_tab_item();
        }

        if (dbgui_begin_tab_item("SeqObj Data", NULL)) {
            dbgui_textf("flags: 0x%X", objdata->flags);
            dbgui_textf("finished: %d", objdata->finished);

            dbgui_end_tab_item();
        }
    }
}
