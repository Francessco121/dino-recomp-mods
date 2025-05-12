#include "recomputils.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"
#include "sys/memory.h"

#include "extfs_common.h"

/**
 * @file tab_ext.c
 * @brief Extendable filesystem tab(le)-based files.
 *
 * Allows mods to modify and extend filesystem files that use .TAB/.BIN pairs.
 */

// 
typedef struct {
    void *data;
    void *fullReplacementData;
    // The size of this individual entry.
    u32 sizeBytes;
    // The maximum number of bytes that can be read starting from the beginning of this entry.
    // For tabs with a stride, this will include all of the entries after this one up to the end of the stride.
    // For tabs without a stride, this will equal [sizeBytes].
    u32 maxReadSizeBytes;
} TabExtEntry;

// 
typedef struct {
    _Bool isSupported;

    const char *name;
    s32 id;
    s32 binId;
    // The number of files that the game reads at a time for this tab. Usually 1.
    // When >1, files within a stride will be merged together when the tab is rebuilt
    // so that the game can read across file boundaries within the stride.
    s32 stride;

    // Length: [entryCount]
    u32 *originalEntries;
    u32 entryCount;
    u32 sizeBytes;
    _Bool hasEndMarker;
    
    // Length: [entryCount]
    TabExtEntry *replacements;
    // Length: [entryCount]
    u32 *rebuiltEntries;
    // For each entry, an offset from the rebuilt entry offset to the original entry offset.
    // Necessary for entries that weren't replaced so the original ROM read can be performed.
    // Length: [entryCount]
    s32 *relocations;
} TabExt;

// A map of file IDs to extended tabs.
// Only supported tab files will be initialized.
static TabExt tabExts[NUM_FILES];
// A map of bin file IDs to their respective tab file ID.
// Unsupported file IDs will return -1.
static s32 binsToTabs[NUM_FILES];

static TabExt *load_tab_ext(s32 fileId);
static TabExtEntry *load_tab_ext_entry(s32 tabFileId, s32 tabIdx, const void *existingData, u32 existingDataSize);
static s32 _read_file_region_base(u32 id, void *dst, u32 offset, s32 size);

static void assert_supported_tab(s32 tabFileId) {
    extfs_assert(tabFileId >= 0 && tabFileId < NUM_FILES, "[extfs] File ID out of bounds: %d", tabFileId);
    extfs_assert(tabExts[tabFileId].isSupported, "[extfs] Unsupported/invalid tab file ID: %d", tabFileId);
}

RECOMP_EXPORT void extfs_tab_set_entry_replacement(s32 tabFileId, s32 tabIdx, const void *data, u32 sizeBytes) {
    extfs_assert(extfsLoadStage == EXTFS_STAGE_REPLACEMENTS, "[extfs] Cannot set replacements outside of replacements load stage/event.");
    assert_supported_tab(tabFileId);

    TabExt *tab = load_tab_ext(tabFileId);

    TabExtEntry *entry = &tab->replacements[tabIdx];
    if (entry->data != NULL) {
        extfs_log_error("[extfs] File %s/%d already has a replacement!\n", tab->name, tabIdx);
        return;
    }

    load_tab_ext_entry(tabFileId, tabIdx, data, sizeBytes);

    extfs_log("[extfs] Set replacement file data for %s/%d.\n", tab->name, tabIdx);
}

RECOMP_EXPORT void *extfs_tab_get_entry(s32 tabFileId, s32 tabIdx, u32 *outSize) {
    extfs_assert(extfsLoadStage == EXTFS_STAGE_MODIFICATIONS, "[extfs] Cannot get tab entries outside of modifications load stage/event.");
    assert_supported_tab(tabFileId);

    TabExtEntry *entry = load_tab_ext_entry(tabFileId, tabIdx, NULL, 0);
    if (outSize != NULL) {
        *outSize = entry->sizeBytes;
    }

    return entry->data;
}

