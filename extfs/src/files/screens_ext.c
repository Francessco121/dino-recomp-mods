#include "screens_ext.h"

#include "modding.h"
#include "extfs_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"

static TabExt tab = {
    .name = "SCREENS",
    .id = SCREENS_TAB,
    .binId = SCREENS_BIN,
    .stride = 1
};

_Bool screens_ext_try_read_tab(void **outTab) {
    return tab_ext_get_rebuilt_entries(&tab, outTab);
}

_Bool screens_ext_try_read_bin(void *dst, u32 offset, u32 size) {
    return tab_ext_try_read_bin(&tab, dst, offset, size);
}

EXTFS_ON_INIT_CALLBACK void screens_ext_init() {
    tab_ext_init(&tab);
}

EXTFS_ON_COMMIT_CALLBACK void screens_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void extfs_screens_set_replacement(s32 screenID, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, screenID, data, sizeBytes);
}

RECOMP_EXPORT void *extfs_screens_get(s32 screenID, u32 *outSize) {
    return tab_ext_get_entry(&tab, screenID, outSize);
}

RECOMP_EXPORT void *extfs_screens_resize(s32 screenID, u32 newSize) {
    return tab_ext_resize_entry(&tab, screenID, newSize);
}
