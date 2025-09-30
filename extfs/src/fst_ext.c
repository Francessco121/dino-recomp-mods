#include "fst_ext.h"

#include "modding.h"
#include "recomputils.h"
#include "extfs_common.h"

#include "PR/ultratypes.h"
#include "sys/fs.h"
#include "sys/memory.h"

typedef struct {
    void *data;
    u32 size;
    _Bool ownedByExtfs;
} FstExtEntry;

static FstExtEntry fstReplacements[NUM_FILES];
static Fs *originalFst;

EXTFS_ON_FST_INIT_CALLBACK void fst_init() {
    // Save vanilla FS.tab
    originalFst = gFST;
}

EXTFS_ON_REBUILD_FST_CALLBACK void fst_rebuild() {
    // Alloc new FS.tab and build it
    s32 size = (s32)&__file1Address - (s32)&__fstAddress;
    gFST = (Fs *)recomp_alloc(size);
    gFST->fileCount = originalFst->fileCount;

    u32 offset = 0;
    u32 i;
    for (i = 0; i < originalFst->fileCount; i++) {
        FstExtEntry *replacement = &fstReplacements[i];
        gFST->offsets[i] = offset;

        //extfs_log("[extfs] %04x -> %s.\n", (s32)&__file1Address + offset, DINO_FS_FILENAMES[i]);

        if (replacement->data != NULL) {
            offset += replacement->size;
        } else {
            offset += originalFst->offsets[i + 1] - originalFst->offsets[i];
        }

        offset = mmAlign2(offset);
    }

    gFST->offsets[i] = offset;

    extfs_log("[extfs] Rebuilt FS.tab.\n");
}

void fst_ext_set_file(s32 fileID, void *data, u32 size, _Bool ownedByExtfs) {
    extfs_assert(fileID >= 0 && fileID < NUM_FILES, "[extfs] File ID out of bounds: %d", fileID);

    FstExtEntry *replacement = &fstReplacements[fileID];
    if (replacement->data != NULL && replacement->ownedByExtfs) {
        if (replacement->data != data) {
            recomp_free(replacement->data);
        }
    }
    
    replacement->data = data;
    replacement->size = size;
    replacement->ownedByExtfs = ownedByExtfs;
}

RECOMP_EXPORT void extfs_set_fst_file_replacement(s32 fileID, const void *data, u32 size) {
    fst_ext_set_file(fileID, (void*)data, size, /*ownedByExtfs=*/FALSE);

    extfs_log("[extfs] Set FST file replacement for %s.\n", DINO_FS_FILENAMES[fileID]);
}

u32 fst_ext_get_file_size(s32 fileID) {
    FstExtEntry *replacement = &fstReplacements[fileID];
    if (replacement->data != NULL) {
        return replacement->size;
    } else {
        return gFST->offsets[fileID + 1] - gFST->offsets[fileID];
    }
}

void fst_ext_read_from_file(s32 fileID, void *dst, u32 offset, u32 size) {
    FstExtEntry *replacement = &fstReplacements[fileID];
    if (replacement->data != NULL) {
        // Read replacement file
        bcopy((u8*)replacement->data + offset, dst, size);
        //extfs_log("[extfs] Reading from FST file replacement for %d.\n", fileID);
    } else {
        // Read original ROM
        u32 fileOffset = originalFst->offsets[fileID];

        read_from_rom((u32)&__file1Address + fileOffset + offset, dst, size);
    }
}

s32 fst_ext_audio_dma(void *dst, u32 romAddr, u32 size) {
    romAddr -= (u32)&__file1Address;
    
    s32 fileID = -1;
    u32 offset = 0;

    for (u32 i = 0; i < gFST->fileCount; i++) {
        u32 entryOffset = gFST->offsets[i];
        u32 entryEndOffset = gFST->offsets[i + 1];

        if (romAddr >= entryOffset && romAddr < entryEndOffset) {
            fileID = i;
            offset = romAddr - entryOffset;
            break;
        }
    }

    if (fileID == -1) {
        return FALSE;
    }

    FstExtEntry *replacement = &fstReplacements[fileID];
    if (replacement->data == NULL) {
        return FALSE;
    }

    //extfs_log("[extfs] [AUDIO DMA] Reading from FST file replacement for %d. %x   %x\n", fileID, offset, size);

    bcopy((u8*)replacement->data + offset, dst, size);
    return TRUE;
}
