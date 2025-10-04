#include "modding.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "PR/gu.h"
#include "dlls/objects/common/sidekick.h"
#include "dlls/objects/210_player.h"
#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "recompconfig.h"
#include "recomputils.h"
#include "sys/asset_thread.h"
#include "sys/camera.h"
#include "sys/dll.h"
#include "sys/gfx/model.h"
#include "sys/main.h"
#include "sys/math.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "sys/objmsg.h"
#include "sys/gfx/gx.h"
#include "sys/map.h"
#include "dll.h"
#include "types.h"
#include "functions.h"

typedef enum {
    TRIGGER_DEBUG_LOGGING_ON,
    TRIGGER_DEBUG_LOGGING_OFF,
} TriggerDebugLogging;

static s32 triggerCmdPrintStart = TRUE;

static int trigger_printf(Object *self, const char *fmt, ...) {
    va_list args;
    int ret;

    va_start(args, fmt);

    if (triggerCmdPrintStart) {
        recomp_printf("Trigger uID 0x%X:\n  ", self->createInfo == NULL ? 0xFFFFFFFF : self->createInfo->uID);
        triggerCmdPrintStart = FALSE;
    } else {
        recomp_printf("  ");
    }

    ret = recomp_vprintf(fmt, args);

    va_end(args);

    return ret;
}

#include "recomp/dlls/objects/325_trigger_recomp.h"

typedef struct {
    u8 condition; // TriggerCommandConditionFlags enum
    u8 id;
    // For some commands, param1 and 2 are read as a single u16, but 
    // in code they are read individually and then combined with bit math.
    u8 param1;
    u8 param2;
} TriggerCommand;

typedef struct {
/*00*/ ObjCreateInfo base;
/*18*/ TriggerCommand commands[8];
/*38*/ s16 localID; // TODO: needs verification
/*3A*/ u8 sizeX; // unit depends on trigger type
/*3B*/ u8 sizeY; // unit depends on trigger type
/*3C*/ u8 sizeZ; // unit depends on trigger type
/*3D*/ u8 rotationY; // unit depends on trigger type
/*3E*/ u8 rotationX; // unit depends on trigger type
/*3F*/ u8 _unk3F[4];
/// The object type of the object that can activate the trigger.
/// If multiple instances of the given type exist in the scene, the one
/// closest to the trigger will be used.
/// Exceptions:
/// 0 - Always the primary player (even if multiple player instances exist)
/// 1 - Always the primary sidekick (even if multiple sidekick instances exist)
/// 2 - The camera(?)
/*43*/ u8 activatorObjType;
// Game bit flag to save the entered state of the trigger in.
// This flag will be set the first time the trigger is entered. If the flag is already
// set upon object creation, the entered state of the trigger will be restored, possibly
// running commands on the next update that support being restored.
/*44*/ s16 bitFlagID;
// Number of game ticks from object creation to wait before activating a 
// TriggerTime instance.
/*46*/ u16 timerDuration;
// Game bit flags that must be *all* set before the trigger can activate.
// Only supported by TriggerPlane and TriggerBits (plane only supports one flag to check).
// A negative ID indicates that there is no flag to check for that condition slot. 
/*48*/ s16 conditionBitFlagIDs[4];
} TriggerCreateInfo;

DLL_INTERFACE(DLL_TriggerScript) {
    /*:*/ DLL_INTERFACE_BASE(DLL);
    // Array length will vary
    void (*subscripts[1])(Object *trigger, Object *activator, s8 dir, s32 activatorDistSquared);
};

typedef struct {
/*00*/ u8 flags; // TriggerFlags enum
/*01*/ u8 _unk1[3];
/*04*/ f32 radiusSquared;
/*08*/ u8 _unk8[4];
/*0C*/ s32 elapsedTicks; // for TriggerTime
/*10*/ Vec3f lookVector; // for TriggerPlane
/*1C*/ f32 lookVectorNegDot;
/*20*/ Vec3f activatorPrevPos;
/*2C*/ Vec3f activatorCurrPos;
/*38*/ Vec3f planeMin;
/*44*/ Vec3f planeMax;
/*50*/ u8 _unk50[8];
/*58*/ s16 bitFlagID;
/*5A*/ s16 conditionBitFlagIDs[4];
/*62*/ u8 _unk62[2];
/*64*/ u32 soundHandles[8];
// Special "script" DLLs where each export is a "subscript".
/*84*/ DLL_TriggerScript *scripts[8];
} TriggerState;

typedef enum {
    // Activator entered at least once
    TRG_ACTIVATOR_ENTERED = 0x1,
    // Activator exited at least once
    TRG_ACTIVATOR_EXITED = 0x2,
    TRG_RESTORE_ENTERED_STATE = 0x4,
    TRG_FIRST_TICK = 0x40
} TriggerFlags;

