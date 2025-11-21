#pragma once

#include "PR/ultratypes.h"

void fst_ext_set_file(s32 fileID, void *data, u32 size, _Bool ownedByRepacker);
u32 fst_ext_get_file_size(s32 fileID);
void fst_ext_read_from_file(s32 fileID, void *dst, u32 offset, u32 size);
s32 fst_ext_audio_dma(void *dst, u32 rom, u32 size);
