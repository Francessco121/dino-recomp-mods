#include "modding.h"

#include "PR/os.h"
#include "sys/camera.h"
#include "sys/joypad.h"
#include "sys/main.h"
#include "sys/map.h"
#include "sys/math.h"
#include "sys/menu.h"
#include "sys/segment_13D0.h"
#include "dll.h"

extern s8 gPauseState; // so we can pause the game ourselves
// so we can manually run trackDraw
extern Gfx *gCurGfx;
extern Mtx *gCurMtx;
extern Vertex *gCurVtx;
extern Triangle *gCurPol;
// so we can detect map changes
extern s8 gMainDoMapChange;

typedef struct {
    SRT transform;
    f32 fov;
} FreeCamera;

typedef struct {
    u16 buttons;
    u16 pressed;
    u16 released;
    s8 stickX;
    s8 stickY;
} FreeCamJoypad;

static _Bool freeCamActive = FALSE;
static FreeCamera freeCam;
static _Bool stealInputs = FALSE;
static FreeCamJoypad joypad;
static s32 prevMenu = 0;
static _Bool customPause = FALSE;

static void freecamInit(void) {
    camSetCameraSelector(0);
    Camera* camera = camGetMain();

    freeCam.transform.transl = camera->srt.transl;
    freeCam.transform.scale = 1.0f;
    freeCam.transform.yaw = camera->srt.yaw;
    freeCam.transform.pitch = camera->srt.pitch;
    freeCam.transform.roll = camera->srt.roll;

    freeCam.fov = camGetFOV();
}

static void freecamUpdateCamera(void) {
    camSetCameraSelector(0);
    Camera* camera = camGetMain();
    camera->srt.yaw = freeCam.transform.yaw;
    camera->srt.pitch = freeCam.transform.pitch;
    camera->srt.roll = freeCam.transform.roll;
    camera->srt.transl.x = freeCam.transform.transl.x;
    camera->srt.transl.y = freeCam.transform.transl.y;
    camera->srt.transl.z = freeCam.transform.transl.z;

    if (camGetFOV() != freeCam.fov) {
        camSetFOV(freeCam.fov);
    }
    
    camUpdateCameraForObject(camera);
    //map_func_80046B58(camera->tx, camera->ty, camera->tz);

    camSetLetterbox(0);
}

static void freecamControl(void) {
    stealInputs = TRUE;

    const f32 MOVE_SPEED = 5.0f;
    const f32 ROTATE_SPEED = 200.0f;

    Vec3f moveDir = {0.0f, 0.0f, 0.0f};
    f32 movePower = 0.0f;
    f32 yawPower = 0.0f;
    f32 pitchPower = 0.0f;

    if (joypad.buttons & Z_TRIG) {
        yawPower = joypad.stickX / 127.0f;
        pitchPower = -(joypad.stickY / 127.0f);

        if (joypad.buttons & L_CBUTTONS) {
            moveDir.x -= 1.0f;
        }
        if (joypad.buttons & R_CBUTTONS) {
            moveDir.x += 1.0f;
        }
        if (joypad.buttons & U_CBUTTONS) {
            moveDir.z -= 1.0f;
        }
        if (joypad.buttons & D_CBUTTONS) {
            moveDir.z += 1.0f;
        }

        if ((joypad.buttons & R_TRIG) && !(joypad.buttons & U_JPAD)) { // ignore if dpad-up so toggling freecam doesn't also move
            moveDir.y = -1.0f;
        } else if (joypad.buttons & L_TRIG) {
            moveDir.y = 1.0f;
        }

        movePower = MIN(1.0f, VECTOR_MAGNITUDE(moveDir));
    } else {
        moveDir.x = joypad.stickX / 127.0f;
        moveDir.z = -(joypad.stickY / 127.0f);
        moveDir.y = 0.0f;

        if ((joypad.buttons & R_TRIG) && !(joypad.buttons & U_JPAD)) { // ignore if dpad-up so toggling freecam doesn't also move
            moveDir.y = -1.0f;
        } else if (joypad.buttons & L_TRIG) {
            moveDir.y = 1.0f;
        }

        movePower = MIN(1.0f, VECTOR_MAGNITUDE(moveDir) * (127.0f / 65.0f));

        if (joypad.buttons & L_CBUTTONS) {
            yawPower -= 1.0f;
        }
        if (joypad.buttons & R_CBUTTONS) {
            yawPower += 1.0f;
        }
        if (joypad.buttons & U_CBUTTONS) {
            pitchPower -= 1.0f;
        }
        if (joypad.buttons & D_CBUTTONS) {
            pitchPower += 1.0f;
        }
    }

    if (vec3Normalize(&moveDir) != 0.0f) {
        moveDir.x *= MOVE_SPEED * movePower * gUpdateRateF;
        moveDir.y *= MOVE_SPEED * movePower * gUpdateRateF;
        moveDir.z *= MOVE_SPEED * movePower * gUpdateRateF;

        SRT camRotSRT = {
            .transl.x = 0,
            .transl.y = 0,
            .transl.z = 0,
            .scale = 1.0f,
            .yaw = -freeCam.transform.yaw,
            .pitch = freeCam.transform.pitch,
            .roll = 0
        };
        MtxF camMtx;
        mathYprXyzMtx(&camMtx, &camRotSRT);

        Vec3f moveDirTransformed;
        mathMtxFastXFMF(&camMtx, &moveDir, &moveDirTransformed);

        freeCam.transform.transl.x -= moveDirTransformed.x;
        freeCam.transform.transl.y -= moveDirTransformed.y;
        freeCam.transform.transl.z -= moveDirTransformed.z;
    }

    if (yawPower != 0.0f) {
        s32 yaw = freeCam.transform.yaw + (s32)(yawPower * ROTATE_SPEED * gUpdateRateF);
        CIRCLE_WRAP(yaw);
        freeCam.transform.yaw = yaw;
    }

    if (pitchPower != 0.0f) {
        s32 pitch = freeCam.transform.pitch + (s32)(pitchPower * ROTATE_SPEED * gUpdateRateF);
        CIRCLE_WRAP(pitch);
        freeCam.transform.pitch = pitch;
    }
}

