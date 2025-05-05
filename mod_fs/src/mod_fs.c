#include "PR/os.h"
#define _MOD_FS_SRC
#include "mod_fs.h"

#include "recomputils.h"
#include "modding.h"

//#include "tab.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"
#include "sys/memory.h"

extern void bcopy_recomp(const void *src, void *dst, int length);
#define bcopy bcopy_recomp

extern void bzero_recomp(void *, int);
#define bzero bzero_recomp

extern void free_recomp(void* p);
#define free free_recomp

RECOMP_DECLARE_EVENT(modfs_on_init());

extern Fs * gFST;
extern u32 gLastFSTIndex;
extern s32 __fstAddress;
extern s32 __file1Address;

extern void read_from_rom(u32 romAddr, u8* dst, s32 size);

static s32 _read_file_region_base(u32 id, void *dst, u32 offset, s32 size);

//static TabModList modelsList = { .name = "MODELS.TAB" };
//static TabModList blocksList = { .name = "BLOCKS.TAB" };

/*
- get_handle
- load tab into recomp heap (if not already)
- load specific file of bin file into recomp heap (if not already)
- resize if more space was requested (malloc, bcopy, free)
- keep track of loaded files (not the whole .bin)
- return pointer to it


- on init_filesystem:
    - call event to let mods make edits and register replacements (ONLY allow during this period)
        - must use load events to make further edits
    - rebuild tab replacements


- on read_alloc_file:
    - return tab replacement or original
- on read_file_region:
    - return file replacement of original

TODO: allow mods to resize/shrink files
TODO: allow mods to see current file size

*/

typedef struct {
    s32 stride;
    const char *name;
} TabConfig;

TabConfig tabConfigs[NUM_FILES];
s32 tabsToBins[NUM_FILES];
s32 binsToTabs[NUM_FILES];

static void add_tab_bin_pair(s32 tabFileId, s32 binFileId, s32 stride, const char *name) {
    tabsToBins[tabFileId] = binFileId;
    binsToTabs[binFileId] = tabFileId;

    tabConfigs[tabFileId].stride = stride;
    tabConfigs[tabFileId].name = name;
}

static void init_tab_bin_pair_maps() {
    for (s32 i = 0; i < NUM_FILES; i++) {
        tabsToBins[i] = -1;
        binsToTabs[i] = -1;
    }

    add_tab_bin_pair(AUDIO_TAB, AUDIO_BIN, 1, "AUDIO");
    add_tab_bin_pair(SFX_TAB, SFX_BIN, 1, "SFX");
    add_tab_bin_pair(AMBIENT_TAB, AMBIENT_BIN, 1, "AMBIENT");
    add_tab_bin_pair(MUSIC_TAB, MUSIC_BIN, 1, "MUSIC");
    add_tab_bin_pair(MPEG_TAB, MPEG_BIN, 1, "MPEG");
    add_tab_bin_pair(ANIMCURVES_TAB, ANIMCURVES_BIN, 1, "ANIMCURVES");
    add_tab_bin_pair(GAMETEXT_TAB, GAMETEXT_BIN, 1, "GAMETEXT");
    add_tab_bin_pair(TABLES_TAB, TABLES_BIN, 1, "TABLES");
    add_tab_bin_pair(SCREENS_TAB, SCREENS_BIN, 1, "SCREENS");
    add_tab_bin_pair(VOXMAP_TAB, VOXMAP_BIN, 1, "VOXMAP");
    add_tab_bin_pair(TEXPRE_TAB, TEXPRE_BIN, 1, "TEXPRE");
    add_tab_bin_pair(MAPS_TAB, MAPS_BIN, 7, "MAPS");
    add_tab_bin_pair(TEX1_TAB, TEX1_BIN, 1, "TEX1");
    add_tab_bin_pair(TEX0_TAB, TEX0_BIN, 1, "TEX0");
    add_tab_bin_pair(BLOCKS_TAB, BLOCKS_BIN, 1, "BLOCKS");
    add_tab_bin_pair(HITS_TAB, HITS_BIN, 1, "HITS");
    add_tab_bin_pair(MODELS_TAB, MODELS_BIN, 1, "MODELS");
    add_tab_bin_pair(MODANIM_TAB, MODANIM_BIN, 1, "MODELANIM");
    add_tab_bin_pair(ANIM_TAB, ANIM_BIN, 1, "ANIM");
    add_tab_bin_pair(AMAP_TAB, AMAP_BIN, 1, "AMAP");
    add_tab_bin_pair(VOXOBJ_TAB, VOXOBJ_BIN, 1, "VOXOBJ");
    add_tab_bin_pair(MODLINES_TAB, MODLINES_BIN, 1, "MODLINES");
    add_tab_bin_pair(SAVEGAME_TAB, SAVEGAME_BIN, 1, "SAVEGAME");
    add_tab_bin_pair(OBJSEQ_TAB, OBJSEQ_BIN, 1, "OBJSEQ");
    add_tab_bin_pair(OBJECTS_TAB, OBJECTS_BIN, 1, "OBJECTS");
}

