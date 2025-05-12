#pragma once

#include "modding.h"

#include "PR/ultratypes.h"

RECOMP_IMPORT("extfs", void extfs_tab_set_entry_replacement(s32 tabFileId, s32 tabIdx, const void *data, u32 sizeBytes))
RECOMP_IMPORT("extfs", void *extfs_tab_get_entry(s32 tabFileId, s32 tabIdx, u32 *outSize))
RECOMP_IMPORT("extfs", void *extfs_tab_resize_entry(s32 tabFileId, s32 tabIdx, u32 newSize))

#define EXTFS_ON_LOAD_REPLACEMENTS_CALLBACK RECOMP_CALLBACK("extfs", extfs_on_load_replacements)
#define EXTFS_ON_LOAD_MODIFICATIONS_CALLBACK RECOMP_CALLBACK("extfs", extfs_on_load_modifications)
