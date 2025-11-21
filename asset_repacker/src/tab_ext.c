#include "tab_ext.h"

#include "recomputils.h"
#include "repacker_common.h"
#include "fst_ext.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"
#include "sys/memory.h"

static void resize_tab(TabExt *tab, u32 newCount) {
    repacker_assert(tab->replacements.entries == NULL, "[repacker] Cannot resize rebuilt tab: %s!", tab->name);

    if (newCount < tab->replacements.count) {
        // Shrink

        // Free entries that will no longer be accessible
        for (u32 i = newCount; i < tab->replacements.count; i++) {
            TabExtEntry *entry = &tab->replacements.entries[i];
            if (entry->data != NULL) {
                recomp_free(entry->data);
                entry->data = NULL;
            }
        }

        repacker_log("[repacker] Resized tab %s: %d -> %d\n", tab->name, tab->replacements.count, newCount);

        tab->replacements.count = newCount;
        tab->isDirty = TRUE;
    } else if (newCount > tab->replacements.count) {
        // Expand

        if (newCount > tab->replacements.capacity) {
            // Doesn't fit within existing capacity, allocate a new array
            TabExtEntry *newReplacements = recomp_alloc(newCount * sizeof(TabExtEntry));
            repacker_assert(newReplacements != NULL, "[repacker] resize_tab recomp_alloc failed: %d", tab->id, newCount * sizeof(TabExtEntry));
            
            // Copy over old entries
            bcopy(tab->replacements.entries, newReplacements, tab->replacements.count * sizeof(TabExtEntry));
            // Zero out new entries
            bzero(newReplacements + tab->replacements.count, (newCount - tab->replacements.count) * sizeof(TabExtEntry));

            recomp_free(tab->replacements.entries);
            tab->replacements.entries = newReplacements;

            tab->replacements.capacity = newCount;
        }

        repacker_log("[repacker] Resized tab %s: %d -> %d\n", tab->name, tab->replacements.count, newCount);

        tab->replacements.count = newCount;
        tab->isDirty = TRUE;
    }
}

static TabExtEntry *get_tab_entry(TabExt *tab, s32 tabIdx) {
    repacker_assert(tabIdx >= 0 && (u32)tabIdx < tab->replacements.count, 
        "[repacker] Attempted to get out of bounds tab index given for tab %s: %d  %s", tab->name, tabIdx);

    TabExtEntry *entry = &tab->replacements.entries[tabIdx];

    if (entry->data == NULL) {
        // Entry has no replacement
        if ((u32)tabIdx < tab->original.count) {
            // Load original contents into entry
            u32 offset = tab->original.entries[tabIdx];
            u32 size = tab->original.entries[tabIdx + 1] - offset;

            entry->data = recomp_alloc(size);
            entry->sizeBytes = size;
            entry->capacityBytes = size;

            repacker_assert(entry->data != NULL, "[repacker] get_tab_entry(%d) original buffer recomp_alloc failed: %d", tab->id, size);

            fst_ext_read_from_file(tab->binId, entry->data, offset, size);
        }
    }

    tab->isDirty = TRUE;

    return entry;
}

void tab_ext_set_entry_replacement(TabExt *tab, s32 tabIdx, const void *data, u32 sizeBytes) {
    repacker_assert(repackerStage == REPACKER_STAGE_SUBFILE_REPLACEMENTS, "[repacker] Cannot set replacements outside of replacements load stage/event.");
    repacker_assert(tabIdx >= 0, "[repacker] Negative tab index given for tab %s: %d", tab->name, tabIdx);

    if ((u32)tabIdx >= tab->replacements.count) {
        resize_tab(tab, tabIdx + 1);
    }

    TabExtEntry *entry = &tab->replacements.entries[tabIdx];

    if (entry->data == NULL) {
        // Entry not yet replaced, allocate initial buffer
        entry->data = recomp_alloc(sizeBytes);
        entry->capacityBytes = sizeBytes;
        
        repacker_assert(entry->data != NULL, "[repacker] tab_ext_set_entry_replacement(%d) initial recomp_alloc failed: %d", tab->id, sizeBytes);
    } else if (sizeBytes > entry->capacityBytes) {
        // Not enough room in current replacement buffer, reallocate it
        recomp_free(entry->data);
        entry->data = recomp_alloc(sizeBytes);
        entry->capacityBytes = sizeBytes;
        
        repacker_assert(entry->data != NULL, "[repacker] tab_ext_set_entry_replacement(%d) reallocate recomp_alloc failed: %d", tab->id, sizeBytes);
    }

    // Update replacement data
    bcopy(data, entry->data, sizeBytes);
    entry->sizeBytes = sizeBytes;

    tab->isDirty = TRUE;

    repacker_log("[repacker] Set replacement file data for %s/%d.\n", tab->name, tabIdx);
}