RECOMP_EXPORT void *extfs_tab_resize_entry(s32 tabFileId, s32 tabIdx, u32 newSize) {
    extfs_assert(extfsLoadStage == EXTFS_STAGE_MODIFICATIONS, "[extfs] Cannot resize tab entries outside of modifications load stage/event.");
    assert_supported_tab(tabFileId);

    TabExt *tab = load_tab_ext(tabFileId);
    TabExtEntry *entry = load_tab_ext_entry(tabFileId, tabIdx, NULL, 0);

    void *oldData = entry->data;
    u32 oldSize = entry->sizeBytes;

    entry->data = recomp_alloc(newSize);
    extfs_assert(entry->data, "[extfs] extfs_tab_resize_entry(%d) resize recomp_alloc failed: %d", tabFileId, newSize);

    u32 toCopy = oldSize;
    if (toCopy > newSize) {
        toCopy = newSize; // truncate
    }

    bcopy(oldData, entry->data, toCopy);
    if (toCopy < newSize) {
        bzero((u8*)entry->data + toCopy, newSize - toCopy);
    }
    recomp_free(oldData);

    entry->sizeBytes = newSize;

    extfs_log("[extfs] Resized file data for %s/%d %x -> %x.\n", tab->name, tabIdx, oldSize, newSize);

    return entry->data;
}

static void setup_ext_tab(
    const char *name,
    s32 id,
    s32 binId,
    s32 stride
) {
    TabExt *tab = &tabExts[id];
    tab->name = name;
    tab->id = id;
    tab->binId = binId;
    tab->stride = stride;
    tab->isSupported = TRUE;

    binsToTabs[binId] = id;
}

void tab_ext_init() {
    bzero(tabExts, sizeof(tabExts));

    for (s32 i = 0; i < NUM_FILES; i++) {
        binsToTabs[i] = -1;
    }

    setup_ext_tab("AUDIO", AUDIO_TAB, AUDIO_BIN, /*stride=*/1);
    setup_ext_tab("SFX", SFX_TAB, SFX_BIN, /*stride=*/1);
    setup_ext_tab("AMBIENT", AMBIENT_TAB, AMBIENT_BIN, /*stride=*/1);
    setup_ext_tab("MUSIC", MUSIC_TAB, MUSIC_BIN, /*stride=*/1);
    setup_ext_tab("MPEG", MPEG_TAB, MPEG_BIN, /*stride=*/1);
    setup_ext_tab("ANIMCURVES", ANIMCURVES_TAB, ANIMCURVES_BIN, /*stride=*/1);
    setup_ext_tab("GAMETEXT", GAMETEXT_TAB, GAMETEXT_BIN, /*stride=*/1);
    setup_ext_tab("TABLES", TABLES_TAB, TABLES_BIN, /*stride=*/1);
    setup_ext_tab("SCREENS", SCREENS_TAB, SCREENS_BIN, /*stride=*/1);
    setup_ext_tab("VOXMAP", VOXMAP_TAB, VOXMAP_BIN, /*stride=*/1);
    setup_ext_tab("TEXPRE", TEXPRE_TAB, TEXPRE_BIN, /*stride=*/1);
    setup_ext_tab("MAPS", MAPS_TAB, MAPS_BIN, /*stride=*/7);
    setup_ext_tab("TEX1", TEX1_TAB, TEX1_BIN, /*stride=*/1);
    setup_ext_tab("TEX0", TEX0_TAB, TEX0_BIN, /*stride=*/1);
    setup_ext_tab("BLOCKS", BLOCKS_TAB, BLOCKS_BIN, /*stride=*/1);
    setup_ext_tab("HITS", HITS_TAB, HITS_BIN, /*stride=*/1);
    setup_ext_tab("MODELS", MODELS_TAB, MODELS_BIN, /*stride=*/1);
    setup_ext_tab("MODANIM", MODANIM_TAB, MODANIM_BIN, /*stride=*/1);
    setup_ext_tab("ANIM", ANIM_TAB, ANIM_BIN, /*stride=*/1);
    setup_ext_tab("AMAP", AMAP_TAB, AMAP_BIN, /*stride=*/1);
    setup_ext_tab("VOXOBJ", VOXOBJ_TAB, VOXOBJ_BIN, /*stride=*/1);
    setup_ext_tab("MODLINES", MODLINES_TAB, MODLINES_BIN, /*stride=*/1);
    setup_ext_tab("OBJSEQ", OBJSEQ_TAB, OBJSEQ_BIN, /*stride=*/1);
    setup_ext_tab("OBJECTS", OBJECTS_TAB, OBJECTS_BIN, /*stride=*/1);
}

