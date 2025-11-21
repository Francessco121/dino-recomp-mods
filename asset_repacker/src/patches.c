#include "modding.h"

#include "PR/os.h"
#include "libnaudio/n_libaudio.h"
#include "sys/audio/speaker.h"
#include "sys/audio.h"
#include "sys/fs.h"
#include "sys/memory.h"

#include "repacker_common.h"
#include "fst_ext.h"

RECOMP_DECLARE_EVENT(repacker_on_load_fst_replacements());
RECOMP_DECLARE_EVENT(repacker_on_load_replacements());
RECOMP_DECLARE_EVENT(repacker_on_load_modifications());

// TODO: review event names
RECOMP_DECLARE_EVENT(_repacker_on_fst_init());
RECOMP_DECLARE_EVENT(_repacker_on_init());
RECOMP_DECLARE_EVENT(_repacker_on_commit());
RECOMP_DECLARE_EVENT(_repacker_on_rebuild_fst());

RECOMP_HOOK_RETURN("init_filesystem") void fs_init_hook() {
    _repacker_on_fst_init();

    repackerStage = REPACKER_STAGE_FST_REPLACEMENTS;
    repacker_on_load_fst_replacements();

    _repacker_on_init();

    repackerStage = REPACKER_STAGE_SUBFILE_REPLACEMENTS;
    repacker_on_load_replacements();

    repackerStage = REPACKER_STAGE_MODIFICATIONS;
    repacker_on_load_modifications();

    repackerStage = REPACKER_STAGE_COMMITTED;
    _repacker_on_commit();
    
    _repacker_on_rebuild_fst();
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

extern u32 audFrameCt;
extern u32 nextDMA;
extern AMDMAState dmaState;
extern OSIoMesg audDMAIOMesgBuf[NUM_DMA_MESSAGES];
extern OSMesgQueue audDMAMessageQ;

RECOMP_PATCH s32 __amDMA(s32 addr, s32 len, void *state) {
    AMDMABuffer *dmaPtr, *lastDmaPtr;
    void *foundBuffer;
    s32 delta, addrEnd, buffEnd;
    s32 pad;

    lastDmaPtr = NULL;
    delta = addr & 1;
    dmaPtr = dmaState.firstUsed;
    addrEnd = addr + len;

    /* first check to see if a currently existing buffer contains the
       sample that you need.  */

    while (dmaPtr) {
        buffEnd = dmaPtr->startAddr + DMA_BUFFER_LENGTH;
        if (dmaPtr->startAddr > (u32) addr) { /* since buffers are ordered */
            break;                            /* abort if past possible */
        } else if (addrEnd <= buffEnd) {      /* yes, found a buffer with samples */
            dmaPtr->lastFrame = audFrameCt;   /* mark it used */
            foundBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return (int) osVirtualToPhysical(foundBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (AMDMABuffer *) dmaPtr->node.next;
    }

    /* get here, and you didn't find a buffer, so dma a new one */

    /* get a buffer from the free list */
    dmaPtr = dmaState.firstFree;

    if (!dmaPtr && !lastDmaPtr) {
        lastDmaPtr = dmaState.firstUsed;
    }

    /*
     * if you get here and dmaPtr is null, send back a bogus
     * pointer, it's better than nothing
     */
    if (!dmaPtr) {
        //STUBBED_PRINTF("OH DEAR - No audio DMA buffers left\n");
        return (int) osVirtualToPhysical(lastDmaPtr->ptr) + delta;
    }

    dmaState.firstFree = (AMDMABuffer *) dmaPtr->node.next;
    alUnlink((ALLink *) dmaPtr);

    /* add it to the used list */
    if (lastDmaPtr) { /* if you have other dmabuffers used, add this one */
                      /* to the list, after the last one checked above */
        alLink((ALLink *) dmaPtr, (ALLink *) lastDmaPtr);
    } else if (dmaState.firstUsed) { /* if this buffer is before any others */
                                     /* jam at begining of list */
        lastDmaPtr = dmaState.firstUsed;
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink *) lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink *) dmaPtr;
    } else { /* no buffers in list, this is the first one */
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }

    foundBuffer = dmaPtr->ptr;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = audFrameCt; /* mark it */

    // @recomp: Read from replaced files, if any
    if (fst_ext_audio_dma(foundBuffer, addr, DMA_BUFFER_LENGTH)) {
        nextDMA++;
    } else {
        osPiStartDma(&audDMAIOMesgBuf[nextDMA++], OS_MESG_PRI_HIGH, OS_READ, addr, foundBuffer, DMA_BUFFER_LENGTH,
                    &audDMAMessageQ);
    }

    return (int) osVirtualToPhysical(foundBuffer) + delta;
}