void *tab_ext_get_entry(TabExt *tab, s32 tabIdx, u32 *outSize) {
    repacker_assert(repackerStage == REPACKER_STAGE_MODIFICATIONS, "[repacker] Cannot get tab entries outside of modifications load stage/event.");
    repacker_assert(tabIdx >= 0, "[repacker] Negative tab index given for tab %s: %d", tab->name, tabIdx);

    if ((u32)tabIdx >= tab->replacements.count) {
        resize_tab(tab, tabIdx + 1);
    }

    TabExtEntry *entry = get_tab_entry(tab, tabIdx);
    if (outSize != NULL) {
        *outSize = entry->sizeBytes;
    }

    tab->isDirty = TRUE;

    return entry->data;
}

void *tab_ext_resize_entry(TabExt *tab, s32 tabIdx, u32 newSize) {
    repacker_assert(repackerStage == REPACKER_STAGE_MODIFICATIONS, "[repacker] Cannot resize tab entries outside of modifications load stage/event.");
    repacker_assert(tabIdx >= 0, "[repacker] Negative tab index given for tab %s: %d", tab->name, tabIdx);

    if ((u32)tabIdx >= tab->replacements.count) {
        resize_tab(tab, tabIdx + 1);
    }

    TabExtEntry *entry = get_tab_entry(tab, tabIdx);

    if (newSize < entry->sizeBytes) {
        // Shrink
        repacker_log("[repacker] Resized entry data for %s/%d %x -> %x.\n", tab->name, tabIdx, entry->sizeBytes, newSize);

        entry->sizeBytes = newSize;
    } else if (newSize > entry->sizeBytes) {
        // Grow
        if (newSize > entry->capacityBytes) {
            // Need more space
            void *newData = recomp_alloc(newSize);
            repacker_assert(newData != NULL, "[repacker] tab_ext_resize_entry(%d) resize recomp_alloc failed: %d", tab->id, newSize);
            
            bcopy(entry->data, newData, entry->sizeBytes);
            recomp_free(entry->data);
            
            entry->data = newData;
            entry->capacityBytes = newSize;
        }

        repacker_log("[repacker] Resized entry data for %s/%d %x -> %x.\n", tab->name, tabIdx, entry->sizeBytes, newSize);

        entry->sizeBytes = newSize;
    }

    tab->isDirty = TRUE;

    return entry->data;
}

void tab_ext_init(TabExt *tab) {
    repacker_assert(tab->original.entries == NULL && tab->replacements.entries == NULL, 
        "[repacker] tab_ext_init: Tab %s already initialized!", tab->name);
        
    tab->isDirty = FALSE;

    // Load original entries
    u32 size = fst_ext_get_file_size(tab->id);

    void *originalEntries = recomp_alloc(size);
    repacker_assert(originalEntries != NULL, "[repacker] load_tab_ext(%d) originalEntries recomp_alloc failed: %d", tab->id, size);

    fst_ext_read_from_file(tab->id, originalEntries, 0, size);
    tab->original.entries = originalEntries;

    u32 entryCount = size / sizeof(u32);
    if (entryCount > 0 && tab->original.entries[entryCount - 1] == 0xFFFFFFFF) {
        // Has special end marker
        tab->hasEndMarker = TRUE;
        entryCount--;
    }
    // Last number is not a real entry, just there to calculate the size of the prev
    entryCount--;
    tab->original.count = entryCount;

    // Setup initial replacements list
    u32 replacementsSize = sizeof(TabExtEntry) * tab->original.count;

    tab->replacements.entries = recomp_alloc(replacementsSize);
    repacker_assert(tab->replacements.entries != NULL, 
        "[repacker] load_tab_ext(%d) replacements recomp_alloc failed: %d", tab->id, replacementsSize);

    bzero(tab->replacements.entries, replacementsSize);
    
    tab->replacements.capacity = tab->original.count;
    tab->replacements.count = tab->original.count;

    repacker_log("[repacker] Initialized tab_ext for %s with %d entries.\n", tab->name, entryCount);
}

