#include "modding.h"
#include "extfs_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"

static TabExt tab = {
    .name = "MODELS",
    .id = MODELS_TAB,
    .binId = MODELS_BIN
};

EXTFS_ON_INIT_CALLBACK void models_ext_init() {
    tab_ext_init(&tab);
}

EXTFS_ON_COMMIT_CALLBACK void models_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void extfs_models_set_replacement(s32 modelTabIdx, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, modelTabIdx, data, sizeBytes);
}

RECOMP_EXPORT void *extfs_models_get(s32 modelTabIdx, u32 *outSize) {
    return tab_ext_get_entry(&tab, modelTabIdx, outSize);
}

RECOMP_EXPORT void *extfs_models_resize(s32 modelTabIdx, u32 newSize) {
    return tab_ext_resize_entry(&tab, modelTabIdx, newSize);
}
