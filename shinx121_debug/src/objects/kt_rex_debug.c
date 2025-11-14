#include "kt_rex_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/engine/18_objfsa.h"
#include "dlls/engine/33.h"
#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "sys/generic_stack.h"
#include "sys/print.h"

enum KTLogicStates {
    KT_LSTATE_0 = 0,
    KT_LSTATE_1_DEFEATED = 1,
    KT_LSTATE_2_WALK = 2,
    KT_LSTATE_3_TURN_CORNER = 3,
    KT_LSTATE_4_ROAR = 4,
    KT_LSTATE_5_CHARGE = 5,
    KT_LSTATE_6_CHARGE_END = 6,
    KT_LSTATE_7_ZAPPED = 7,
    KT_LSTATE_8_ON_GROUND = 8,
    KT_LSTATE_9_STAND_UP = 9,
    KT_LSTATE_10_FULL_CHARGE = 10,
    KT_LSTATE_11_REVERSE = 11
};

enum KTAnimStates {
    KT_ASTATE_0 = 0,
    KT_ASTATE_1_MOVING_STRAIGHT = 1,
    KT_ASTATE_2_TURN_90 = 2,
    KT_ASTATE_3_TURN_180 = 3,
    KT_ASTATE_4_ROAR = 4,
    KT_ASTATE_5_CHARGE_END = 5,
    KT_ASTATE_6_ZAPPED = 6,
    KT_ASTATE_7_ON_GROUND = 7,
    KT_ASTATE_8_STAND_UP = 8
};

enum KTFlags {
    // Whether the boss is moving clockwise (0) or counter-clockwise (1) around the arena.
    KTFLAG_REVERSED = 0x1,
    // Which arena segment the boss is in.
    // Bits 1-2.
    KTFLAG_SEGMENT = 0x6,
    // Whether the boss was just damaged by the player.
    KTFLAG_DAMAGED = 0x8,
    // Whether the boss can be damaged.
    KTFLAG_VULNERABLE = 0x10,
    // Whether the random chance reverse/charge was already rolled for the current logic state.
    KTFLAG_ROLLED_CHANCE = 0x20
};

#define KTFLAG_GET_SEGMENT(ktflags) ((ktflags >> 1) & 3)
#define KTFLAG_SET_SEGMENT(segment) (segment << 1)

// Note: This is not the only object data. DLL 33 data is prepended to this in memory.
typedef struct {
/*000*/ GenericStack *stateStack;
/*004*/ f32 timer;
        // Where in the current segment the boss in, ranging from 0-1.
        // When reversed (moving counter-clockwise), this value progresses from 1 to 0 instead of 0 to 1.
/*008*/ f32 segmentPos;
/*00C*/ s32 standingUpSegment; // The last segment the boss stood up in after being knocked down.
        // The following fields are curve XYZ coords. These determine the positions used
        // for the A and B sides of each arena segment, for both clockwise and counter-clockwise movement.
/*010*/ f32 segStartCW_X[4];
/*020*/ f32 segStartCW_Y[4];
/*030*/ f32 segStartCW_Z[4];
/*040*/ f32 segEndCW_X[4];
/*050*/ f32 segEndCW_Y[4];
/*060*/ f32 segEndCW_Z[4];
/*070*/ f32 segEndCCW_X[4]; // Reversed positions start here (see above comments)
/*080*/ f32 segEndCCW_Y[4];
/*090*/ f32 segEndCCW_Z[4];
/*0A0*/ f32 segStartCCW_X[4];
/*0B0*/ f32 segStartCCW_Y[4];
/*0C0*/ f32 segStartCCW_Z[4];
        // The current A and B positions of each segment.
        // These are flipped when the boss is reversed (moving counter-clockwise).
/*0D0*/ f32 *segA_X;
/*0D4*/ f32 *segA_Y;
/*0D8*/ f32 *segA_Z;
/*0DC*/ f32 *segB_X;
/*0E0*/ f32 *segB_Y;
/*0E4*/ f32 *segB_Z;
        // World position that the boss should be in.
        // Moves to this position on the next tick.
        // Calculated by combining the current segment pos with the target curve pos.
/*0E8*/ Vec3f pos;
        // Where in the boss's current segment the player is, ranging from 0-1.
        // Note: This is *not* the pos of the player in their current segment. If the player is not in
        // the same segment as the boss, this value is not well defined.
/*0F4*/ f32 playerSegmentPos;
/*0F8*/ s16 turnStartYaw; // The yaw of the boss at the start of a turn.
/*0FA*/ u16 flags;
        // = 0 when walking around normally.
        // = 1 when charging at sabre.
        // = 2 when charging around the arena after getting zapped.
/*0FC*/ u8 anger;
/*0FD*/ u8 roarType;
        // Which arena segment the boss is in (bitfield).
        // This is used to check if the boss is in the same segment as the player.
/*0FE*/ u8 selfSegmentBitfield;
        // Which arena segment the player is in (bitfield).
        // Determined by triggers that set/unset gamebits as the player walks around the arena.
/*0FF*/ u8 playerSegmentBitfield;
/*100*/ u8 laserWallBitfield; // Bitfield of which laser walls are active.
/*101*/ u8 fightProgress; // Increases as the player progresses the fight by damaging the boss.
/*102*/ u8 health;
/*103*/ s8 chargeCounter; // How many segments to charge down. Decrements after each turn while charging.
/*104*/ s32 fxFlags;
/*108*/ u8 breathingSfxIndex;
/*10C*/ SRT unk10C;
/*124*/ SRT unk124;
/*13C*/ SRT unk13C;
/*154*/ SRT unk154;
/*16C*/ Vec3f unk16C;
} KTrex_Data;