void tab_ext_rebuild(TabExt *tab) {
    // Skip if not modified
    if (tab->isDirty) {
        // Rebuild tab
        u32 tabSizeBytes = (tab->replacements.count + 1) * sizeof(u32);
        if (tab->hasEndMarker) {
            tabSizeBytes += sizeof(u32);
        }

        u32 *newTab = recomp_alloc(tabSizeBytes);
        repacker_assert(newTab != NULL, "[repacker] rebuild_tab %d newTab recomp_alloc failed: %d", tab->id, tabSizeBytes);
        
        u32 offset = 0;
        for (u32 i = 0; i < tab->replacements.count; i++) {
            newTab[i] = offset;

            u32 entrySize = 0;
            if (tab->replacements.entries[i].data != NULL) {
                // Replaced file
                entrySize = tab->replacements.entries[i].sizeBytes;
            } else {
                if (i < tab->original.count) {
                    // Use original size
                    entrySize = tab->original.entries[i + 1] - tab->original.entries[i];
                } else {
                    // No replacement or vanilla data, zero size
                    entrySize = 0;
                }
            }

            offset += entrySize;

        }

        newTab[tab->replacements.count] = offset;
        if (tab->hasEndMarker) {
            newTab[tab->replacements.count + 1] = 0xFFFFFFFF;
        }

        fst_ext_set_file(tab->id, newTab, tabSizeBytes, /*ownedByRepacker=*/TRUE);

        repacker_log("[repacker] Rebuilt %s.tab.\n", tab->name);

        // Rebuild bin
        u32 binSizeBytes = offset;

        u8 *newBin = recomp_alloc(binSizeBytes);
        repacker_assert(newTab != NULL, "[repacker] rebuild_tab %d newBin recomp_alloc failed: %d", tab->id, binSizeBytes);
        bzero(newBin, binSizeBytes);

        // Note: ROM reads must be to aligned RDRAM addresses. So, we'll read original entries
        // into a temp buffer that always has an aligned address, since the destination in 
        // the bin file might not be an aligned address.
        void *tempBuffer = NULL;
        u32 tempBufferCapacity = 0;

        for (u32 i = 0; i < tab->replacements.count; i++) {
            u32 entryOffset = newTab[i];

            TabExtEntry *entry = &tab->replacements.entries[i];

            if (entry->data != NULL) {
                // Replaced file
                bcopy(entry->data, newBin + entryOffset, entry->sizeBytes);
            } else {
                if (i < tab->original.count) {
                    // Original file
                    u32 originalOffset = tab->original.entries[i];
                    u32 originalSize = tab->original.entries[i + 1] - originalOffset;

                    if (tempBuffer == NULL || originalSize > tempBufferCapacity) {
                        if (tempBuffer != NULL) {
                            recomp_free(tempBuffer);
                        }
                        tempBuffer = recomp_alloc(originalSize);
                        tempBufferCapacity = originalSize;
                        repacker_assert(newTab != NULL, "[repacker] rebuild_tab %d temp buffer recomp_alloc failed: %d", tab->id, originalSize);
                    }

                    fst_ext_read_from_file(tab->binId, tempBuffer, originalOffset, originalSize);
                    bcopy(tempBuffer, newBin + entryOffset, originalSize);
                } else {
                    // No replacement or vanilla data
                }
            }
        }

        if (tempBuffer != NULL) {
            recomp_free(tempBuffer);
            tempBuffer = NULL;
        }

        fst_ext_set_file(tab->binId, newBin, binSizeBytes, /*ownedByRepacker=*/TRUE);

        repacker_log("[repacker] Rebuilt %s.bin.\n", tab->name);
    } else {
        repacker_log("[repacker] Not rebuilding %s (wasn't modified).\n", tab->name);
    }

    // Clean up
    recomp_free(tab->original.entries);
    tab->original.entries = NULL;

    recomp_free(tab->replacements.entries);
    tab->replacements.entries = NULL;
}
