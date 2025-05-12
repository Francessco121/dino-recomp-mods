#pragma once

#include "PR/ultratypes.h"

void tab_ext_init();
void tab_ext_rebuild_tabs();
_Bool tab_ext_get_rebuilt_entries(u32 tabFileId, void **outRebuiltEntries);
_Bool tab_ext_try_read_bin(s32 binFileId, void *dst, u32 offset, u32 size);
