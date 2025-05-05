#pragma once

#ifdef _MOD_FS_SRC
#define RECOMP_IMPORT(mod, func) func;
#else
#include "modding.h"
#endif

#include "PR/ultratypes.h"

RECOMP_IMPORT("mod_fs", void register_model_replacement(s32 id, void *data, u32 size))
RECOMP_IMPORT("mod_fs", void register_block_replacement(s32 tabIdx, void *data, u32 size))
RECOMP_IMPORT("mod_fs", void *get_handle(s32 tabFileId, s32 tabIdx, u32 *outSize))
RECOMP_IMPORT("mod_fs", void *resize_tab_entry(s32 tabFileId, s32 tabIdx, u32 newSize))

#ifndef _MOD_FS_SRC
#define MODFS_ON_INIT_CALLBACK RECOMP_CALLBACK("mod_fs", modfs_on_init)
#endif