typedef struct {
    void *data;
    u32 size;
    u32 availableSize;
} TabEntry;

typedef struct {
    u32 *entries;
    u32 size;
    u32 entryCount;
    s32 hasEndMarker;
    TabEntry *replacements;
    s32 *relocs;
    u32 *rebuiltEntries;
    s32 stride;
} Tab;

static Tab tabFiles[NUM_FILES] = {0};
//static TabEntry *tabFileEntries[NUM_FILES] = {0};

static Tab *get_original_tab_file(s32 fileId) {
    Tab *tabFile = &tabFiles[fileId];

    if (tabFile->entries == NULL) {
        u32 *fstEntry = fileId + gFST->offsets;
        u32 offset = fstEntry[0];
        u32 size = fstEntry[1] - offset;

        void *tabFileData = recomp_alloc(size);
        if (tabFileData == NULL) {
            recomp_exit_with_error("get_original_tab_file tab data recomp_alloc failed");
            return NULL;
        }

        read_from_rom((u32)&__file1Address + offset, (u8*)tabFileData, size);
        tabFile->entries = tabFileData;
        tabFile->size = size;

        u32 entryCount = size / sizeof(u32);
        if (tabFile->entries[entryCount - 1] == 0xFFFFFFFF) {
            // Has special end marker
            tabFile->hasEndMarker = TRUE;
            entryCount--;
        }
        // Last number is not a real entry, just there to calculate the size of the prev
        entryCount--;
        tabFile->entryCount = entryCount;

        tabFile->stride = tabConfigs[fileId].stride;

        recomp_printf("[mod_fs] Loaded original tab %s with %d entries.\n", tabConfigs[fileId].name, entryCount);
    }

    if (tabFile->replacements == NULL) {
        u32 replacementsSize = sizeof(TabEntry) * tabFile->entryCount;

        tabFile->replacements = recomp_alloc(replacementsSize);
        if (tabFile->replacements == NULL) {
            recomp_exit_with_error("get_original_tab_file replacements recomp_alloc failed");
            return NULL;
        }

        bzero(tabFile->replacements, replacementsSize);
    }

    return tabFile;
}

