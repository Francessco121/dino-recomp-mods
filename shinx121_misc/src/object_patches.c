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

RECOMP_HOOK("obj_setup_object") void obj_setup_object_hook(ObjSetup *setup, u32 setupFlags, s32 mapID, s32 param4, Object *parent, s32 param6) {
    s32 objId = setup->objId;

    if (!(setupFlags & 2)) {
        if (objId > gObjIndexCount) {
            recomp_eprintf("[obj_setup_object] objId (0x%X) out of bounds.\n", objId);
        }
    }
}
