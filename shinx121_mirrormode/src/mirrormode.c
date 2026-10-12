#include "modding.h"
#include "recomputils.h"

#include "PR/R4300.h"
#include "PR/gu.h"
#include "PR/os.h"
#include "dlls/engine/6_amsfx.h"
#include "dlls/objects/210_player.h"
#include "sys/gfx/model.h"
#include "sys/math.h"

extern MtxF gProjectionMtx;
extern u16 gPerspNorm;
extern f32 gFovY;
extern f32 gAspect;
extern f32 gNearPlane;
extern f32 gFarPlane;
extern Mtx gRSPProjectionMtx;
extern f32 gProjectionScaleX;

RECOMP_PATCH void camSetFOV(f32 fovY) {
    if (fovY > 90.0f) {
        fovY = 90.0f;
    } else if (fovY < 40.0f) {
        fovY = 40.0f;
    }

    gFovY = fovY;

    guPerspectiveF(gProjectionMtx.m, &gPerspNorm, gFovY, gAspect, gNearPlane, gFarPlane, 1.0f);

    // @recomp: Flip projection matrix X-axis
    gProjectionMtx.m[0][0] *= -1.0f;

    mathMtxF2L(&gProjectionMtx, &gRSPProjectionMtx);
    gProjectionScaleX = gProjectionMtx.m[0][0];
}

RECOMP_PATCH void camSetAspect(f32 aspect) {
    gAspect = aspect;
    guPerspectiveF(gProjectionMtx.m, &gPerspNorm, gFovY, gAspect, gNearPlane, gFarPlane, 1.0f);

    // @recomp: Flip projection matrix X-axis
    gProjectionMtx.m[0][0] *= -1.0f;

    mathMtxF2L(&gProjectionMtx, &gRSPProjectionMtx);
    gProjectionScaleX = gProjectionMtx.m[0][0];
    gProjectionMtx.m[0][3] *= 0.5f;
}

RECOMP_PATCH void camSetNearPlane(f32 near) {
    gNearPlane = near;
    guPerspectiveF(gProjectionMtx.m, &gPerspNorm, gFovY, gAspect, gNearPlane, gFarPlane, 1.0f);

    // @recomp: Flip projection matrix X-axis
    gProjectionMtx.m[0][0] *= -1.0f;
    
    mathMtxF2L(&gProjectionMtx, &gRSPProjectionMtx);
    gProjectionScaleX = gProjectionMtx.m[0][0];
    gProjectionMtx.m[0][3] *= 0.5f;
}

extern Gfx *gMainGfx[2];
extern Gfx *gCurGfx;
extern u8 gFrameBufIdx;

static void hack_gfx(Gfx* start, Gfx* end, _Bool recurse) {
    // dirty evil diabolical hack...
    // don't try this one at home kids!
    Gfx* gfx = start;
    while (end == NULL || gfx < end) {
        u32 cmd = (gfx->words.w0 & 0xFF000000) >> 24;
        if (cmd == G_ENDDL) {
            break;
        } else if (cmd == G_DL) {
            if (gfx->dma.par == G_DL_NOPUSH || recurse) {
                hack_gfx((Gfx*)PHYS_TO_K0(gfx->words.w1), NULL, recurse);
            }
            if (gfx->dma.par == G_DL_NOPUSH) {
                break;
            }
        } else if (cmd == G_GEOMETRYMODE) {
            gfx->words.w0 |= G_CULL_BACK;
            if (gfx->words.w1 & G_CULL_BACK) {
                gfx->words.w1 &= ~G_CULL_BACK;
                gfx->words.w1 |= G_CULL_FRONT;
            }
        }
        gfx++;
    }
}

RECOMP_HOOK_RETURN("trackDraw") void culling_hack(void) {
    hack_gfx(gMainGfx[gFrameBufIdx], gCurGfx, TRUE);
}

typedef struct _ControllersSnapshot {
    /**
     * A raw controller pad snapshot for each controller.
     */
    OSContPad pads[MAXCONTROLLERS];

    /**
     * For each controller, a button bitfield where set button bits indicate that
     * the button was just pressed down.
     */
    u16 buttonPresses[MAXCONTROLLERS];

    /**
     * For each controller, a button bitfield where set button bits indicate that
     * the button was just released.
     */
    u16 buttonReleases[MAXCONTROLLERS];
} ControllersSnapshot;

extern s8 joyHandleStickDeadzone(s8 stick);

extern OSContPad gContPads[MAXCONTROLLERS];
extern u8 gVirtualContPortMap[MAXCONTROLLERS];
extern u8 gNumBufContSnapshots[2];
extern u8 gPrevContSnapshotsI;
extern ControllersSnapshot *gContSnapshots[2];

RECOMP_PATCH s8 joyGetStickX(int port) {
    if (port > 0) {
        return 0;
    } else {
        // @recomp: Flip stick X-axis to compensate
        return -joyHandleStickDeadzone(gContPads[gVirtualContPortMap[port]].stick_x);
    }
}

RECOMP_PATCH s8 joyGetStickXBuffered(int port, int buffer) {
    OSContPad *pads;
    ControllersSnapshot *snapshot;

    if (port > 0) {
        return 0;
    }

    if (buffer < 0) {
        buffer = 0;
    } else if (buffer >= gNumBufContSnapshots[gPrevContSnapshotsI ^ 1]) {
        buffer = gNumBufContSnapshots[gPrevContSnapshotsI ^ 1] - 1;
    }

    snapshot = gContSnapshots[gPrevContSnapshotsI ^ 1];
    snapshot = &snapshot[buffer];

    pads = snapshot->pads;

    // @recomp: Flip stick X-axis to compensate
    return -joyHandleStickDeadzone(pads[gVirtualContPortMap[port]].stick_x);
}

RECOMP_PATCH s32 __dll210_dll_210_func_1D880(Object* player, s32* arg1, s32* arg2) {
    Player_Data* objdata;

    objdata = player->data;
    if (objdata->flags & 0x400) {
        // @recomp: Flip aim reticle to compensate
        *arg1 = (-(objdata->aimX - (320 / 2))) + (320 / 2);
        *arg2 = objdata->aimY;
        return 1;
    }
    return 0;
}

RECOMP_PATCH void __dll6_amSfx_func_2438(f32 x, f32 z, s32 arg2, s8* outPan, s8* outFx) {
    f32 magnitude;
    s32 theta;
    s32 pad;
    s8 panOffset;
    s8 sp2A;

    magnitude = sqrtf(SQ(x) + SQ(z));
    theta = -mathAtan2f(x, z);
    theta = (arg2 - theta) - M_180_DEGREES;
    CIRCLE_WRAP(theta)
    if (magnitude <= 100.0f) {
        panOffset = -((mathSinInterp(theta) / 1024) * (magnitude * 0.01f));
        sp2A = (s8) -((mathCosInterp(theta) / 1024) * (magnitude * 0.01f));
    } else {
        panOffset = -mathSin(theta) / 1024;
        sp2A = (s8)  (-mathCos(theta) / 1024) ;
    }
    sp2A = -sp2A;
    *outPan = PAN_CENTRE + panOffset;
    *outFx = sp2A;

    // @recomp: Flip sfx pan to compensate
    *outPan = (-(*outPan - PAN_CENTRE)) + PAN_CENTRE;
}