static TabEntry *get_original_tab_entry(s32 tabFileId, s32 tabIdx) {
    Tab *tab = get_original_tab_file(tabFileId);
    if (tab == NULL) {
        return NULL;
    }

    TabEntry *entry = &tab->replacements[tabIdx];
    if (entry->data == NULL) {
        s32 binFileId = tabsToBins[tabFileId];
        u32 binAddr = gFST->offsets[binFileId];

        // Read all entries in stride
        s32 baseTabIdx = (tabIdx / tab->stride) * tab->stride;
        u32 fullStrideSize = tab->entries[baseTabIdx + tab->stride] - tab->entries[baseTabIdx];
        u32 baseoffset = tab->entries[baseTabIdx];

        void *fullStrideData = recomp_alloc(fullStrideSize);
        if (fullStrideData == NULL) {
            recomp_exit_with_error("get_original_tab_entry full stride data recomp_alloc failed");
            return NULL;
        }

        read_from_rom((u32)&__file1Address + binAddr + baseoffset, fullStrideData, fullStrideSize);

        // Break up into individual entry data
        u32 fullStrideOffset = 0;
        for (s32 i = 0; i < tab->stride; i++) {
            entry = &tab->replacements[baseTabIdx + i];

            u32 offset = tab->entries[baseTabIdx + i];
            u32 size = tab->entries[baseTabIdx + i + 1] - offset;

            entry->data = recomp_alloc(size);
            if (entry->data == NULL) {
                recomp_exit_with_error("get_original_tab_entry data recomp_alloc failed");
                return NULL;
            }

            entry->size = size;

            bcopy((void*)((u32)fullStrideData + fullStrideOffset), entry->data, size);
            fullStrideOffset += size;
        }

        if (fullStrideOffset != fullStrideSize) {
            recomp_error_message_box(
                recomp_sprintf_helper("!!! fullStrideOffset != fullStrideSize  %x != %x \n", fullStrideOffset, fullStrideSize));
        }

        // Clean up
        recomp_free(fullStrideData);

        if (tab->stride == 1) {
            recomp_printf("[mod_fs] Loaded original file data for %s/%d.\n", tabConfigs[tabFileId].name, tabIdx);
        } else {
            recomp_printf("[mod_fs] Loaded original file data for %s/%d-%d.\n", tabConfigs[tabFileId].name, baseTabIdx, baseTabIdx + tab->stride);
        }

        entry = &tab->replacements[tabIdx];
    }

    return entry;
}

RECOMP_EXPORT void *resize_tab_entry(s32 tabFileId, s32 tabIdx, u32 newSize) {
    TabEntry *entry = get_original_tab_entry(tabFileId, tabIdx);

    void *oldData = entry->data;
    u32 oldSize = entry->size;

    entry->data = recomp_alloc(newSize);
    if (entry->data == NULL) {
        recomp_exit_with_error("get_original_tab_entry data resize recomp_alloc failed");
        entry->data = oldData;
        return oldData;
    }

    u32 toCopy = oldSize;
    if (toCopy > newSize) {
        toCopy = newSize; // truncate
    }

    bcopy(oldData, entry->data, toCopy);
    if (toCopy < newSize) {
        bzero((u8*)entry->data + toCopy, newSize - toCopy);
    }
    recomp_free(oldData);

    entry->size = newSize;

    recomp_printf("[mod_fs] Resized file data for %s/%d %x -> %x.\n", tabConfigs[tabFileId].name, tabIdx, oldSize, newSize);

    return entry->data;
}

static void set_tab_entry_replacement(s32 tabFileId, s32 tabIdx, void *data, u32 size) {
    Tab *tab = get_original_tab_file(tabFileId);
    if (tab == NULL) {
        return;
    }

    TabEntry *entry = &tab->replacements[tabIdx];
    if (entry->data != NULL) {
        return;
    }

    entry->data = data;
    entry->size = size;

    recomp_printf("[mod_fs] Set replacement file data for %s/%d.\n", tabConfigs[tabFileId].name, tabIdx);
}

