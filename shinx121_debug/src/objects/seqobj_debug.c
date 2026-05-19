#include "seqobj_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "sys/gfx/animseq.h"
#include "sys/main.h"
#include "sys/objtype.h"
#include "dll.h"

typedef struct {
/*00*/ ObjSetup base;
/*18*/ s16 gamebitHasPlayed; // when gamebit is set, this objseq has played before
/*1A*/ s16 gamebitPlay;    // when gamebit is set, triggers this seqobj to play
/*1C*/ u8 rotate;
/*1D*/ u8 playbackOptions;
/*1E*/ s8 seqIndex;            //The index of the sequence in the Object.bin entry's sequence list
/*1F*/ s8 modelInstIdx;        //Choose between 3D models, visible when debugging (usually a clapperboard)
/*20*/ s16 replayStartTime;
/*22*/ u16 replayActorMask; // bitfield of which actors are enabled if this seq is replaying
/*24*/ u8 warpID;              //Optionally warp the player
} SeqObj_Setup;

typedef struct {
    u8 flags;
    s8 lastPlayBitValue;
} SeqObj_Data;


void seqobj_debug_tab(Object *obj) {
    SeqObj_Data *objdata = obj->data;
    SeqObj_Setup *objsetup = (SeqObj_Setup*)obj->setup;

    dbgui_text_wrapped("Note: Using this UI to play a SeqObj will not alter the gamebits tied to the SeqObj but the sequence itself may have side effects.");
    dbgui_separator();

    if (dbgui_button("Play")) {
        gDLL_3_Animation->vtbl->start_obj_sequence(objsetup->seqIndex, obj, -1);
    }
    if (objsetup->replayStartTime != 0) {
        dbgui_same_line();
        if (dbgui_button("Replay")) {
            gDLL_3_Animation->vtbl->preempt_sequence_time(obj, objsetup->replayStartTime);
            if (objsetup->playbackOptions & 0x10) {
                gDLL_3_Animation->vtbl->start_obj_sequence(objsetup->seqIndex, obj, objsetup->replayActorMask);
            } else {
                gDLL_3_Animation->vtbl->start_obj_sequence(objsetup->seqIndex, obj, 1);
            }
        }
    }
    if (obj->seqSlot >= 0) {
        dbgui_same_line();
        if (dbgui_button("Stop")) {
            gDLL_3_Animation->vtbl->end_obj_sequence(obj->seqSlot);
        }
    }

    if (obj->def != NULL && obj->def->pSeq != NULL) {
        if (objsetup->seqIndex < 0 || objsetup->seqIndex >= obj->def->numSequences) {
            dbgui_text("WARN: SeqObj seqno out of bounds.");
        } else {
            dbgui_textf("Seq ID: 0x%X", obj->def->pSeq[objsetup->seqIndex]);
            if (objsetup->gamebitPlay != -1) {
                dbgui_textf("Play Bit: %d", main_get_bits(objsetup->gamebitPlay));
            }
            if (objsetup->gamebitHasPlayed != -1) {
                dbgui_textf("Has Played Bit: %d", main_get_bits(objsetup->gamebitHasPlayed));
            }
        }
    }

    if (dbgui_begin_tab_bar("seqobj_tabs")) {
        if (dbgui_begin_tab_item("SeqObj Setup", NULL)) {
            dbgui_textf("gamebitHasPlayed: 0x%X", objsetup->gamebitHasPlayed);
            dbgui_textf("gamebitPlay: 0x%X", objsetup->gamebitPlay);
            dbgui_textf("rotate: 0x%X", objsetup->rotate);
            dbgui_textf("playbackOptions: 0x%X", objsetup->playbackOptions);
            dbgui_textf("seqIndex: %d", objsetup->seqIndex);
            dbgui_textf("modelInstIdx: %d", objsetup->modelInstIdx);
            dbgui_textf("replayStartTime: %d", objsetup->replayStartTime);
            dbgui_textf("replayActorMask: 0x%X", objsetup->replayActorMask);
            dbgui_textf("warpID: 0x%X", objsetup->warpID);

            dbgui_end_tab_item();
        }

        if (dbgui_begin_tab_item("SeqObj Data", NULL)) {
            dbgui_textf("flags: 0x%X", objdata->flags);
            dbgui_textf("lastPlayBitValue: %d", objdata->lastPlayBitValue);

            dbgui_end_tab_item();
        }

        dbgui_end_tab_bar();
    }
}