// Get the extended tab for a tab file ID, loading it for the first time if necessary.
static TabExt *load_tab_ext(s32 fileId) {
    TabExt *tab = &tabExts[fileId];
    extfs_assert(tab->isSupported, "[extfs] load_tab_ext called with unsupported file ID: %d", fileId);

    if (tab->originalEntries == NULL) {
        u32 *fstEntry = fileId + gFST->offsets;
        u32 offset = fstEntry[0];
        u32 size = fstEntry[1] - offset;

        void *originalEntries = recomp_alloc(size);
        extfs_assert(originalEntries != NULL, "[extfs] load_tab_ext(%d) originalEntries recomp_alloc failed: %d", fileId, size);

        read_from_rom((u32)&__file1Address + offset, (u8*)originalEntries, size);
        tab->originalEntries = originalEntries;
        tab->sizeBytes = size;

        u32 entryCount = size / sizeof(u32);
        if (tab->originalEntries[entryCount - 1] == 0xFFFFFFFF) {
            // Has special end marker
            tab->hasEndMarker = TRUE;
            entryCount--;
        }
        // Last number is not a real entry, just there to calculate the size of the prev
        entryCount--;
        tab->entryCount = entryCount;

        extfs_log("[extfs] Loaded original tab %s with %d entries.\n", tab->name, entryCount);
    }

    if (tab->replacements == NULL) {
        u32 replacementsSize = sizeof(TabExtEntry) * tab->entryCount;

        tab->replacements = recomp_alloc(replacementsSize);
        extfs_assert(tab->replacements != NULL, "[extfs] load_tab_ext(%d) replacements recomp_alloc failed: %d", fileId, replacementsSize);

        bzero(tab->replacements, replacementsSize);
    }

    return tab;
}

static TabExtEntry *load_tab_ext_entry(s32 tabFileId, s32 tabIdx, const void *existingData, u32 existingDataSize) {
    TabExt *tab = load_tab_ext(tabFileId);

    TabExtEntry *entry = &tab->replacements[tabIdx];
    if (entry->data == NULL) {
        u32 binAddr = gFST->offsets[tab->binId];

        if (tab->stride == 1) {
            if (existingData == NULL) {
                // No stride, no existing data hint
                u32 offset = tab->originalEntries[tabIdx];
                u32 size = existingData == NULL ? tab->originalEntries[tabIdx + 1] - offset : existingDataSize;

                entry->sizeBytes = size;
                entry->data = recomp_alloc(size);
                extfs_assert(entry->data != NULL, "[extfs] load_tab_ext_entry(%d, %d) entry data recomp_alloc failed: %d", 
                    tabFileId, tabIdx, size);

                read_from_rom((u32)&__file1Address + binAddr + offset, entry->data, size);
                
                extfs_log("[extfs] Loaded original file data for %s/%d.\n", tab->name, tabIdx);
            } else {
                // No stride, has existing data hint
                entry->sizeBytes = existingDataSize;
                entry->data = recomp_alloc(existingDataSize);
                extfs_assert(entry->data != NULL, "[extfs] load_tab_ext_entry(%d, %d) entry data recomp_alloc failed: %d", 
                    tabFileId, tabIdx, existingDataSize);
                
                bcopy(existingData, entry->data, existingDataSize);

                extfs_log("[extfs] Loaded original file data (with replacement) for %s/%d.\n", tab->name, tabIdx);
            }
        } else {
            // Read all entries in stride
            s32 baseTabIdx = (tabIdx / tab->stride) * tab->stride;
            u32 fullStrideSize = tab->originalEntries[baseTabIdx + tab->stride] - tab->originalEntries[baseTabIdx];
            u32 baseoffset = tab->originalEntries[baseTabIdx];

            void *fullStrideData = recomp_alloc(fullStrideSize);
            extfs_assert(fullStrideData != NULL, "[extfs] load_tab_ext_entry(%d, %d) fullStrideData recomp_alloc failed: %d", 
                tabFileId, tabIdx, fullStrideSize);

            read_from_rom((u32)&__file1Address + binAddr + baseoffset, fullStrideData, fullStrideSize);

            // Break up into individual entry data
            u32 fullStrideOffset = 0;
            for (s32 i = 0; i < tab->stride; i++) {
                TabExtEntry *_entry = &tab->replacements[baseTabIdx + i];

                u32 offset = tab->originalEntries[baseTabIdx + i];
                u32 originalEntrySize = tab->originalEntries[baseTabIdx + i + 1] - offset;

                if (existingData == NULL || _entry != entry) {
                    // No existing data or not requested entry, use original data
                    _entry->sizeBytes = originalEntrySize;
                    _entry->data = recomp_alloc(originalEntrySize);
                    extfs_assert(_entry->data != NULL, "[extfs] load_tab_ext_entry(%d, %d) entry data recomp_alloc failed: %d", 
                        tabFileId, tabIdx, originalEntrySize);

                    bcopy((void*)((u32)fullStrideData + fullStrideOffset), _entry->data, originalEntrySize);
                } else {
                    // Existing data and request entry, use existing data hint
                    _entry->sizeBytes = existingDataSize;
                    _entry->data = recomp_alloc(existingDataSize);
                    extfs_assert(_entry->data != NULL, "[extfs] load_tab_ext_entry(%d, %d) entry data recomp_alloc failed: %d", 
                        tabFileId, tabIdx, existingDataSize);
                    
                    bcopy(existingData, _entry->data, existingDataSize);
                }

                fullStrideOffset += originalEntrySize;
            }

            if (fullStrideOffset != fullStrideSize) {
                extfs_log_error("[extfs] load_tab_ext_entry(%d, %d) Leftovers from breaking up stride! fullStrideOffset: %x, fullStrideSize: %x\n", 
                    tabFileId, tabIdx, fullStrideOffset, fullStrideSize);
            }

            // Clean up
            recomp_free(fullStrideData);

            if (existingData == NULL) {
                extfs_log("[extfs] Loaded original file data for %s/%d-%d.\n", tab->name, baseTabIdx, baseTabIdx + tab->stride);
            } else {
                extfs_log("[extfs] Loaded original file data (with replacement for %d) for %s/%d-%d.\n", tabIdx, tab->name, baseTabIdx, baseTabIdx + tab->stride);
            }
        }
    }

    return entry;
}

