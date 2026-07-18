#include "modding.h"
#include "recomputils.h"

#include "sys/objects.h"

extern int gObjIndexCount; //count of OBJINDEX.BIN entries
extern int gNumObjectsTabEntries;

RECOMP_HOOK("objLoadObjdef") void objLoadObjdef_hook(s32 tabIdx) {
    if (tabIdx >= gNumObjectsTabEntries) {
        recomp_eprintf("[objLoadObjdef(%d)] tabIdx out of bounds.\n", tabIdx);
    }
}

static ObjSetup *obj_setup_object__setup;

RECOMP_HOOK("objSetupObjectActual") void objSetupObjectActual_hook(ObjSetup *setup, u32 initFlags, s32 mapID, s32 param4, Object *parent, s32 param6) {
    obj_setup_object__setup = setup;

    s32 objId = setup->objId;

    if (!(initFlags & 2)) {
        if (objId > gObjIndexCount) {
            recomp_eprintf("[objSetupObjectActual] objID (0x%X) out of bounds.\n", objId);
        }
    }
}

RECOMP_HOOK_RETURN("objSetupObjectActual") void objSetupObjectActual_return_hook(void) {
    Object *obj = recomphook_get_return_ptr();
    if (obj == NULL && obj_setup_object__setup != NULL) {
        recomp_eprintf("objSetupObjectActual failed to load objID: (0x%X)!\n", obj_setup_object__setup->objId);
    }
}
