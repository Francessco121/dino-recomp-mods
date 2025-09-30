#include "modding.h"

#include "sys/fs.h"
#include "sys/memory.h"

#include "extfs_common.h"
#include "fst_ext.h"

RECOMP_DECLARE_EVENT(extfs_on_load_fst_replacements());
RECOMP_DECLARE_EVENT(extfs_on_load_replacements());
RECOMP_DECLARE_EVENT(extfs_on_load_modifications());

// TODO: review event names
RECOMP_DECLARE_EVENT(_extfs_on_fst_init());
RECOMP_DECLARE_EVENT(_extfs_on_init());
RECOMP_DECLARE_EVENT(_extfs_on_commit());
RECOMP_DECLARE_EVENT(_extfs_on_rebuild_fst());

RECOMP_HOOK_RETURN("init_filesystem") void fs_init_hook() {
    _extfs_on_fst_init();

    extfsLoadStage = EXTFS_STAGE_FST_REPLACEMENTS;
    extfs_on_load_fst_replacements();

    _extfs_on_init();

    extfsLoadStage = EXTFS_STAGE_SUBFILE_REPLACEMENTS;
    extfs_on_load_replacements();

    extfsLoadStage = EXTFS_STAGE_MODIFICATIONS;
    extfs_on_load_modifications();

    extfsLoadStage = EXTFS_STAGE_COMMITTED;
    _extfs_on_commit();
    
    _extfs_on_rebuild_fst();
}

RECOMP_PATCH void *read_alloc_file(u32 id, u32 a1)
{
    void *data;

    if (id > (u32)gFST->fileCount)
        return NULL;

    // @recomp: Rewrite to use fst_ext
    u32 size = fst_ext_get_file_size(id);

    data = mmAlloc(size, COLOUR_TAG_GREY, NULL);
    if (data == NULL)
        return NULL;

    fst_ext_read_from_file(id, data, 0, size);

    return data;
}

RECOMP_PATCH s32 read_file(u32 id, void *dest)
{
    if (id > (u32)gFST->fileCount)
        return NULL;

    // @recomp: Rewrite to use fst_ext
    u32 size = fst_ext_get_file_size(id);
    fst_ext_read_from_file(id, dest, 0, size);

    return size;
}

RECOMP_PATCH s32 read_file_region(u32 id, void *dst, u32 offset, s32 size)
{
    if (size == 0 || id > (u32)gFST->fileCount)
        return 0;

    // @recomp: Rewrite to use fst_ext
    gLastFSTIndex = id + 1;

    fst_ext_read_from_file(id, dst, offset, size);

    return size;
}

#include "PR/os.h"
#include "libnaudio/n_libaudio.h"
#include "sys/audio/speaker.h"

typedef struct {
    ALLink node;
    u32 startAddr;
    u32 lastFrame;
    char *ptr;
} AMDMABuffer;

typedef struct {
    u8 initialized;
    AMDMABuffer *firstUsed;
    AMDMABuffer *firstFree;
} AMDMAState;

#define DMA_BUFFER_LENGTH       0x800
#define NUM_DMA_MESSAGES        50 

extern s32 D_8008C8CC; // audFrameCt
extern s32 D_8008C8D0; // nextDMA
extern AMDMAState D_800AAA28; // dmaState
extern OSIoMesg D_800AB088[NUM_DMA_MESSAGES]; // audDMAIOMesgBuf
extern OSMesgQueue D_800AB808; // audDMAMessageQ

RECOMP_PATCH s32 al_dame_sub(s32 addr, s32 len, void *state) {
    AMDMABuffer *dmaPtr, *lastDmaPtr;
    void *foundBuffer;
    s32 delta, addrEnd, buffEnd;
    s32 pad;

   // extfs_log("al_dame_sub(%x, %x, %x)\n", addr, len, state);

    lastDmaPtr = NULL;
    delta = addr & 1;
    dmaPtr = D_800AAA28.firstUsed;
    addrEnd = addr + len;

    /* first check to see if a currently existing buffer contains the
       sample that you need.  */

    while (dmaPtr) {
        buffEnd = dmaPtr->startAddr + DMA_BUFFER_LENGTH;
        if (dmaPtr->startAddr > (u32) addr) { /* since buffers are ordered */
            break;                            /* abort if past possible */
        } else if (addrEnd <= buffEnd) {      /* yes, found a buffer with samples */
            dmaPtr->lastFrame = D_8008C8CC;   /* mark it used */
            foundBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return (int) osVirtualToPhysical(foundBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (AMDMABuffer *) dmaPtr->node.next;
    }

    /* get here, and you didn't find a buffer, so dma a new one */

    /* get a buffer from the free list */
    dmaPtr = D_800AAA28.firstFree;

    if (!dmaPtr && !lastDmaPtr) {
        lastDmaPtr = D_800AAA28.firstUsed;
    }

    /*
     * if you get here and dmaPtr is null, send back a bogus
     * pointer, it's better than nothing
     */
    if (!dmaPtr) {
        //stubbed_printf("OH DEAR - No audio DMA buffers left\n");
        return (int) osVirtualToPhysical(lastDmaPtr->ptr) + delta;
    }

    D_800AAA28.firstFree = (AMDMABuffer *) dmaPtr->node.next;
    alUnlink((ALLink *) dmaPtr);

    /* add it to the used list */
    if (lastDmaPtr) { /* if you have other dmabuffers used, add this one */
                      /* to the list, after the last one checked above */
        alLink((ALLink *) dmaPtr, (ALLink *) lastDmaPtr);
    } else if (D_800AAA28.firstUsed) { /* if this buffer is before any others */
                                     /* jam at begining of list */
        lastDmaPtr = D_800AAA28.firstUsed;
        D_800AAA28.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink *) lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink *) dmaPtr;
    } else { /* no buffers in list, this is the first one */
        D_800AAA28.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }

    foundBuffer = dmaPtr->ptr;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = D_8008C8CC; /* mark it */

    if (fst_ext_audio_dma(foundBuffer, addr, DMA_BUFFER_LENGTH)) {
        D_8008C8D0++;
    } else {
        osPiStartDma(&D_800AB088[D_8008C8D0++], OS_MESG_PRI_HIGH, OS_READ, addr, foundBuffer, DMA_BUFFER_LENGTH,
                    &D_800AB808);
    }

    return (int) osVirtualToPhysical(foundBuffer) + delta;
}