static void merge_tab_strides(Tab *tab, s32 tabFileId) {
    for (u32 i = 0; i < tab->entryCount; i += tab->stride) {
        if (tab->replacements[i].data == NULL) {
            continue;
        }

        u32 fullStrideSize = tab->rebuiltEntries[i + tab->stride] - tab->rebuiltEntries[i];
        void *mergedData = recomp_alloc(fullStrideSize);
        if (mergedData == NULL) {
            recomp_exit_with_error("merge_tab_strides recomp_alloc failed");
            return;
        }

        u32 sizeLeft = fullStrideSize;
        for (s32 k = 0; k < tab->stride; k++) {
            TabEntry *entry = &tab->replacements[i + k];

            bcopy(entry->data, mergedData, entry->size);
            recomp_free(entry->data);

            entry->data = mergedData;
            entry->availableSize = sizeLeft;

            mergedData = (void*)((u32)mergedData + entry->size);
            sizeLeft -= entry->size;
        }

        recomp_printf("[mod_fs] Merged data for %s/%d-%d.\n", tabConfigs[tabFileId].name, i, i + tab->stride);
    }
}

static void rebuild_tab(s32 fileId) {
    Tab *tab = &tabFiles[fileId];

    if (tab->entries == NULL) {
        // No replacements or edits exist, just do a normal ROM read like the game does
        return;
    }

    // Rebuild tab with replacements in mind
    s32 *relocs = recomp_alloc(tab->entryCount * sizeof(s32));
    tab->relocs = relocs;

    u32 *newTab = malloc(tab->size, ALLOC_TAG_FS_COL, NULL);
    tab->rebuiltEntries = newTab;

    u32 newOffset = 0;
    s32 hasEndMarker = FALSE;

    for (u32 i = 0; i < tab->entryCount; i++) {
        u32 originalOffset = tab->entries[i];
        u32 originalSize = tab->entries[i + 1] - originalOffset;

        newTab[i] = newOffset;
        // Calculate relocations to convert new ROM offsets to original ROM offsets for unmodified files
        relocs[i] = (s32)originalOffset - (s32)newOffset;

        if (tab->replacements[i].data != NULL) {
            newOffset += tab->replacements[i].size;
        } else {
            newOffset += originalSize;
        }
    }

    newTab[tab->entryCount] = newOffset;
    if (tab->hasEndMarker) {
        newTab[tab->entryCount + 1] = 0xFFFFFFFF;
    }

    recomp_printf("[mod_fs] Rebuilt tab %s.\n", tabConfigs[fileId].name);

    if (tab->stride != 1) {
        merge_tab_strides(tab, fileId);
    }
}

s32 tab_find_idx2(Tab *tab, u32 offset) {
    for (u32 i = 0; i < tab->entryCount; i++) {
        u32 start = tab->rebuiltEntries[i];
        u32 end = tab->rebuiltEntries[i + 1];

        if (offset >= start && offset < end) {
            return i;
        }
    }

    return -1;
}

s32 rebuilt_tab_read(s32 binFileId, void *dst, u32 offset, u32 size) {
    s32 tabFileId = binsToTabs[binFileId];
    if (tabFileId == -1) {
        return FALSE;
    }

    Tab *tab = &tabFiles[tabFileId];
    if (tab->rebuiltEntries == NULL) {
        return FALSE;
    }

    // Find tab index of the file referenced by the offset
    s32 tabIdx = tab_find_idx2(tab, offset);
    if (tabIdx == -1) {
        recomp_eprintf("Couldn't find tab index for offset 0x%x in %s. Bad things to follow!\n", offset, tabConfigs[tabFileId].name);
        // We don't have the correct ROM address to read, just return all zeroes
        bzero(dst, size);
        return TRUE;
    }

    // Find the replacement for the file, if any
    TabEntry *replacement = &tab->replacements[tabIdx];

    if (replacement->data == NULL) {
        // Adjust ROM address back to original
        s32 reloc = tab->relocs[tabIdx];
        _read_file_region_base(binFileId, dst, offset + reloc, size);
    } else {
        // Read from replacement file
        u32 availableSize = tab->stride == 1 ? replacement->size : replacement->availableSize;
        if (size > availableSize) {
            recomp_error_message_box(
                recomp_sprintf_helper("[mod_fs] Replacement not big enough!!! %s[%d]  requested size %x  actual size %x\n", 
                    tabConfigs[tabFileId].name, tabIdx, size, availableSize));
        }

        u32 relativeOffset = offset - tab->rebuiltEntries[tabIdx];
        bcopy((void*)((u32)replacement->data + relativeOffset), dst, size);

        recomp_printf("[mod_fs] Using replacement for %s[%d]\n", tabConfigs[tabFileId].name, tabIdx);
    }

    return TRUE;
}

