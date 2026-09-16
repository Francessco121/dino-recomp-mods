#include "modding.h"
#include "recompevents.h"
#include "recompconfig.h"

#include "PR/os.h"
#include "sys/camera.h"
#include "sys/joypad.h"
#include "sys/main.h"
#include "sys/map.h"
#include "sys/math.h"
#include "sys/menu.h"
#include "sys/fonts.h"
#include "sys/objects.h"
#include "dll.h"

extern s8 gPauseState; // so we can pause the game ourselves
// so we can manually run trackDraw
extern Gfx *gCurGfx;
extern Mtx *gCurMtx;
extern Vertex *gCurVtx;
extern Triangle *gCurPol;
// so we can detect map changes
extern s8 gMainDoMapChange;
// so we can disable menu stick inputs (no core func exists for this)
extern s8 gMenuJoyXSign[MAXCONTROLLERS];
extern s8 gMenuJoyYSign[MAXCONTROLLERS];

typedef void (*MinimapPrintFunc)(Gfx** gdl, Mtx** arg1);

typedef struct {
    SRT transform;
    f32 fov;
    f32 initialFOV;
} FreeCamera;

typedef struct {
    u16 buttons;
    u16 pressed;
    u16 released;
    s8 stickX;
    s8 stickY;
    s8 xSign;
    s8 ySign;
    _Bool ignoreRTrig;
} FreeCamJoypad;

typedef enum {
    FREECAM_CTRLSTICK_TRANSL_XZ,
    FREECAM_CTRLSTICK_TRANSL_XY,
    FREECAM_CTRLSTICK_YAW_PITCH,
    FREECAM_CTRLSTICK_ROLL,
    FREECAM_CTRLSTICK_FOV
} FreeCamControlStickMode;

#define FREECAM_FONT FONT_DINO_SUBTITLE_FONT_1
#define FREECAM_FONT_WINDOW 7

static _Bool freeCamActive = FALSE;
static FreeCamera freeCam;
static FreeCamJoypad joypad;
static s32 prevMenu = -1;
static _Bool customPause = FALSE;
static s32 controlStickMode = FREECAM_CTRLSTICK_TRANSL_XZ;
static _Bool staticCamActive = FALSE;
static MinimapPrintFunc origMinimapPrintFunc;
static f32 quickSwapPopupTimer = 0.0f;
static _Bool qucikSwapPoupupVisible = FALSE;

#define NUM_MENU_ITEMS 6
static s32 selectedMenuItem = 0;
static _Bool menuOpen = FALSE;

static void freecamMinimapPrintStub(Gfx** gdl, Mtx** mtxs) { }

static void freecamPause(void) {
    gPauseState = 1;
    customPause = TRUE;
}

static void freecamUnpause(void) {
    mainUnpause();
    customPause = FALSE;
}

static void freecamEnter(void) {
    if (freeCamActive) {
        return;
    }

    if (!staticCamActive) {
        camSetCameraSelector(0);
        Camera* camera = camGetMain();

        freeCam.transform.transl = camera->srt.transl;
        freeCam.transform.scale = 1.0f;
        freeCam.transform.yaw = camera->srt.yaw;
        freeCam.transform.pitch = camera->srt.pitch;
        freeCam.transform.roll = camera->srt.roll;

        freeCam.fov = camGetFOV();
    }

    freeCam.initialFOV = freeCam.fov;

    staticCamActive = FALSE;
    menuOpen = FALSE;
    selectedMenuItem = 0;
    quickSwapPopupTimer = 0.0f;
    qucikSwapPoupupVisible = FALSE;

    fontLoad(FREECAM_FONT);

    freeCamActive = TRUE;

    prevMenu = menuGetCurrent();

    // hack to hide minimap during freecam
    if (gDLL_59_Minimap->vtbl->func1 != freecamMinimapPrintStub) {
        origMinimapPrintFunc = gDLL_59_Minimap->vtbl->func1;
        gDLL_59_Minimap->vtbl->func1 = freecamMinimapPrintStub;
    }

    if (joypad.pressed & START_BUTTON) {
        // start paused
        freecamPause();
    }

    joypad.ignoreRTrig = TRUE; // ignore activation button until released
    joypad.buttons &= ~(R_TRIG | U_JPAD | START_BUTTON);
    joypad.pressed &= ~(R_TRIG | U_JPAD | START_BUTTON);
}

