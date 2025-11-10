#include "snowbike_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/engine/4_race.h"
#include "dlls/objects/210_player.h"
#include "sys/gfx/modgfx.h"
#include "sys/main.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "sys/print.h"
#include "dll.h"
#include "functions.h"

typedef struct {
/*00*/ Vec3f unk0;
/*0C*/ Vec3f unkC;
/*18*/ f32 unk18; // also gravity kinda?
/*1C*/ f32 unk1C;
/*20*/ f32 unk20;
/*24*/ f32 _unk24;
/*28*/ f32 unk28; // gravity?
/*2C*/ f32 unk2C;
/*30*/ f32 unk30; // friction?
} IMSnowBike_Data_2AC;

typedef struct {
/*00*/ u8 _unk0[0xE - 0x0];
/*0E*/ s8 turnInput; // for players, this is joystick X
/*0F*/ s8 thrustInput; // for players, this is joystick Y
/*10*/ s32 unk10;
} IMSnowBike_Data_2E0;

typedef struct {
/*000*/ SRT unk0;
/*018*/ RaceStruct unk18; // racePos
/*03C*/ u8 _unk3C[0x48 - 0x3C];
/*048*/ u8 unk48;
/*049*/ u8 unk49;
/*04A*/ u8 _unk4A[0x4C - 0x4A];
/*04C*/ DLL27_Data unk4C;
/*2AC*/ IMSnowBike_Data_2AC unk2AC;
/*2E0*/ IMSnowBike_Data_2E0 unk2E0;
/*2F4*/ DLL_IModgfx *unk2F4;
/*2F8*/ DLL_IModgfx *unk2F8;
/*2FC*/ u8 _unk2FC[0x32C - 0x2FC];
/*32C*/ Vec3f unk32C[5];
/*368*/ u8 _unk368[0x380 - 0x368];
/*380*/ f32 unk380;
/*384*/ f32 unk384;
/*388*/ f32 unk388;
/*38C*/ f32 unk38C;
/*390*/ f32 unk390;
/*394*/ f32 unk394;
/*398*/ f32 unk398;
/*39C*/ f32 unk39C;
/*3A0*/ f32 unk3A0;
/*3A4*/ f32 unk3A4;
/*3A8*/ f32 unk3A8;
/*3AC*/ f32 unk3AC;
/*3B0*/ f32 unk3B0;
/*3B4*/ u32 unk3B4; // sound handle
/*3B8*/ u32 unk3B8;
/*3BC*/ u32 unk3BC;
/*3C0*/ u32 unk3C0;
/*3C4*/ s32 unk3C4;
/*3C8*/ s16 unk3C8;
/*3CA*/ s16 unk3CA;
/*3CC*/ s16 unk3CC;
/*3CE*/ s16 unk3CE;
/*3D0*/ s16 unk3D0;
/*3D2*/ s16 unk3D2; // downward tilt while in air?
/*3D4*/ u8 _unk3D4[0x3D6 - 0x3D4];
/*3D6*/ s16 unk3D6;
/*3D8*/ s16 unk3D8;
/*3DA*/ s8 unk3DA;
/*3DB*/ u8 unk3DB;
/*3DC*/ u8 unk3DC;
/*3DD*/ u8 flags;
/*3DE*/ s8 unk3DE; // state?
/*3DF*/ s8 unk3DF;
/*3E0*/ s8 unk3E0;
/*3E1*/ s8 unk3E1;
} IMSnowBike_Data;

typedef struct {
/*00*/ ObjSetup base;
/*18*/ u8 unk18;
/*19*/ u8 unk19;
/*1A*/ s16 unk1A;
/*1C*/ u8 unk1C;
/*1D*/ u8 unk1D;
/*1E*/ s16 unk1E;
} IMSnowBike_Setup;

enum IMSnowBikeFlags {
    SNOWBIKEFLAG_NONE = 0x0,
    SNOWBIKEFLAG_1 = 0x1,
    SNOWBIKEFLAG_2 = 0x2,
    SNOWBIKEFLAG_GROUNDED = 0x4, // at least one test point touching the ground
    SNOWBIKEFLAG_8 = 0x8,
    SNOWBIKEFLAG_10 = 0x10, // currently in sequence?
    SNOWBIKEFLAG_IS_CPU = 0x20 // is SharpClaw
};