RECOMP_EXPORT void *get_handle(s32 tabFileId, s32 tabIdx, u32 *outSize) {
    TabEntry *entry = get_original_tab_entry(tabFileId, tabIdx);
    if (outSize != NULL) {
        *outSize = entry->size;
    }

    return entry->data;
}


RECOMP_EXPORT void register_model_replacement(s32 id, void *data, u32 size) {
    // TODO: lookup idx via modelsind
    //tab_add_replacement(&modelsList, id, data, size);
    set_tab_entry_replacement(MODELS_TAB, id, data, size);
}

RECOMP_EXPORT void register_block_replacement(s32 tabIdx, void *data, u32 size) {
    //tab_add_replacement(&blocksList, tabIdx, data, size);
    set_tab_entry_replacement(BLOCKS_TAB, tabIdx, data, size);
}

RECOMP_HOOK_RETURN("init_filesystem") void fs_init_hook() {
    init_tab_bin_pair_maps();
    modfs_on_init();
    for (s32 i = 0; i < NUM_FILES; i++) {
        rebuild_tab(i);
    }
}

RECOMP_PATCH void *read_alloc_file(u32 id, u32 a1)
{
    u32 * fstEntry;
    void * data;
    s32 size;
    u32 offset;

    if (id > gFST->fileCount)
        return NULL;

    if (tabsToBins[id] != -1) {
        Tab *tab = &tabFiles[id];
        if (tab->rebuiltEntries != NULL) {
            return tab->rebuiltEntries;
        }
    }

    ++id;

    fstEntry = id + gFST->offsets - 1;
    offset = fstEntry[0];
    size = fstEntry[1] - offset;

    data = malloc(size, ALLOC_TAG_FS_COL, NULL);
    if (data == NULL)
        return NULL;

    read_from_rom((u32)&__file1Address + offset, data, size);

    // @recomp: Hijack some files
    // switch (id - 1) {
    //     case BLOCKS_TAB:
    //         data = tab_init(&blocksList, data, (size / sizeof(s32)) - 2);
    //         break;
    //     case MODELS_TAB:
    //         data = tab_init(&modelsList, data, (size / sizeof(s32)) - 2);
    //         break;
    // }

    return data;
}

RECOMP_PATCH s32 read_file_region(u32 id, void *dst, u32 offset, s32 size)
{
    s32 fileAddr;
    u32 * tmp;

    if (size == 0 || id > gFST->fileCount)
        return 0;

    if (rebuilt_tab_read(id, dst, offset, size)) {
        return size;
    }

    tmp      = ++id + gFST->offsets - 1;
    fileAddr = *tmp + offset;

    gLastFSTIndex = id;

    read_from_rom(fileAddr + (s32)&__file1Address, dst, size);

    // @recomp: Hijack some files
    // switch (id - 1) {
    //     case BLOCKS_BIN:
    //         tab_read(&blocksList, fileAddr, dst, offset, size);
    //         break;
    //     case MODELS_BIN:
    //         tab_read(&modelsList, fileAddr, dst, offset, size);
    //         break;
    //     default:
    //         read_from_rom(fileAddr + (s32)&__file1Address, dst, size);
    //         break;
    // }

    return size;
}

static s32 _read_file_region_base(u32 id, void *dst, u32 offset, s32 size)
{
    s32 fileAddr;
    u32 * tmp;

    if (size == 0 || id > gFST->fileCount)
        return 0;

    tmp      = ++id + gFST->offsets - 1;
    fileAddr = *tmp + offset;

    gLastFSTIndex = id;

    read_from_rom(fileAddr + (s32)&__file1Address, dst, size);

    return size;
}
