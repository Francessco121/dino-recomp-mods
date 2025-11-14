#include "modding.h"
#include "recomputils.h"

#include "sys/objects.h"

extern int gObjIndexCount; //count of OBJINDEX.BIN entries
extern int gNumObjectsTabEntries;

RECOMP_HOOK("obj_load_objdef") void obj_load_objdef_hook(s32 tabIdx) {
    if (tabIdx >= gNumObjectsTabEntries) {
        recomp_eprintf("[obj_load_objdef(%d)] tabIdx out of bounds.\n", tabIdx);
    }
}

static ObjSetup *obj_setup_object__setup;

RECOMP_HOOK("obj_setup_object") void obj_setup_object_hook(ObjSetup *setup, u32 initFlags, s32 mapID, s32 param4, Object *parent, s32 param6) {
    obj_setup_object__setup = setup;

    s32 objId = setup->objId;

    if (!(initFlags & 2)) {
        if (objId > gObjIndexCount) {
            recomp_eprintf("[obj_setup_object] objID (0x%X) out of bounds.\n", objId);
        }
    }
}

RECOMP_HOOK_RETURN("obj_setup_object") void obj_setup_object_return_hook(void) {
    Object *obj = recomphook_get_return_ptr();
    if (obj == NULL && obj_setup_object__setup != NULL) {
        recomp_eprintf("obj_setup_object failed to load objID: (0x%X)!\n", obj_setup_object__setup->objId);
    }
}