typedef struct {
/*00*/ DLL33_ObjSetup base;
/*38*/ f32 speeds[3]; // Movement speed, per "anger" level.
/*44*/ u16 roarTime[3]; // How long a roar lasts.
/*4A*/ u16 vulnerableTime[4]; // How long the boss is vulnerable for.
/*52*/ u8 reverseChance[4]; // Chance to reverse direction.
/*56*/ u8 chargeChance[4]; // Chance to charge without seeing Sabre.
} KTrex_ObjSetup;

enum KTModAnims {
    KTANIM_Idle = 0,
    KTANIM_Walk1 = 1, // anger level 0
    KTANIM_Walk2 = 2, // anger level 1
    KTANIM_Walk3 = 3, // anger level 2
    KTANIM_Roar_Small = 4,
    KTANIM_Roar_VeryBig = 5, // unused
    KTANIM_Roar_Big = 6,
    KTANIM_Walk_Slow = 7, // creepy, unused
    KTANIM_Turn_90_CW_Normal = 8,
    KTANIM_Roar_ChargeEnd1 = 9, // referenced but unused due to logic
    KTANIM_Roar_10 = 10, // unused
    KTANIM_FallDown = 11,
    KTANIM_FlailOnGround = 12,
    KTANIM_GetBackUp = 13,
    KTANIM_Turn_90_CCW_Normal = 14,
    KTANIM_Turn_180 = 15,
    KTANIM_Turn_90_CW_Fast = 16,
    KTANIM_Turn_90_CCW_Fast = 17,
    KTANIM_Roar_ChargeEnd2 = 18
};

enum KTModAnimEvents {
    KTANIM_EVT_0_Speak = 0,
    KTANIM_EVT_1_Footfall_Left = 1,
    KTANIM_EVT_2_Footfall_Right = 2,
    KTANIM_EVT_7 = 7,
    KTANIM_EVT_9_Spit_Partfx_Enable = 9,
    KTANIM_EVT_10_Spit_Partfx_Disable = 10
};

