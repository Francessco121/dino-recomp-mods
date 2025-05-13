#include "modding.h"

#include "sys/fs.h"
#include "sys/memory.h"

#include "extfs_common.h"
#include "files/blocks_ext.h"
#include "files/maps_ext.h"
#include "files/models_ext.h"
#include "files/objects_ext.h"
#include "files/screens_ext.h"

RECOMP_DECLARE_EVENT(extfs_on_load_replacements());
RECOMP_DECLARE_EVENT(extfs_on_load_modifications());

RECOMP_DECLARE_EVENT(_extfs_on_init());
RECOMP_DECLARE_EVENT(_extfs_on_commit());

RECOMP_HOOK_RETURN("init_filesystem") void fs_init_hook() {
    _extfs_on_init();

    extfsLoadStage = EXTFS_STAGE_REPLACEMENTS;
    extfs_on_load_replacements();

    extfsLoadStage = EXTFS_STAGE_MODIFICATIONS;
    extfs_on_load_modifications();

    extfsLoadStage = EXTFS_STAGE_COMMITTED;
    _extfs_on_commit();
}

RECOMP_PATCH void *read_alloc_file(u32 id, u32 a1)
{
    u32 * fstEntry;
    void * data;
    s32 size;
    u32 offset;

    if (id > (u32)gFST->fileCount)
        return NULL;

    // @recomp: Extended filesystem patches
    switch (id) {
        case BLOCKS_TAB:
            if (blocks_ext_try_read_tab(&data)) return data;
            break;
        case MAPS_TAB:
            if (maps_ext_try_read_tab(&data)) return data;
            break;
        case MODELS_TAB:
            if (models_ext_try_read_tab(&data)) return data;
            break;
        case OBJECTS_TAB:
            if (objects_ext_try_read_tab(&data)) return data;
            break;
        case SCREENS_TAB:
            if (screens_ext_try_read_tab(&data)) return data;
            break;
    }

    ++id;

    fstEntry = id + gFST->offsets - 1;
    offset = fstEntry[0];
    size = fstEntry[1] - offset;

    data = malloc(size, ALLOC_TAG_FS_COL, NULL);
    if (data == NULL)
        return NULL;

    read_from_rom((u32)&__file1Address + offset, data, size);

    return data;
}

RECOMP_PATCH s32 read_file_region(u32 id, void *dst, u32 offset, s32 size)
{
    s32 fileAddr;
    u32 * tmp;

    if (size == 0 || id > (u32)gFST->fileCount)
        return 0;

    // @recomp: Extended filesystem patches
    switch (id) {
        case BLOCKS_BIN:
            if (blocks_ext_try_read_bin(dst, offset, size)) return size;
            break;
        case MAPS_BIN:
            if (maps_ext_try_read_bin(dst, offset, size)) return size;
            break;
        case MODELS_BIN:
            if (models_ext_try_read_bin(dst, offset, size)) return size;
            break;
        case OBJECTS_BIN:
            if (objects_ext_try_read_bin(dst, offset, size)) return size;
            break;
        case SCREENS_BIN:
            if (screens_ext_try_read_bin(dst, offset, size)) return size;
            break;
    }

    tmp      = ++id + gFST->offsets - 1;
    fileAddr = *tmp + offset;

    gLastFSTIndex = id;

    read_from_rom(fileAddr + (s32)&__file1Address, dst, size);

    return size;
}
