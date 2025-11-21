#include "modding.h"
#include "repacker_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"

static TabExt tab = {
    .name = "HITS",
    .id = HITS_TAB,
    .binId = HITS_BIN
};

REPACKER_ON_INIT_CALLBACK void hits_ext_init() {
    tab_ext_init(&tab);
}

REPACKER_ON_COMMIT_CALLBACK void hits_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void repacker_hits_set_replacement(s32 hitTabIdx, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, hitTabIdx, data, sizeBytes);
}

RECOMP_EXPORT void *repacker_hits_get(s32 hitTabIdx, u32 *outSize) {
    return tab_ext_get_entry(&tab, hitTabIdx, outSize);
}

RECOMP_EXPORT void *repacker_hits_resize(s32 hitTabIdx, u32 newSize) {
    return tab_ext_resize_entry(&tab, hitTabIdx, newSize);
}