static void freecamExit(void) {
    if (!freeCamActive) {
        return;
    }

    if (prevMenu != -1) {
        menuSet(prevMenu);
    }

    if (origMinimapPrintFunc != NULL) {
        gDLL_59_Minimap->vtbl->func1 = origMinimapPrintFunc;
        origMinimapPrintFunc = NULL;
    }

    if (customPause) {
        freecamUnpause();
    }

    fontUnload(FREECAM_FONT);

    freeCamActive = FALSE;
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

static void freecamCamControl(void) {
    const f32 QUICKSWAP_POPUP_DURATION = (1.0f * 60.0f);

    if (joypad.pressed & U_JPAD) {
        controlStickMode = FREECAM_CTRLSTICK_TRANSL_XZ;
        quickSwapPopupTimer = QUICKSWAP_POPUP_DURATION;
    }
    if (joypad.pressed & R_JPAD) {
        controlStickMode = FREECAM_CTRLSTICK_TRANSL_XY;
        quickSwapPopupTimer = QUICKSWAP_POPUP_DURATION;
    }
    if (joypad.pressed & D_JPAD) {
        controlStickMode = FREECAM_CTRLSTICK_YAW_PITCH;
        quickSwapPopupTimer = QUICKSWAP_POPUP_DURATION;
    }
    if (joypad.pressed & L_JPAD) {
        controlStickMode = FREECAM_CTRLSTICK_FOV;
        quickSwapPopupTimer = QUICKSWAP_POPUP_DURATION;
    }

    f32 moveSpeed = recomp_get_config_double("move_speed") / 60.0f;
    f32 rotateSpeed = ((recomp_get_config_double("rotate_speed") / 360.0f) * M_360_DEGREES_F) / 60.0f;
    f32 fovSpeed = recomp_get_config_double("fov_speed") / 60.0f;

    Vec3f moveDir = {0.0f, 0.0f, 0.0f};
    f32 movePower = 0.0f;
    f32 yawPower = 0.0f;
    f32 pitchPower = 0.0f;
    f32 rollPower = 0.0f;
    f32 fovPower = 0.0f;

    switch (controlStickMode) {
    case FREECAM_CTRLSTICK_TRANSL_XZ:
    case FREECAM_CTRLSTICK_TRANSL_XY:
        moveDir.x = joypad.stickX / 127.0f;
        moveDir.z = -(joypad.stickY / 127.0f);
        moveDir.y = 0.0f;

        if (joypad.buttons & R_TRIG) {
            moveDir.y -= 1.0f;
        }
        if (joypad.buttons & L_TRIG) {
            moveDir.y += 1.0f;
        }

        if (controlStickMode == FREECAM_CTRLSTICK_TRANSL_XY) {
            f32 y = moveDir.y;
            moveDir.y = moveDir.z;
            moveDir.z = y;
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
        break;
    case FREECAM_CTRLSTICK_YAW_PITCH:
        yawPower = MIN(1.0f, joypad.stickX / 65.0f);
        pitchPower = -(MIN(1.0f, joypad.stickY / 65.0f));

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

        if (joypad.buttons & R_TRIG) {
            moveDir.y -= 1.0f;
        }
        if (joypad.buttons & L_TRIG) {
            moveDir.y += 1.0f;
        }

        movePower = MIN(1.0f, VECTOR_MAGNITUDE(moveDir));
        break;
    case FREECAM_CTRLSTICK_ROLL:
    case FREECAM_CTRLSTICK_FOV:
        if (controlStickMode == FREECAM_CTRLSTICK_ROLL) {
            rollPower = MIN(1.0f, joypad.stickX / 65.0f);
        } else {
            fovPower = -(MIN(1.0f, joypad.stickY / 65.0f));
        }

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

        if (joypad.buttons & R_TRIG) {
            moveDir.y -= 1.0f;
        }
        if (joypad.buttons & L_TRIG) {
            moveDir.y += 1.0f;
        }

        movePower = MIN(1.0f, VECTOR_MAGNITUDE(moveDir));
        break;
    }

    if (vec3Normalize(&moveDir) != 0.0f) {
        moveDir.x *= moveSpeed * movePower * gUpdateRateF;
        moveDir.y *= moveSpeed * movePower * gUpdateRateF;
        moveDir.z *= moveSpeed * movePower * gUpdateRateF;

        SRT camRotSRT = {
            .transl.x = 0,
            .transl.y = 0,
            .transl.z = 0,
            .scale = 1.0f,
            .yaw = -freeCam.transform.yaw,
            .pitch = freeCam.transform.pitch,
            .roll = freeCam.transform.roll
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
        s32 yaw = freeCam.transform.yaw + (s32)(yawPower * rotateSpeed * gUpdateRateF);
        CIRCLE_WRAP(yaw);
        freeCam.transform.yaw = yaw;
    }

    if (pitchPower != 0.0f) {
        s32 pitch = freeCam.transform.pitch + (s32)(pitchPower * rotateSpeed * gUpdateRateF);
        CIRCLE_WRAP(pitch);
        freeCam.transform.pitch = pitch;
    }

    if (rollPower != 0.0f) {
        s32 roll = freeCam.transform.roll + (s32)(rollPower * rotateSpeed * gUpdateRateF);
        CIRCLE_WRAP(roll);
        freeCam.transform.roll = roll;
    }

    if (fovPower != 0.0f) {
        freeCam.fov += fovPower * fovSpeed * gUpdateRateF;
        freeCam.fov = CLAMP_EXPR(freeCam.fov, 40.0f, 90.0f); // core camera code also does this clamp
    }
}

static void freecamMenuSetup(void) {
    fontWindowUseFont(FREECAM_FONT_WINDOW, FREECAM_FONT);
    fontWindowSetTextColour(FREECAM_FONT_WINDOW, 255, 255, 255, 255, 255);
    fontWindowSetTextBgColour(FREECAM_FONT_WINDOW, 0, 0, 0, 128);
    fontWindowSetCoords(FREECAM_FONT_WINDOW, 8, 8, 320 - 8, 240 - 8);
    fontWindowDisableWordwrap(FREECAM_FONT_WINDOW);
    fontWindowFlushStrings(FREECAM_FONT_WINDOW);
}

static void freecamMenuControlStickMode(f32 x, f32 y) {
    fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Control Stick: ", 0, ALIGN_TOP_LEFT);
    x += fontGetTextWidth(FREECAM_FONT_WINDOW, "Control Stick: ", 0, FREECAM_FONT) - 1;
    switch (controlStickMode) {
    case FREECAM_CTRLSTICK_TRANSL_XZ:
        fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "XZ Translation", 0, ALIGN_TOP_LEFT);
        break;
    case FREECAM_CTRLSTICK_TRANSL_XY:
        fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "XY Translation", 0, ALIGN_TOP_LEFT);
        break;
    case FREECAM_CTRLSTICK_YAW_PITCH:
        fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Yaw/Pitch", 0, ALIGN_TOP_LEFT);
        break;
    case FREECAM_CTRLSTICK_ROLL:
        fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Roll", 0, ALIGN_TOP_LEFT);
        break;
    case FREECAM_CTRLSTICK_FOV:
        fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "FOV", 0, ALIGN_TOP_LEFT);
        break;
    }
}