typedef enum {
    // When activator is "inside" the trigger
    CMD_COND_IN = 0x1,
    // When activator is "outside" of the trigger
    CMD_COND_OUT = 0x2,
    // Command can be activated if trigger is entered more than once
    CMD_COND_RE_ENTER = 0x4,
    // Command can be activated if trigger is exited more than once
    CMD_COND_RE_EXIT = 0x8,
    // Command is activated every game tick the in/out conditions are met,
    // and not just on the initial entry/exit tick
    CMD_COND_CONTINUOUS = 0x10,
    // Activate the command when restoring the trigger's entered state
    CMD_COND_RESTORE = 0x20
} TriggerCommandConditionFlags;

typedef enum {
    TRG_CMD_HAZARD = 0x1, // "gameplay vulnerable"?
    TRG_CMD_2 = 0x2, // not implemented
    TRG_CMD_MUSIC_ACTION = 0x3,
    TRG_CMD_SOUND = 0x4,
    TRG_CMD_5 = 0x5,
    TRG_CMD_CAMERA_ACTION = 0x6,
    TRG_CMD_7 = 0x7, // not implemented
    TRG_CMD_TRACK = 0x8,
    TRG_CMD_9 = 0x9, // not implemented
    TRG_CMD_ENV_FX = 0xA,
    TRG_CMD_ANIM_SEQ = 0xB,
    TRG_CMD_TRIGGER = 0xC,
    TRG_CMD_LIGHTING = 0xD,
    TRG_CMD_E = 0xE, // not implemented
    TRG_CMD_F = 0xF,
    TRG_CMD_LOD_MODEL = 0x10,
    TRG_CMD_11 = 0x11, // TRG_CMD_TRICKY_?
    TRG_CMD_FLAG = 0x12,
    TRG_CMD_ENABLE_OBJ_GROUP = 0x13,
    TRG_CMD_DISABLE_OBJ_GROUP = 0x14,
    TRG_CMD_TEXTURE_LOAD = 0x15,
    TRG_CMD_TEXTURE_FREE = 0x16,
    TRG_CMD_17 = 0x17, // not implemented
    TRG_CMD_SET_MAP_SETUP = 0x18,
    TRG_CMD_SCRIPT = 0x19,
    TRG_CMD_WORLD_ENABLE_OBJ_GROUP = 0x1A,
    TRG_CMD_WORLD_DISABLE_OBJ_GROUP = 0x1B,
    TRG_CMD_KYTE_FLIGHT_GROUP = 0x1C,
    TRG_CMD_KYTE_TALK_SEQ = 0x1D,
    TRG_CMD_WORLD_SET_MAP_SETUP = 0x1E,
    TRG_CMD_SAVE_GAME = 0x1F,
    TRG_CMD_MAP_LAYER = 0x20,
    TRG_CMD_FLAG_TOGGLE = 0x21,
    TRG_CMD_TOGGLE_OBJ_GROUP = 0x22,
    TRG_CMD_RESTART = 0x23,
    TRG_CMD_WATER_FALLS_FLAGS = 0x24,
    TRG_CMD_WATER_FALLS_FLAGS2 = 0x25,
    TRG_CMD_SIDEKICK = 0x26
} TriggerCommandID;

