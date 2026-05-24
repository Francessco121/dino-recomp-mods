#include "bwlog_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/objects/210_player.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "sys/print.h"
#include "dll.h"

typedef struct {
/*000*/ DLL27_Data collider;
/*260*/ Vec3f endPoints[2];
/*278*/ Vec3f velocity[2];
/*290*/ f32 powerZ[2]; // forward/backward
/*298*/ f32 powerX[2]; // lateral
/*2A0*/ Vec4f rollCurve; //Roll speed spline
/*2B0*/ f32 rollSpeed; // wobble (smoothed)
/*2B4*/ f32 targetWaterYOffset;
/*2B8*/ f32 paddleTimer; // move timer (when > 0, move forward)
/*2BC*/ f32 rollTimer;
/*2C0*/ f32 rollAcceleration; // wobble
/*2C4*/ s32 _unk2C4;
/*2C8*/ f32 rollCurveProgress;
/*2CC*/ f32 tValueRoll;
/*2D0*/ f32 flowX[2]; //DFriverflow objects' combined push strength in X (values for both ends of log)
/*2D8*/ f32 flowY[2]; //DFriverflow objects' combined push strength in Y (values for both ends of log)
/*2E0*/ f32 flowZ[2]; //DFriverflow objects' combined push strength in Z (values for both ends of log)
/*2E8*/ u8 _unk2E8[0x2F8 - 0x2E8];
/*2F8*/ f32 joyATimer;
/*2FC*/ f32 paddlePower;
/*300*/ f32 unk300[2];
/*308*/ f32 soundPitch;
/*30C*/ f32 soundVolume;
/*310*/ f32 riverflowMagnitude;
/*314*/ u32 soundHandle; //Controls rushing water sound loop
/*318*/ s32 rollAngle; // when rolling, the current roll rotation
/*31C*/ u16 wiggleYOffsets[2]; // y offset for each end point to pitch the log up/down to simulate small waves
/*320*/ u16 joyPressed; // controller buttons pressed
/*322*/ s16 joyStickX; // joystick x
/*324*/ s16 joyStickY; // joystick y
/*326*/ s16 soundPitchPhase;
/*328*/ s16 soundVolumePhase;
/*32A*/ u8 state; // roll state (0 = not rolling, 1 = left, 2 = right)
/*32B*/ u8 playerVehicleAnim;
/*32C*/ u8 joyARecentTap; // a pressed (turns off automatically after a time or if a is pressed again)
/*32D*/ u8 unk32D; // bitfield of which side of the log is touching terrain (0x1 = front, 0x2 = back, 0x3 = both)
/*32E*/ u8 mountState; // see VehicleMountState
/*32F*/ u8 _unk32F[0x338 - 0x32F];
/*338*/ Object *dockpoint;
} BWlog_Data;

void bwlog_debug_tab(Object *obj) {
    BWlog_Data *objdata = obj->data;

    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("endPoints[%d]: %f,%f,%f", i, objdata->endPoints[i].x, objdata->endPoints[i].y, objdata->endPoints[i].z);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("velocity[%d]: %f,%f,%f", i, objdata->velocity[i].x, objdata->velocity[i].y, objdata->velocity[i].z);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("powerZ[%d]: %f", i, objdata->powerZ[i]);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("powerX[%d]: %f", i, objdata->powerX[i]);
    }
    dbgui_textf("rollCurve: %f,%f,%f,%f", objdata->rollCurve.x, objdata->rollCurve.y, objdata->rollCurve.z, objdata->rollCurve.w);
    dbgui_textf("rollSpeed: %f", objdata->rollSpeed);
    dbgui_textf("targetWaterYOffset: %f", objdata->targetWaterYOffset);
    dbgui_textf("paddleTimer: %f", objdata->paddleTimer);
    dbgui_textf("rollTimer: %f", objdata->rollTimer);
    dbgui_textf("rollAcceleration: %f", objdata->rollAcceleration);
    dbgui_textf("rollCurveProgress: %f", objdata->rollCurveProgress);
    dbgui_textf("tValueRoll: %f", objdata->tValueRoll);
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("flowX[%d]: %f", i, objdata->flowX[i]);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("flowY[%d]: %f", i, objdata->flowY[i]);
    }
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("flowZ[%d]: %f", i, objdata->flowZ[i]);
    }
    dbgui_textf("joyATimer: %f", objdata->joyATimer);
    dbgui_textf("paddlePower: %f", objdata->paddlePower);
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("unk300[%d]: %f", i, objdata->unk300[i]);
    }
    dbgui_textf("soundPitch: %f", objdata->soundPitch);
    dbgui_textf("soundVolume: %f", objdata->soundVolume);
    dbgui_textf("riverflowMagnitude: %f", objdata->riverflowMagnitude);
    dbgui_textf("soundHandle: 0x%X", objdata->soundHandle);
    dbgui_textf("rollAngle: %d", objdata->rollAngle);
    for (s32 i = 0; i < 2; i++) {
        dbgui_textf("wiggleYOffsets[%d]: 0x%X", i, objdata->wiggleYOffsets[i]);
    }
    dbgui_textf("joyPressed: 0x%X", objdata->joyPressed);
    dbgui_textf("joyStickX: %d", objdata->joyStickX);
    dbgui_textf("joyStickY: %d", objdata->joyStickY);
    dbgui_textf("soundPitchPhase: %d", objdata->soundPitchPhase);
    dbgui_textf("soundVolumePhase: %d", objdata->soundVolumePhase);
    dbgui_textf("state: %d", objdata->state);
    dbgui_textf("playerVehicleAnim: %d", objdata->playerVehicleAnim);
    dbgui_textf("joyARecentTap: %d", objdata->joyARecentTap);
    dbgui_textf("unk32D: %d", objdata->unk32D);
    dbgui_textf("mountState: %d", objdata->mountState);
    if (objdata->dockpoint != NULL) {
        if (dbgui_tree_node("dockpoint")) {
            object_edit_contents(objdata->dockpoint);
            dbgui_tree_pop();
        }
    } else {
        dbgui_text("dockpoint: (null)");
    }
}