enum KTFxFlags {
    KTFX_Footfall_Right1 = 0x1,
    KTFX_Footfall_Left1 = 0x2,
    KTFX_Footfall_Right2 = 0x4,
    KTFX_Footfall_Left2 = 0x8,
    KTFX_Footfall_Right3 = 0x10,
    KTFX_Footfall_Left3 = 0x20,
    KTFX_Sound_68E = 0x40,
    KTFX_Sound_BigRoar = 0x80,
    KTFX_Sound_IntroRoar = 0x100,
    KTFX_Sound_ChargeEndRoar = 0x200,
    KTFX_Spit_Partfx = 0x800,
    KTFX_Spit_Partfx_Disable = 0x1000,
    KTFX_Sound_FlailRoar = 0x2000,
    KTFX_Sound_Breathing1 = 0x4000,
    KTFX_Sound_Breathing2 = 0x8000,
    KTFX_Sound_WallSlam = 0x10000,
    KTFX_Sound_Explosion = 0x20000,
    KTFX_Sound_GroundScrape = 0x40000,
    KTFX_Sound_PainRoar = 0x80000,
    KTFX_EnablePartFx = 0x100000
};

void kt_rex_debug_tab(Object *obj) {
    DLL33_Data *dll33Data = obj->data;
    ObjFSA_Data *fsa = &dll33Data->fsa;
    KTrex_Data *ktdata = dll33Data->unk3F4;
    if (ktdata == NULL) {
        return;
    }

    if (dbgui_begin_tab_bar("ktrex_tabs")) {
        if (dbgui_begin_tab_item("Obj Setup", NULL)) {
            KTrex_ObjSetup *objsetup = (KTrex_ObjSetup*)obj->setup;

            dbgui_textf("speeds: %f,%f,%f", objsetup->speeds[0], objsetup->speeds[1], objsetup->speeds[2]);
            dbgui_textf("roarTime: %d,%d,%d", objsetup->roarTime[0], objsetup->roarTime[1], objsetup->roarTime[2]);
            dbgui_textf("vulnerableTime: %d,%d,%d,%d", objsetup->vulnerableTime[0], objsetup->vulnerableTime[1], objsetup->vulnerableTime[2], objsetup->vulnerableTime[3]);
            dbgui_textf("reverseChance: %d,%d,%d,%d", objsetup->reverseChance[0], objsetup->reverseChance[1], objsetup->reverseChance[2],  objsetup->reverseChance[3]);
            dbgui_textf("chargeChance: %d,%d,%d,%d", objsetup->chargeChance[0], objsetup->chargeChance[1], objsetup->chargeChance[2],  objsetup->chargeChance[3]);

            dbgui_end_tab_item();
        }
        if (dbgui_begin_tab_item("KTrex Data", NULL)) {
            dbgui_textf("objfsa logic state: %d", fsa->logicState);
            dbgui_textf("objfsa anim state: %d", fsa->animState);

            if (dbgui_tree_node("stateStack")) {
                s32 top = ktdata->stateStack->top;
                for (s32 i = 0; i < ktdata->stateStack->count; i++) {
                    top -= 1;
                    if (top < 0) {
                        top = ktdata->stateStack->capacity - 1;
                    }

                    s32 *ptr = (void*)((u32)ktdata->stateStack->data + top * ktdata->stateStack->elementSize);

                    dbgui_textf("[%d] %d", i, *ptr);
                    
                }
                
                dbgui_tree_pop();
            }

            dbgui_textf("timer: %f", ktdata->timer);
            dbgui_textf("segmentPos: %f", ktdata->segmentPos);
            dbgui_textf("standingUpSegment: %d", ktdata->standingUpSegment);
            if (dbgui_tree_node("segStartCW_X")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segStartCW_X[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segStartCW_Y")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segStartCW_Y[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segStartCW_Z")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segStartCW_Z[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segEndCW_X")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segEndCW_X[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segEndCW_Y")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segEndCW_Y[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segEndCW_Z")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segEndCW_Z[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segEndCCW_X")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segEndCCW_X[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segEndCCW_Y")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segEndCCW_Y[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segEndCCW_Z")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segEndCCW_Z[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segStartCCW_X")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segStartCCW_X[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segStartCCW_Y")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segStartCCW_Y[i]);
                }
                
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("segStartCCW_Z")) {
                for (s32 i = 0; i < 4; i++) {
                    dbgui_textf("[%d] %f", i, ktdata->segStartCCW_Z[i]);
                }
                
                dbgui_tree_pop();
            }
            dbgui_textf("segA_X: %p", ktdata->segA_X);
            dbgui_textf("segA_Y: %p", ktdata->segA_Y);
            dbgui_textf("segA_Z: %p", ktdata->segA_Z);
            dbgui_textf("segB_X: %p", ktdata->segB_X);
            dbgui_textf("segB_Y: %p", ktdata->segB_Y);
            dbgui_textf("segB_Z: %p", ktdata->segB_Z);
            dbgui_textf("pos: %f,%f,%f", ktdata->pos.x, ktdata->pos.y, ktdata->pos.z);
            dbgui_textf("playerSegmentPos: %f", ktdata->playerSegmentPos);
            dbgui_textf("turnStartYaw: %d", ktdata->turnStartYaw);
            dbgui_textf("flags: 0x%X", ktdata->flags);
            dbgui_textf("  flags[REVERSED]: %d", (ktdata->flags & KTFLAG_REVERSED) ? 1 : 0);
            dbgui_textf("  flags[SEGMENT]: %d", KTFLAG_GET_SEGMENT(ktdata->flags));
            dbgui_textf("  flags[DAMAGED]: %d", (ktdata->flags & KTFLAG_DAMAGED) ? 1 : 0);
            dbgui_textf("  flags[VULNERABLE]: %d", (ktdata->flags & KTFLAG_VULNERABLE) ? 1 : 0);
            dbgui_textf("  flags[ROLLED_CHANCE]: %d", (ktdata->flags & KTFLAG_ROLLED_CHANCE) ? 1 : 0);
            dbgui_textf("anger: %d", ktdata->anger);
            dbgui_textf("roarType: %d", ktdata->roarType);
            dbgui_textf("selfSegmentBitfield: 0x%X", ktdata->selfSegmentBitfield);
            dbgui_textf("playerSegmentBitfield: 0x%X", ktdata->playerSegmentBitfield);
            dbgui_textf("laserWallBitfield: 0x%X", ktdata->laserWallBitfield);
            dbgui_textf("fightProgress: %d", ktdata->fightProgress);
            dbgui_textf("health: %d", ktdata->health);
            dbgui_textf("chargeCounter: %d", ktdata->chargeCounter);
            dbgui_textf("fxFlags: 0x%X", ktdata->fxFlags);
            dbgui_textf("breathingSfxIndex: %d", ktdata->breathingSfxIndex);
            dbgui_textf("unk10C: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk10C.yaw, ktdata->unk10C.pitch, ktdata->unk10C.roll,
                ktdata->unk10C.transl.x, ktdata->unk10C.transl.y, ktdata->unk10C.transl.z,
                ktdata->unk10C.scale, ktdata->unk10C.flags);
            dbgui_textf("unk124: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk124.yaw, ktdata->unk124.pitch, ktdata->unk124.roll,
                ktdata->unk124.transl.x, ktdata->unk124.transl.y, ktdata->unk124.transl.z,
                ktdata->unk124.scale, ktdata->unk124.flags);
            dbgui_textf("unk13C: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk13C.yaw, ktdata->unk13C.pitch, ktdata->unk13C.roll,
                ktdata->unk13C.transl.x, ktdata->unk13C.transl.y, ktdata->unk13C.transl.z,
                ktdata->unk13C.scale, ktdata->unk13C.flags);
            dbgui_textf("unk154: rot: 0x%X,0x%X,0x%X pos: %f,%f,%f scale: %f flags: 0x%X", 
                ktdata->unk154.yaw, ktdata->unk154.pitch, ktdata->unk154.roll,
                ktdata->unk154.transl.x, ktdata->unk154.transl.y, ktdata->unk154.transl.z,
                ktdata->unk154.scale, ktdata->unk154.flags);
            dbgui_textf("unk16C: %f,%f,%f", ktdata->unk16C.x, ktdata->unk16C.y, ktdata->unk16C.z);

            dbgui_end_tab_item();
        }

        dbgui_end_tab_bar();
    }
}
