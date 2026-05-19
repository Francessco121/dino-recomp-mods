#include "modding.h"
#include "recomputils.h"

#include "PR/ultratypes.h"
#include "dlls/engine/3_animation.h"
#include "sys/dll.h"
#include "dll.h"

#include "recomp/dlls/engine/3_ANIM_recomp.h"

#define MAX_SEQSLOTS 45

/*0x168*/ extern s8 sSlotInUse[MAX_SEQSLOTS];

static void *hijack_dll_export(DLLFile *dll, s32 exportIdx, void *hijack) {
    u32 *vtbl = DLL_FILE_TO_EXPORTS(dll);

    // +1 to get skip the initial null pointer. Export indices do not include the null at the start of the vtable.
    void **exportPtr = (void**)&vtbl[exportIdx + 1];
    void *original = *exportPtr;
    *exportPtr = hijack;

    return original;
}

typedef s32 (*Export17)(s32 objectSeqIndex, Object* object, s32 arg2);
static Export17 export17_func; 
static s32 export17_hijack(s32 objectSeqIndex, Object* object, s32 arg2);

// Note: Do a return hook so we have a higher chance of being last.
// Do this weirdly cause a normal hook won't catch the hijack by dinomod.
static DLLFile *recomp_sDll;
RECOMP_HOOK_DLL(anim_ctor) void anim_ctor_hook(DLLFile *dll) {
    recomp_sDll = dll;
}
RECOMP_HOOK_RETURN_DLL(anim_ctor) void anim_ctor_ret_hook() {
    export17_func = hijack_dll_export(recomp_sDll, 17, export17_hijack);
}

RECOMP_HOOK_RETURN_DLL(anim_dtor) void anim_dtor_hook() {
    export17_func = NULL;
}

s32 export17_hijack(s32 objectSeqIndex, Object* object, s32 arg2) {
    s32 ret = export17_func(objectSeqIndex, object, arg2);

    if (ret == -1) {
        s32 freeSlots = 0;
        for (s32 i = 25; i < 45; i++) {
            if (sSlotInUse[i] == 0) {
                freeSlots++;
            }
        }

        s32 seq = -1;
        s32 invalidObj = FALSE;
        s32 oobSeqIdx = FALSE;
        s32 objUID = -1;
        if (object != NULL && object->def != NULL) {
            if (objectSeqIndex >= 0 && 
                objectSeqIndex < object->def->numSequences &&
                object->def->pSeq != NULL) {
                seq = object->def->pSeq[objectSeqIndex];
            } else {
                oobSeqIdx = TRUE;
            }

            if (object->setup != NULL) {
                objUID = object->setup->uID;
            }
        } else {
            invalidObj = TRUE;
        }

        if (invalidObj) {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence. Object pointer is invalid.\n  start_obj_sequence(%d, %p, %d)\n",
                objectSeqIndex, object, arg2);
        } else if (oobSeqIdx) {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence. Object sequence index out of bounds (object has %d sequences).\n  start_obj_sequence(%d, %p, %d)\n  Object: %s (ID 0x%X, UID 0x%X)\n",
                object->def->numSequences,
                objectSeqIndex, object, arg2,
                object->def->name, object->id, objUID);
        } else if (freeSlots == 0) {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence. No sequence slots available.\n  start_obj_sequence(%d, %p, %d)\n  Seq: 0x%X\n  Object: %s (ID 0x%X, UID 0x%X)\n",
                objectSeqIndex, object, arg2,
                seq,
                object->def->name, object->id, objUID);
        } else {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence.\n  start_obj_sequence(%d, %p, %d)\n  Seq: 0x%X\n  Object: %s (ID 0x%X, UID 0x%X)\n",
                objectSeqIndex, object, arg2,
                seq,
                object->def->name, object->id, objUID);
        }
    }

    return ret;
}
