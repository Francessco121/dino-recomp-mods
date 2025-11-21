#pragma once

#include "PR/ultratypes.h"
#include "sys/fs.h"

extern Fs *gFST;
extern u32 gLastFSTIndex;
extern s32 __fstAddress;
extern s32 __file1Address;

extern void read_from_rom(u32 romAddr, u8* dst, s32 size);

extern void bcopy_recomp(const void *src, void *dst, int length);
#define bcopy bcopy_recomp

extern void bzero_recomp(void *, int);
#define bzero bzero_recomp

#define REPACKER_ON_FST_INIT_CALLBACK RECOMP_CALLBACK(".", _repacker_on_fst_init)
#define REPACKER_ON_INIT_CALLBACK RECOMP_CALLBACK(".", _repacker_on_init)
#define REPACKER_ON_COMMIT_CALLBACK RECOMP_CALLBACK(".", _repacker_on_commit)
#define REPACKER_ON_REBUILD_FST_CALLBACK RECOMP_CALLBACK(".", _repacker_on_rebuild_fst)

typedef enum {
    REPACKER_STAGE_UNINITIALIZED,
    REPACKER_STAGE_FST_REPLACEMENTS,
    REPACKER_STAGE_SUBFILE_REPLACEMENTS,
    REPACKER_STAGE_MODIFICATIONS,
    REPACKER_STAGE_COMMITTED
} RepackerStage;

extern RepackerStage repackerStage;

extern const char *DINO_FS_FILENAMES[NUM_FILES];

void repacker_assert(_Bool condition, const char *fmt, ...);
void repacker_assert_no_exit(_Bool condition, const char *fmt, ...);
void repacker_log(const char *fmt, ...);
void repacker_log_error(const char *fmt, ...);
void repacker_error_box(const char *fmt, ...);
