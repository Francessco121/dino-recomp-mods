#include "modding.h"
#include "extfs_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"

static TabExt tab = {
    .name = "OBJECTS",
    .id = OBJECTS_TAB,
    .binId = OBJECTS_BIN
};

EXTFS_ON_INIT_CALLBACK void objects_ext_init() {
    tab_ext_init(&tab);
}

EXTFS_ON_COMMIT_CALLBACK void objects_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void extfs_objects_set_replacement(s32 objectTabIdx, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, objectTabIdx, data, sizeBytes);
}

RECOMP_EXPORT void *extfs_objects_get(s32 objectTabIdx, u32 *outSize) {
    return tab_ext_get_entry(&tab, objectTabIdx, outSize);
}

RECOMP_EXPORT void *extfs_objects_resize(s32 objectTabIdx, u32 newSize) {
    return tab_ext_resize_entry(&tab, objectTabIdx, newSize);
}
