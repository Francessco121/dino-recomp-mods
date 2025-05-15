#include "hits_ext.h"

#include "modding.h"
#include "extfs_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"

static TabExt tab = {
    .name = "HITS",
    .id = HITS_TAB,
    .binId = HITS_BIN,
    .stride = 1
};

_Bool hits_ext_try_read_tab(void **outTab) {
    return tab_ext_get_rebuilt_entries(&tab, outTab);
}

_Bool hits_ext_try_read_bin(void *dst, u32 offset, u32 size) {
    return tab_ext_try_read_bin(&tab, dst, offset, size);
}

_Bool hits_ext_try_get_tab_size(u32 *outSize) {
    return tab_ext_get_rebuilt_size(&tab, outSize);
}

EXTFS_ON_INIT_CALLBACK void hits_ext_init() {
    tab_ext_init(&tab);
}

EXTFS_ON_COMMIT_CALLBACK void hits_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void extfs_hits_set_replacement(s32 hitTabIdx, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, hitTabIdx, data, sizeBytes);
}

RECOMP_EXPORT void *extfs_hits_get(s32 hitTabIdx, u32 *outSize) {
    return tab_ext_get_entry(&tab, hitTabIdx, outSize);
}

RECOMP_EXPORT void *extfs_hits_resize(s32 hitTabIdx, u32 newSize) {
    return tab_ext_resize_entry(&tab, hitTabIdx, newSize);
}
