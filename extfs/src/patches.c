#include "modding.h"

#include "sys/fs.h"
#include "sys/memory.h"

#include "extfs_common.h"
#include "tab_ext.h"

RECOMP_DECLARE_EVENT(extfs_on_load_replacements());
RECOMP_DECLARE_EVENT(extfs_on_load_modifications());

RECOMP_HOOK_RETURN("init_filesystem") void fs_init_hook() {
    tab_ext_init();
    extfsLoadStage = EXTFS_STAGE_REPLACEMENTS;
    extfs_on_load_replacements();
    extfsLoadStage = EXTFS_STAGE_MODIFICATIONS;
    extfs_on_load_modifications();
    extfsLoadStage = EXTFS_STAGE_COMMITTED;
    tab_ext_rebuild_tabs();
}

RECOMP_PATCH void *read_alloc_file(u32 id, u32 a1)
{
    u32 * fstEntry;
    void * data;
    s32 size;
    u32 offset;

    if (id > (u32)gFST->fileCount)
        return NULL;

    // @recomp: Use rebuilt tabs if they exist
    if (tab_ext_get_rebuilt_entries(id, &data)) {
        return data;
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

    // @recomp: Use replacement tab entries if they exist
    if (tab_ext_try_read_bin(id, dst, offset, size)) {
        return size;
    }

    tmp      = ++id + gFST->offsets - 1;
    fileAddr = *tmp + offset;

    gLastFSTIndex = id;

    read_from_rom(fileAddr + (s32)&__file1Address, dst, size);

    return size;
}