static void freecamMenuControl(void) {
    freecamMenuSetup();

    fontWindowAddStringXY(FREECAM_FONT_WINDOW, 0, 0, "== FREECAM ==", 0, ALIGN_TOP_LEFT);
    for (s32 i = 0; i < NUM_MENU_ITEMS; i++) {
        s32 x = 0;
        s32 y = (i + 1) * 15;
        if (selectedMenuItem == i) {
            fontWindowAddStringXY(FREECAM_FONT_WINDOW, 0, y, "* ", 0, ALIGN_TOP_LEFT);
            x += fontGetTextWidth(FREECAM_FONT_WINDOW, "* ", 0, FREECAM_FONT) - 1;
        }

        switch (i) {
        case 0:
            freecamMenuControlStickMode(x, y);

            if (selectedMenuItem == i) {
                if ((joypad.pressed & L_JPAD) || (joypad.xSign < 0)) {
                    controlStickMode--;
                    if (controlStickMode < 0) {
                        controlStickMode = 4;
                    }
                }
                if ((joypad.pressed & R_JPAD) || (joypad.xSign > 0) || (joypad.pressed & A_BUTTON)) {
                    controlStickMode++;
                    if (controlStickMode >= 5) {
                        controlStickMode = 0;
                    }
                }
            }
            break;
        case 1:
            fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Reset FOV", 0, ALIGN_TOP_LEFT);
            if (selectedMenuItem == i) {
                if (joypad.pressed & A_BUTTON) {
                    freeCam.fov = freeCam.initialFOV;
                }
            }
            break;
        case 2:
            fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Reset Roll", 0, ALIGN_TOP_LEFT);
            if (selectedMenuItem == i) {
                if (joypad.pressed & A_BUTTON) {
                    freeCam.transform.roll = 0;
                }
            }
            break;
        case 3:
            fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Close Menu", 0, ALIGN_TOP_LEFT);
            if (selectedMenuItem == i) {
                if (joypad.pressed & A_BUTTON) {
                    menuOpen = FALSE;
                }
            }
            break;
        case 4:
            fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Exit Freecam", 0, ALIGN_TOP_LEFT);
            if (selectedMenuItem == i) {
                if (joypad.pressed & A_BUTTON) {
                    freecamExit();
                }
            }
            break;
        case 5:
            fontWindowAddStringXY(FREECAM_FONT_WINDOW, x, y, "Exit w/ Static Cam", 0, ALIGN_TOP_LEFT);
            if (selectedMenuItem == i) {
                if (joypad.pressed & A_BUTTON) {
                    freecamExit();
                    staticCamActive = TRUE;
                }
            }
            break;
        }
    }

    if ((joypad.pressed & U_JPAD) || (joypad.ySign > 0)) {
        selectedMenuItem--;
        if (selectedMenuItem < 0) {
            selectedMenuItem = NUM_MENU_ITEMS - 1;
        }
    }
    if ((joypad.pressed & D_JPAD) || (joypad.ySign < 0)) {
        selectedMenuItem++;
        if (selectedMenuItem >= NUM_MENU_ITEMS) {
            selectedMenuItem = 0;
        }
    }
}

