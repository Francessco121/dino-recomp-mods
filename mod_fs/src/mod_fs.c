#define _MOD_FS_SRC
#include "mod_fs.h"

#include "imports.h"
#include "modding.h"

#include "tab.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"
#include "sys/memory.h"

RECOMP_DECLARE_EVENT(modfs_on_init());

extern Fs * gFST;
extern u32 gLastFSTIndex;
extern s32 __fstAddress;
extern s32 __file1Address;

extern void read_from_rom(u32 romAddr, u8* dst, s32 size);

static TabModList modelsList = { .name = "MODELS.TAB" };
static TabModList blocksList = { .name = "BLOCKS.TAB" };

RECOMP_EXPORT void register_model_replacement(s32 id, void *data, u32 size) {
    // TODO: lookup idx via modelsind
    tab_add_replacement(&modelsList, id, data, size);
}

RECOMP_EXPORT void register_block_replacement(s32 tabIdx, void *data, u32 size) {
    tab_add_replacement(&blocksList, tabIdx, data, size);
}

RECOMP_HOOK_RETURN("init_filesystem") void fs_init_hook() {
    modfs_on_init();
}

RECOMP_PATCH void *read_alloc_file(u32 id, u32 a1)
{
    u32 * fstEntry;
    void * data;
    s32 size;
    u32 offset;

    if (id > gFST->fileCount)
        return NULL;

    ++id;

    fstEntry = id + gFST->offsets - 1;
    offset = fstEntry[0];
    size = fstEntry[1] - offset;

    data = malloc(size, ALLOC_TAG_FS_COL, NULL);
    if (data == NULL)
        return NULL;

    read_from_rom((u32)&__file1Address + offset, data, size);

    // @recomp: Hijack some files
    switch (id - 1) {
        case BLOCKS_TAB:
            data = tab_init(&blocksList, data, (size / sizeof(s32)) - 2);
            break;
        case MODELS_TAB:
            data = tab_init(&modelsList, data, (size / sizeof(s32)) - 2);
            break;
    }

    return data;
}

RECOMP_PATCH s32 read_file_region(u32 id, void *dst, u32 offset, s32 size)
{
    s32 fileAddr;
    u32 * tmp;

    if (size == 0 || id > gFST->fileCount)
        return 0;

    tmp      = ++id + gFST->offsets - 1;
    fileAddr = *tmp + offset;

    gLastFSTIndex = id;

    // @recomp: Hijack some files
    switch (id - 1) {
        case BLOCKS_BIN:
            tab_read(&blocksList, fileAddr, dst, offset, size);
            break;
        case MODELS_BIN:
            tab_read(&modelsList, fileAddr, dst, offset, size);
            break;
        default:
            read_from_rom(fileAddr + (s32)&__file1Address, dst, size);
            break;
    }

    return size;
}
