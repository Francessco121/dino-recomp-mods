#include "game/objects/object.h"
#include "modding.h"
#include "recompconfig.h"
#include "recomputils.h"

#include "PR/ultratypes.h"

typedef enum {
    SEQ_DEBUG_LOGGING_ON,
    SEQ_DEBUG_LOGGING_OFF,
} SeqDebugLogging;

#include "recomp/dlls/engine/3_ANIM_recomp.h"

#define MAX_SEQSLOTS 45

/*0x168*/ extern s8 sSlotInUse[MAX_SEQSLOTS];

RECOMP_HOOK_DLL(anim_start_obj_sequence) void start_obj_seq_hook(s32 seqno, Object* object, s32 enabledActors) {
    s32 i;
    
    if (recomp_get_config_u32("seq_logging") != SEQ_DEBUG_LOGGING_ON) {
        return;
    }

    if (seqno == -1) {
        return;
    }
    for (i = 25; i < MAX_SEQSLOTS; i++) {
        if (sSlotInUse[i] == 0) {
            i = MAX_SEQSLOTS + 1; // break
        }
    }
    if (i == MAX_SEQSLOTS) {
        return;
    }
    if ((seqno < 0) || (seqno >= object->def->numSequences)) {
        return;
    }
    if (object->def->pSeq == NULL) {
        return;
    }

    const char *objName = object->def == NULL ? "<unknown>" : object->def->name;
    recomp_printf("%s (%d) playing seq %d (0x%X)\n", objName, object->id, seqno, object->def->pSeq[seqno]);
}
