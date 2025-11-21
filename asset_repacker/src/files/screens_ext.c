#include "modding.h"
#include "repacker_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"

static TabExt tab = {
    .name = "SCREENS",
    .id = SCREENS_TAB,
    .binId = SCREENS_BIN
};

REPACKER_ON_INIT_CALLBACK void screens_ext_init() {
    tab_ext_init(&tab);
}

REPACKER_ON_COMMIT_CALLBACK void screens_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void repacker_screens_set_replacement(s32 screenTabIdx, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, screenTabIdx, data, sizeBytes);
}

RECOMP_EXPORT void *repacker_screens_get(s32 screenTabIdx, u32 *outSize) {
    return tab_ext_get_entry(&tab, screenTabIdx, outSize);
}

RECOMP_EXPORT void *repacker_screens_resize(s32 screenTabIdx, u32 newSize) {
    return tab_ext_resize_entry(&tab, screenTabIdx, newSize);
}
