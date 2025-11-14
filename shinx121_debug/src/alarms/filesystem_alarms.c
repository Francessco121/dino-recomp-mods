#include "modding.h"
#include "recomputils.h"

#include "PR/ultratypes.h"

RECOMP_HOOK("queue_load_file_region_to_ptr") void queue_load_file_region_to_ptr_hook(void **dest, s32 fileId, s32 offset, s32 length) {
    if (offset < 0) {
        recomp_exit_with_error(recomp_sprintf_helper(
            "queue_load_file_region_to_ptr(%p, 0x%X, %d, %d) negative offset!", dest, fileId, offset, length));
    }
}