void snowbike_debug_tab(Object *obj) {
    IMSnowBike_Data *objdata = obj->data;

    dbgui_textf("unk0[rot]: 0x%X,0x%X,0x%X", objdata->unk0.yaw, objdata->unk0.pitch, objdata->unk0.roll);
    dbgui_textf("unk0[pos]: %f,%f,%f", objdata->unk0.transl.x, objdata->unk0.transl.y, objdata->unk0.transl.z);
    dbgui_textf("unk0[scale]: %f", objdata->unk0.scale);
    dbgui_textf("unk48: 0x%X", objdata->unk48);
    dbgui_textf("unk49: 0x%X", objdata->unk49);
    dbgui_textf("unk2AC.unk0: %f,%f,%f", objdata->unk2AC.unk0.x, objdata->unk2AC.unk0.y, objdata->unk2AC.unk0.z);
    dbgui_textf("unk2AC.unkC: %f,%f,%f", objdata->unk2AC.unkC.x, objdata->unk2AC.unkC.y, objdata->unk2AC.unkC.z);
    dbgui_textf("unk2AC.unk18: %f", objdata->unk2AC.unk18);
    dbgui_textf("unk2AC.unk1C: %f", objdata->unk2AC.unk1C);
    dbgui_textf("unk2AC.unk20: %f", objdata->unk2AC.unk20);
    dbgui_textf("unk2AC.unk28: %f", objdata->unk2AC.unk28);
    dbgui_textf("unk2AC.unk2C: %f", objdata->unk2AC.unk2C);
    dbgui_textf("unk2AC.unk30: %f", objdata->unk2AC.unk30);
    dbgui_textf("unk2E0.turnInput: %d", objdata->unk2E0.turnInput);
    dbgui_textf("unk2E0.thrustInput: %d", objdata->unk2E0.thrustInput);
    dbgui_textf("unk2E0.unk10: %d", objdata->unk2E0.unk10);
    for (s32 i = 0; i < 5; i++) {
        dbgui_textf("unk32C[%d]: %f,%f,%f", i, objdata->unk32C[i].x, objdata->unk32C[i].y, objdata->unk32C[i].z);
    }
    dbgui_textf("unk380: %f", objdata->unk380);
    dbgui_textf("unk384: %f", objdata->unk384);
    dbgui_textf("unk388: %f", objdata->unk388);
    dbgui_textf("unk38C: %f", objdata->unk38C);
    dbgui_textf("unk390: %f", objdata->unk390);
    dbgui_textf("unk394: %f", objdata->unk394);
    dbgui_textf("unk398: %f", objdata->unk398);
    dbgui_textf("unk39C: %f", objdata->unk39C);
    dbgui_textf("unk3A0: %f", objdata->unk3A0);
    dbgui_textf("unk3A4: %f", objdata->unk3A4);
    dbgui_textf("unk3A8: %f", objdata->unk3A8);
    dbgui_textf("unk3AC: %f", objdata->unk3AC);
    dbgui_textf("unk3B0: %f", objdata->unk3B0);
    dbgui_textf("unk3B4: 0x%X", objdata->unk3B4);
    dbgui_textf("unk3B8: 0x%X", objdata->unk3B8);
    dbgui_textf("unk3BC: 0x%X", objdata->unk3BC);
    dbgui_textf("unk3C0: 0x%X", objdata->unk3C0);
    dbgui_textf("unk3C4: %d", objdata->unk3C4);
    dbgui_textf("unk3C8: %d", objdata->unk3C8);
    dbgui_textf("unk3CA: %d", objdata->unk3CA);
    dbgui_textf("unk3CC (yaw): 0x%X", objdata->unk3CC);
    dbgui_textf("unk3CE (pitch): 0x%X", objdata->unk3CE);
    dbgui_textf("unk3D0 (roll): 0x%X", objdata->unk3D0);
    dbgui_textf("unk3D2: %d", objdata->unk3D2);
    dbgui_textf("unk3D6: %d", objdata->unk3D6);
    dbgui_textf("unk3D8: %d", objdata->unk3D8);
    dbgui_textf("unk3DA: %d", objdata->unk3DA);
    dbgui_textf("unk3DB: 0x%X", objdata->unk3DB);
    dbgui_textf("unk3DC: 0x%X", objdata->unk3DC);
    dbgui_textf("flags[0x1]: %d", (objdata->flags & SNOWBIKEFLAG_1) ? 1 : 0);
    dbgui_textf("flags[0x2]: %d", (objdata->flags & SNOWBIKEFLAG_2) ? 1 : 0);
    dbgui_textf("flags[GROUNDED]: %d", (objdata->flags & SNOWBIKEFLAG_GROUNDED) ? 1 : 0);
    dbgui_textf("flags[0x8]: %d", (objdata->flags & SNOWBIKEFLAG_8) ? 1 : 0);
    dbgui_textf("flags[0x10]: %d", (objdata->flags & SNOWBIKEFLAG_10) ? 1 : 0);
    dbgui_textf("flags[IS_CPU]: %d", (objdata->flags & SNOWBIKEFLAG_IS_CPU) ? 1 : 0);
    dbgui_textf("unk3DE: %d", objdata->unk3DE);
    dbgui_textf("unk3DF: %d", objdata->unk3DF);
    dbgui_textf("unk3E0: %d", objdata->unk3E0);
    dbgui_textf("unk3E1: %d", objdata->unk3E1);
}
