#include "modding.h"
#include "repacker_common.h"
#include "tab_ext.h"

#include "PR/ultratypes.h"

static TabExt tab = {
    .name = "MAPS",
    .id = MAPS_TAB,
    .binId = MAPS_BIN
};

REPACKER_ON_INIT_CALLBACK void maps_ext_init() {
    tab_ext_init(&tab);
}

REPACKER_ON_COMMIT_CALLBACK void maps_ext_commit() {
    tab_ext_rebuild(&tab);
}

RECOMP_EXPORT void repacker_maps_set_replacement(s32 mapID, s32 segment, const void *data, u32 sizeBytes) {
    tab_ext_set_entry_replacement(&tab, (mapID * 7) + segment, data, sizeBytes);
}

RECOMP_EXPORT void *repacker_maps_get(s32 mapID, s32 segment, u32 *outSize) {
    return tab_ext_get_entry(&tab, (mapID * 7) + segment, outSize);
}

RECOMP_EXPORT void *repacker_maps_resize(s32 mapID, s32 segment, u32 newSize) {
    return tab_ext_resize_entry(&tab, (mapID * 7) + segment, newSize);
}
