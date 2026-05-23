#pragma once

#include "PR/ultratypes.h"

typedef struct {
    s32 seqIdx;
    u16 seqActorBits;
    s32 seqPlayLastRet;
    s32 modAnimIdx;
    u8 modAnimBank;
    u8 modAnimAutoplay;
    u8 modAnimOverride;
    u8 modAnimOverrideDisabledControl;
    f32 modAnimDelta;
} ObjEditorData;