static void freecamQuickSwapPopupControl(void) {
    if (quickSwapPopupTimer > 0.0f) {
        quickSwapPopupTimer -= gUpdateRateF;

        qucikSwapPoupupVisible = TRUE;
        freecamMenuSetup();
        freecamMenuControlStickMode(0, 15);
    } else {
        qucikSwapPoupupVisible = FALSE;
    }
}

static void freecamControl(void) {
    if (menuOpen && (joypad.pressed & B_BUTTON)) {
        menuOpen = FALSE;
    }
    if (joypad.pressed & Z_TRIG) {
        if (!menuOpen) {
            selectedMenuItem = 0;
            quickSwapPopupTimer = 0.0f;
        }

        menuOpen = !menuOpen;
    }
    
    if (menuOpen) {
        freecamMenuControl();
    } else {
        freecamQuickSwapPopupControl();
        freecamCamControl();
    }
}

static _Bool freecamCheckToggleButtonCombo(void) {
    if (objGetPlayer() == NULL) {
        return FALSE;
    }
    if (recomp_get_config_u32("allow_freecam") != 0) {
        return FALSE;
    }

    if (freeCamActive) {
        return (joypad.buttons & R_TRIG) && ((joypad.buttons & U_JPAD) || (joypad.pressed & START_BUTTON));
    } else {
        return (joypad.buttons & R_TRIG) && ((joypad.pressed & U_JPAD) || (joypad.pressed & START_BUTTON));
    }
}