static void freecamPause(void) {
    gPauseState = 1;
    prevMenu = menuGetCurrent();
    customPause = TRUE;
}

static void freecamUnpause(void) {
    mainUnpause();
    menuSet(prevMenu);
    customPause = FALSE;
}

// Note: this hook does not run while paused
RECOMP_HOOK_RETURN("__dll2_CamControl_tick") void freecamTick(void) {
    if (customPause) {
        return; // just in case
    }

    if ((joypad.buttons & R_TRIG) && (joypad.pressed & U_JPAD)) {
        if (!freeCamActive) {
            freecamInit();
        } else {
            if (customPause) {
                freecamUnpause();
            }
        }
        freeCamActive = !freeCamActive;
    }

    if (!freeCamActive) {
        return;
    }

    if ((joypad.pressed & START_BUTTON) && mainGetPauseState() == 0) {
        joypad.pressed &= ~START_BUTTON;
        freecamPause();
    }

    freecamControl();
    freecamUpdateCamera();
}

RECOMP_HOOK_RETURN("main_func_80013D80") void freecamTickLate(void) {
    if (!freeCamActive) {
        return;
    }

    if (customPause) {
        if (menuGetCurrent() != MENU_NONE) {
            menuSet(MENU_NONE);
        }

        if (joypad.pressed & START_BUTTON) {
            freecamUnpause();
        }

        if ((joypad.buttons & R_TRIG) && (joypad.pressed & U_JPAD)) {
            freecamUnpause();
            freeCamActive = !freeCamActive;
        }

        if (customPause) {
            freecamControl();
            freecamUpdateCamera();
        }
    }
}

RECOMP_HOOK("__dll1_cmdmenu_update1") void freecamCmdmenuHook(void) {
    // Hide cmdmenu while freecam is active
    if (freeCamActive && stealInputs) {
        gDLL_1_cmdmenu->vtbl->disable_buttons(0xFFFF);
    }
}

RECOMP_HOOK("mainTick") void freecamInputSteal(void) {
    joypad.buttons = joyGetButtons(0);
    joypad.pressed = joyGetPressed(0);
    joypad.released = joyGetReleased(0);
    joypad.stickX = joyGetStickX(0);
    joypad.stickY = joyGetStickY(0);
    
    if (freeCamActive && stealInputs) {
        joyDisableButtons(0, 0xFFFF);
        joyDisableStick(0);
    }
}

// Note: this hook runs right before trackDraw normally would (but doesn't because of the pause state)
RECOMP_HOOK_RETURN("__dll4_dll_4_func_FDC") void freecamCustomPauseDraw(void) {
    // monkaW
    if (customPause && gPauseState == 2) {
        gDLL_24_Waterfx->vtbl->tick(/*gUpdateRate*/0);
        gDLL_15_Projgfx->vtbl->func2(/*gUpdateRate*/0, 0);
        gDLL_14_Modgfx->vtbl->func2(0, 0, 0); // TODO: is this even ok?
        gDLL_13_Expgfx->vtbl->func2(0, /*gUpdateRate*/0, 0, 0);

        trackDraw(&gCurGfx, &gCurMtx, &gCurVtx, &gCurPol, &gCurVtx, &gCurPol);
    }
}

RECOMP_HOOK("mainHandleMapChange") void freecamMapChangeHook(void) {
    // Disable freecam on map change
    if (gMainDoMapChange) {
        if (customPause) {
            freecamUnpause();
        }
        freeCamActive = FALSE;
    }
}
