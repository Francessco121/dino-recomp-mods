#pragma once

#include "PR/ultratypes.h"

typedef struct {
    // If NULL, this entry is not replaced.
    void *data;
    // The size of this individual entry.
    u32 sizeBytes;
    // The allocated space of this individual entry.
    //
    // If an entry shrinks, it will not be re-allocated and instead the
    // [sizeBytes] field will just be lowered.
    u32 capacityBytes;
} TabExtEntry;

typedef struct {
    const char *name;
    s32 id;
    s32 binId;

    struct {
        u32 *entries;
        u32 count;
    } original;
    _Bool hasEndMarker;
    
    struct {
        TabExtEntry *entries;
        u32 count;
        u32 capacity;
    } replacements;

    _Bool isDirty;
} TabExt;

void tab_ext_init(TabExt *tab);
void tab_ext_set_entry_replacement(TabExt *tab, s32 tabIdx, const void *data, u32 sizeBytes);
void *tab_ext_get_entry(TabExt *tab, s32 tabIdx, u32 *outSize);
void *tab_ext_resize_entry(TabExt *tab, s32 tabIdx, u32 newSize);
void tab_ext_rebuild(TabExt *tab);
