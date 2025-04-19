#pragma once

#include "PR/ultratypes.h"

typedef struct {
    s32 tabIdx;
    void *data;
    u32 size;
} TabEntryReplacement;

typedef struct {
    char *name;
    s32 *tab;
    s32 tabLength;
    s32 *relocs;
    s32 replacementsLength;
    TabEntryReplacement replacements[2000];
} TabModList;

void tab_add_replacement(TabModList *list, s32 tabIdx, void *data, u32 size);
s32 tab_find_idx(TabModList *list, u32 offset);
void tab_read(TabModList *list, s32 romAddr, void *dst, u32 offset, u32 size);
s32 *tab_init(TabModList *list, s32 *originalTab, s32 originalTabLength);