static void rebuild_tab(TabExt *tab) {
    // Rebuild tab with replacements in mind
    s32 *relocs = recomp_alloc(tab->entryCount * sizeof(s32));
    extfs_assert(relocs != NULL, "[extfs] rebuild_tab %d relocs recomp_alloc failed: %d", 
        tab->id, tab->entryCount * sizeof(s32));
    tab->relocations = relocs;

    u32 *newTab = malloc(tab->sizeBytes, ALLOC_TAG_FS_COL, NULL);
    extfs_assert(newTab != NULL, "[extfs] rebuild_tab %d newTab malloc failed: %d", 
        tab->id, tab->sizeBytes);
    tab->rebuiltEntries = newTab;

    u32 newOffset = 0;
    s32 hasEndMarker = FALSE;

    for (u32 i = 0; i < tab->entryCount; i++) {
        u32 originalOffset = tab->originalEntries[i];
        u32 originalSize = tab->originalEntries[i + 1] - originalOffset;

        newTab[i] = newOffset;
        // Calculate relocations to convert new ROM offsets to original ROM offsets for unmodified files
        relocs[i] = (s32)originalOffset - (s32)newOffset;

        if (tab->replacements[i].data != NULL) {
            newOffset += tab->replacements[i].sizeBytes;
        } else {
            newOffset += originalSize;
        }
    }

    newTab[tab->entryCount] = newOffset;
    if (tab->hasEndMarker) {
        newTab[tab->entryCount + 1] = 0xFFFFFFFF;
    }

    // Merge stride data
    if (tab->stride > 1) {
        for (u32 i = 0; i < tab->entryCount; i += tab->stride) {
            if (tab->replacements[i].data == NULL) {
                continue;
            }
    
            u32 fullStrideSize = tab->rebuiltEntries[i + tab->stride] - tab->rebuiltEntries[i];
            void *mergedData = recomp_alloc(fullStrideSize);
            extfs_assert(mergedData != NULL, "[extfs] merge_tab_ext_strides %d mergedData recomp_alloc failed: %d", tab->id, fullStrideSize);
    
            u32 sizeLeft = fullStrideSize;
            for (s32 k = 0; k < tab->stride; k++) {
                TabExtEntry *entry = &tab->replacements[i + k];
    
                bcopy(entry->data, mergedData, entry->sizeBytes);
                recomp_free(entry->data);
    
                entry->data = mergedData;
                entry->maxReadSizeBytes = sizeLeft;
    
                mergedData = (void*)((u32)mergedData + entry->sizeBytes);
                sizeLeft -= entry->sizeBytes;
            }
    
            extfs_log("[extfs] Merged data for %s/%d-%d.\n", tab->name, i, i + tab->stride);
        }
    } else {
        for (u32 i = 0; i < tab->entryCount; i++) {
            TabExtEntry *entry = &tab->replacements[i];
            if (entry->data != NULL) {
                entry->maxReadSizeBytes = entry->sizeBytes;
            }
        }
    }

    extfs_log("[extfs] Rebuilt tab %s.\n", tab->name);
}