// @recomp: Mirrors trigger_process_commands logic exactly but only prints out debug messages instead of acting on commands
//          This has the side effect of printing all commands to be ran before running any of them, making debugging slightly
//          annoying since it's less obvious which command caused an issue. The silver lining is that this is a non-intrusive
//          patch and doesn't stop other mods from updating the process logic (like DinoMod).
RECOMP_HOOK_DLL(trigger_process_commands) void trigger_process_commands_print_hook(Object *self, Object *activator, s8 dir, s32 activatorDistSquared) {
    TriggerCreateInfo* createInfo; // sp+74
    TriggerState* state; // sp+70
    TriggerCommand *cmd;
    u8 i;
    Object* sidekick;

    if (recomp_get_config_u32("trigger_logging") != TRIGGER_DEBUG_LOGGING_ON) {
        return;
    }

    triggerCmdPrintStart = TRUE;

    state = (TriggerState*)self->state;
    createInfo = (TriggerCreateInfo*)self->createInfo;
    
    for (i = 0, cmd = createInfo->commands; i < 8; i++, cmd++) {
        if (cmd->id == 0) {
            // No command assigned to this slot
            continue;
        }

        if ((state->flags & TRG_RESTORE_ENTERED_STATE) && !(cmd->condition & CMD_COND_RESTORE)) {
            continue;
        }
        
        if (!(cmd->condition & CMD_COND_CONTINUOUS)) {
            // Not continuous, check for entry/exit tick
            if (dir == 1) {
                // Entered
                if (!(cmd->condition & CMD_COND_IN)) {
                    continue;
                }
                if ((state->flags & TRG_ACTIVATOR_ENTERED) && !(cmd->condition & CMD_COND_RE_ENTER)) {
                    continue;
                }
            } else if (dir == -1) {
                // Exited
                if (!(cmd->condition & CMD_COND_OUT)) {
                    continue;
                }
                if ((state->flags & TRG_ACTIVATOR_EXITED) && !(cmd->condition & CMD_COND_RE_EXIT)) {
                    continue;
                }
            } else {
                continue;
            }
        } else {
            // Continuous, check if in/out
            if (cmd->condition & CMD_COND_IN) {
                if (dir < 0) {
                    continue;
                }
            } else if (cmd->condition & CMD_COND_OUT) {
                if (dir > 0) {
                    continue;
                }
            }
        }
        
        switch (cmd->id) {                  /* switch 1 */
        case TRG_CMD_HAZARD: 
            trigger_printf(self, "Trigger [%d], Gamplay Vulnerable [%d]\n", i, cmd->param1);
            break;
        case TRG_CMD_MUSIC_ACTION: 
            // TODO: check for side effects in amseq
            if ((dir < 0) && (gDLL_5_AMSEQ->vtbl->func2(self, (cmd->param2 | (cmd->param1 << 8))) != 0)) {
                trigger_printf(self, "Trigger [%d], Music Action,       Action Num [%d] Free\n", i, (cmd->param2 | (cmd->param1 << 8)));
            } else {
                trigger_printf(self, "Trigger [%d], Music Action,       Action Num [%d] Set\n", i, (cmd->param2 | (cmd->param1 << 8)));
            }
            break;
        case TRG_CMD_SOUND: 
            trigger_printf(self, "Trigger [%d], Sound FX,           Action Num [%d],Handle Num [%d]\n", 
                i, (cmd->param2 | (cmd->param1 << 8)), state->soundHandles[i]);
            break;
        case TRG_CMD_CAMERA_ACTION: 
            trigger_printf(self, "Trigger [%d], Camera,             Action [%d], Camera Num [%d], PassDir [%d]\n", 
                i, cmd->param1, cmd->param2, dir);
            break;
        case TRG_CMD_TRACK: 
            // "Trigger [%d], Track Sky On"
            // "Trigger [%d], Track Sky Off"
            // "Trigger [%d], Track AntiAlias On"
            // "Trigger [%d], Track AntiAlias Off"
            // "Trigger [%d], Track SkyObjects On"
            // "Trigger [%d], Track SkyObjects Off"
            // "Trigger [%d], Track Dome On"
            // "Trigger [%d], Track Dome Off"
            // "Trigger [%d], Track MrSheen On %d"
            // "Trigger [%d], Track MrSheen Off"
            trigger_printf(self, "Trigger [%d], Track [%d, %d]\n", i, cmd->param1, cmd->param2);
            break;
        case TRG_CMD_5: 
            trigger_printf(self, "Trigger [%d], CMD_5\n", i);
            break;
        case TRG_CMD_ENV_FX:
            trigger_printf(self, "Trigger [%d], Environment Effect, Action Num [%d], Range [%d]\n", 
                i, (cmd->param2 | (cmd->param1 << 8)), activatorDistSquared);
            break;
        case TRG_CMD_LIGHTING:
            trigger_printf(self, "Trigger [%d], Lighting,           Action      [%d], Range [%d], PassDir [%d]\n", 
                i, (cmd->param2 | (cmd->param1 << 8)), activatorDistSquared, dir);
            break;
        case TRG_CMD_ANIM_SEQ:
            switch (cmd->param1) {
                case 0:
                case 3:
                    trigger_printf(self, "Trigger [%d], Anim Sequence,      SequenceID [%d], Activate\n", i, cmd->param2);
                    break;
                case 1:
                    trigger_printf(self, "Trigger [%d], Anim Sequence,      SequenceID [%d], Flag = 1\n", i, cmd->param2);
                    break;
                case 2:
                    trigger_printf(self, "Trigger [%d], Anim Sequence,      SequenceID [%d], Flag = 0\n", i, cmd->param2);
                    break;
            }
            break;
        case TRG_CMD_TRIGGER:
            trigger_printf(self, "Trigger [%d], Trigger,            Local ID   [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        // case TRG_CMD_?
            // "Storyboard disabled, please remove trigger\n"
        case TRG_CMD_LOD_MODEL:
            trigger_printf(self, "Trigger [%d], LOD Model [%d]\n", i, cmd->param1);
            break;
        case TRG_CMD_F:
            trigger_printf(self, "Trigger [%d], Setup Point,        Level      [%d], SetupPoint [%d]\n", i, cmd->param1, cmd->param2);
            break;
        case TRG_CMD_FLAG:
            trigger_printf(self, "Trigger [%d], Bits [0x%x]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_FLAG_TOGGLE:
            trigger_printf(self, "Trigger [%d], Bits Toggle [0x%x]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_ENABLE_OBJ_GROUP:
            trigger_printf(self, "Trigger [%d], Object Load [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_DISABLE_OBJ_GROUP:
            trigger_printf(self, "Trigger [%d], Object Free [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_TOGGLE_OBJ_GROUP:
            trigger_printf(self, "Trigger [%d], Object Toggle [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_TEXTURE_LOAD:
            trigger_printf(self, "Trigger [%d], Tex Load\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_TEXTURE_FREE:
            trigger_printf(self, "Trigger [%d], Tex Free\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_SET_MAP_SETUP:
            trigger_printf(self, "Trigger [%d], SetMapSetup [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_SCRIPT:
            trigger_printf(self, "Script [%d], Subscript [%d]\n", cmd->param1, cmd->param2);
            break;
        case TRG_CMD_WORLD_ENABLE_OBJ_GROUP:
            trigger_printf(self, "Trigger [%d], WorldObjectLoad [%d, %d]\n", i, cmd->param2, cmd->param1);
            break;
        case TRG_CMD_WORLD_DISABLE_OBJ_GROUP:
            // "Trigger [%d], Object Free\n"
            trigger_printf(self, "Trigger [%d], WorldObjectFree [%d, %d]\n", i, cmd->param2, cmd->param1);
            break;
        case TRG_CMD_KYTE_FLIGHT_GROUP:
            trigger_printf(self, "Trigger [%d], KyteFlightGroup [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_KYTE_TALK_SEQ:
            trigger_printf(self, "Trigger [%d], KyteTalkSeq [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_WORLD_SET_MAP_SETUP:
            trigger_printf(self, "Trigger [%d], WorldSetMapSetup [%d, %d]\n", i, cmd->param2, cmd->param1);
            break;
        case TRG_CMD_11:
            trigger_printf(self, "Trigger [%d], Tricky Bit 0x4E2 = [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
            break;
        case TRG_CMD_SAVE_GAME:
            trigger_printf(self, "Trigger [%d], SaveGame [%d]\n", i, cmd->param2);
            break;
        case TRG_CMD_MAP_LAYER:
            if ((s8)cmd->param2 == -1) {
                // Note: This mod isn't DinoMod but it's convienent to have this check. param2 will only be -1 in this case.
                trigger_printf(self, "[DinoMod] Trigger [%d], Set MapLayer [%d]\n", i, cmd->param1);
            } else {
                if (cmd->param1 == 0) {
                    trigger_printf(self, "Trigger [%d], Increment MapLayer\n", i);
                } else {
                    trigger_printf(self, "Trigger [%d], Decrement MapLayer\n", i);
                }
            }
            break;
        case TRG_CMD_RESTART:
            switch (cmd->param1) {            /* switch 4; irregular */
            case 0:                         /* switch 4 */
                // "Restart Set [%d]\n"
                trigger_printf(self, "Restart Set [%d]\n", i);
                break;
            case 1:                         /* switch 4 */
                // "Restart Clear [%d]\n"
                trigger_printf(self, "Restart Clear [%d]\n", i);
                break;
            case 2:                         /* switch 4 */
                // "Restart Goto [%d]\n"
                trigger_printf(self, "Restart Goto [%d]\n", i);
                break;
            }
            break;
        case TRG_CMD_SIDEKICK:
            sidekick = get_sidekick();
            if (sidekick != NULL) {
                switch (cmd->param1) {       /* switch 5; irregular */
                case 0:                     /* switch 5 */
                    trigger_printf(self, "Trigger [%d], sidekick func23\n", i);
                    break;
                case 1:                     /* switch 5 */
                    trigger_printf(self, "killing sidekick\n");
                    break;
                case 2:                     /* switch 5 */
                    trigger_printf(self, "Trigger [%d], sidekick findobj\n", i);
                    break;
                }
            }
            break;
        case TRG_CMD_WATER_FALLS_FLAGS:
        case TRG_CMD_WATER_FALLS_FLAGS2:
            trigger_printf(self, "Trigger [%d], amSfxWaterFallsSetFlags,   Action [%d], PassDir [%d]\n", i, cmd->param1, dir);
            break;
        }
    }
}