// Note: this hook does not run while paused
RECOMP_HOOK_RETURN("__dll2_CamControl_tick") void freecamTick(void) {
    if (customPause) {
        return; // just in case
    }

    if (freecamCheckToggleButtonCombo()) {
        if (!freeCamActive) {
            freecamEnter();
        } else {
            freecamExit();
        }
    }

    if (staticCamActive) {
        freecamUpdateCamera();
        return;
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

    if (menuGetCurrent() != MENU_NONE) {
        if (menuGetCurrent() != MENU_PAUSE) {
            prevMenu = menuGetCurrent();
        }
        menuSet(MENU_NONE);
    }

    if (recomp_get_config_u32("allow_freecam") != 0) {
        freecamExit();
    }

    if (customPause) {
        if (joypad.pressed & START_BUTTON) {
            freecamUnpause();
        }

        if (freecamCheckToggleButtonCombo()) {
            freecamExit();
        }

        if (customPause) {
            freecamControl();
            freecamUpdateCamera();
        }
    }
}

// Note: does not run while paused (because we swap to no menu)
RECOMP_HOOK("__dll1_cmdmenu_update1") void freecamCmdmenuHook(void) {
    // Hide cmdmenu while freecam is active
    if (freeCamActive) {
        gDLL_1_cmdmenu->vtbl->disable_buttons(0xFFFF);
    }
}

RECOMP_HOOK("mainTick") void freecamInputSteal(void) {
    joypad.buttons = joyGetButtons(0);
    joypad.pressed = joyGetPressed(0);
    joypad.released = joyGetReleased(0);
    joypad.stickX = joyGetStickX(0);
    joypad.stickY = joyGetStickY(0);
    joyGetStickMenuXYSign(0, &joypad.xSign, &joypad.ySign);

    // In recomp, the D-pad and C-buttons are often bound to the same buttons, so let
    // the D-pad cancel out C-buttons so we can still use the D-pad for stuff
    if (joypad.buttons & U_JPAD) {
        joypad.buttons &= ~U_CBUTTONS;
        joypad.pressed &= ~U_CBUTTONS;
    }
    if (joypad.buttons & R_JPAD) {
        joypad.buttons &= ~R_CBUTTONS;
        joypad.pressed &= ~R_CBUTTONS;
    }
    if (joypad.buttons & D_JPAD) {
        joypad.buttons &= ~D_CBUTTONS;
        joypad.pressed &= ~D_CBUTTONS;
    }
    if (joypad.buttons & L_JPAD) {
        joypad.buttons &= ~L_CBUTTONS;
        joypad.pressed &= ~L_CBUTTONS;
    }

    if (joypad.ignoreRTrig) {
        if (joypad.buttons & R_TRIG) {
            joypad.buttons &= ~R_TRIG;
            joypad.pressed &= ~R_TRIG;
        } else {
            joypad.released &= ~R_TRIG; // pretend it was never pressed
            joypad.ignoreRTrig = FALSE;
        }
    }
    
    if (freeCamActive) {
        joyDisableButtons(0, 0xFFFF);
        joyDisableStick(0);
        gMenuJoyXSign[0] = 0;
        gMenuJoyYSign[0] = 0;
    }
}

// Note: this hook runs right before trackDraw normally would (but doesn't because of the pause state)
RECOMP_HOOK_RETURN("__dll4_dll_4_func_FDC") void freecamCustomPauseDraw(void) {
    // monkaW
    if (customPause && gPauseState == 2) {
        gDLL_24_Waterfx->vtbl->tick(/*gUpdateRate*/0);
        gDLL_15_Projgfx->vtbl->func2(/*gUpdateRate*/0, 0);
        //gDLL_14_Modgfx->vtbl->func2(0, 0, 0); // TODO: how to deal with modgfx
        gDLL_13_Expgfx->vtbl->func2(0, /*gUpdateRate*/0, 0, 0);

        trackTick(1); // lfx stuff will crash if this isn't called. an arg of 1 prevents time from advancing
        trackDraw(&gCurGfx, &gCurMtx, &gCurVtx, &gCurPol, &gCurVtx, &gCurPol);
    }
}

RECOMP_ON_GAME_TICK_CALLBACK void freecamMenuDraw(void) {
    if (freeCamActive && (menuOpen || qucikSwapPoupupVisible)) {
        fontWindowDraw(&gCurGfx, &gCurMtx, &gCurVtx, FREECAM_FONT_WINDOW);
    }
}

RECOMP_HOOK("mainHandleMapChange") void freecamMapChangeHook(void) {
    // Disable freecam/static cam on map change
    if (gMainDoMapChange) {
        freecamExit();
        staticCamActive = FALSE;
    }
}