void tab_ext_rebuild_tabs() {
    for (s32 i = 0; i < NUM_FILES; i++) {
        TabExt *tab = &tabExts[i];
        if (tab->originalEntries != NULL) {
            rebuild_tab(tab);
        }
    }
}

_Bool tab_ext_get_rebuilt_entries(u32 tabFileId, void **outRebuiltEntries) {
    if (tabFileId >= 0 && tabFileId < NUM_FILES) {
        TabExt *tab = &tabExts[tabFileId];
        if (tab->rebuiltEntries != NULL) {
            *outRebuiltEntries = tab->rebuiltEntries;
            return TRUE;
        }
    }

    return FALSE;
}

static s32 tab_find_idx(const TabExt *tab, u32 offset) {
    for (u32 i = 0; i < tab->entryCount; i++) {
        u32 start = tab->rebuiltEntries[i];
        u32 end = tab->rebuiltEntries[i + 1];

        if (offset >= start && offset < end) {
            return i;
        }
    }

    return -1;
}

_Bool tab_ext_try_read_bin(s32 binFileId, void *dst, u32 offset, u32 size) {
    s32 tabFileId = binsToTabs[binFileId];
    if (tabFileId == -1) {
        return FALSE;
    }

    TabExt *tab = &tabExts[tabFileId];
    if (tab->rebuiltEntries == NULL) {
        return FALSE;
    }

    // Find tab index of the file referenced by the offset
    s32 tabIdx = tab_find_idx(tab, offset);
    if (tabIdx == -1) {
        extfs_error_box("[extfs] Couldn't find tab index for offset 0x%x in %s. Bad things to follow!\n", offset, tab->name);
        // We don't have the correct ROM address to read, just return all zeroes
        bzero(dst, size);
        return TRUE;
    }

    // Find the replacement for the file, if any
    TabExtEntry *replacement = &tab->replacements[tabIdx];

    if (replacement->data == NULL) {
        // Adjust ROM address back to original
        s32 reloc = tab->relocations[tabIdx];
        _read_file_region_base(binFileId, dst, offset + reloc, size);
    } else {
        // Read from replacement file
        u32 relativeOffset = offset - tab->rebuiltEntries[tabIdx];
        u32 availableSize = replacement->maxReadSizeBytes - relativeOffset;
        if (size > availableSize) {
            extfs_error_box("[extfs] Replacement not big enough for %s[%d]! Requested size: %x, Available size: %x",
                tab->name, tabIdx, size, availableSize);
        }

        bcopy((void*)((u32)replacement->data + relativeOffset), dst, size);

        extfs_log("[extfs] Using replacement for %s[%d]\n", tab->name, tabIdx);
    }

    return TRUE;
}

// Copy of the original read_file_region
static s32 _read_file_region_base(u32 id, void *dst, u32 offset, s32 size)
{
    s32 fileAddr;
    u32 * tmp;

    if (size == 0 || id > (u32)gFST->fileCount)
        return 0;

    tmp      = ++id + gFST->offsets - 1;
    fileAddr = *tmp + offset;

    gLastFSTIndex = id;

    read_from_rom(fileAddr + (s32)&__file1Address, dst, size);

    return size;
}
