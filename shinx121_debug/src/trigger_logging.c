// #include "modding.h"
// #include "recompconfig.h"
// #include "recomputils.h"

// #include "PR/ultratypes.h"
// #include "PR/gbi.h"
// #include "PR/gu.h"
// #include "dlls/objects/common/sidekick.h"
// #include "dlls/objects/210_player.h"
// #include "game/objects/object.h"
// #include "game/objects/object_id.h"
// #include "sys/asset_thread.h"
// #include "sys/camera.h"
// #include "sys/dll.h"
// #include "sys/gfx/model.h"
// #include "sys/main.h"
// #include "sys/math.h"
// #include "sys/objects.h"
// #include "sys/objtype.h"
// #include "sys/objmsg.h"
// #include "sys/gfx/gx.h"
// #include "sys/map.h"
// #include "dll.h"
// #include "types.h"
// #include "functions.h"

// #include "trigger.h"

// typedef enum {
//     TRIGGER_DEBUG_LOGGING_ON,
//     TRIGGER_DEBUG_LOGGING_OFF,
// } TriggerDebugLogging;

// static s32 triggerCmdPrintStart = TRUE;

// static int trigger_printf(Object *self, const char *fmt, ...) {
//     va_list args;
//     int ret;

//     va_start(args, fmt);

//     if (triggerCmdPrintStart) {
//         recomp_printf("Trigger uID 0x%X:\n  ", self->setup == NULL ? 0xFFFFFFFF : self->setup->uID);
//         triggerCmdPrintStart = FALSE;
//     } else {
//         recomp_printf("  ");
//     }

//     ret = recomp_vprintf(fmt, args);

//     va_end(args);

//     return ret;
// }

// #include "recomp/dlls/objects/325_trigger_recomp.h"

// // @recomp: Mirrors trigger_process_commands logic exactly but only prints out debug messages instead of acting on commands
// //          This has the side effect of printing all commands to be ran before running any of them, making debugging slightly
// //          annoying since it's less obvious which command caused an issue. The silver lining is that this is a non-intrusive
// //          patch and doesn't stop other mods from updating the process logic (like DinoMod).
// RECOMP_HOOK_DLL(trigger_process_commands) void trigger_process_commands_print_hook(Object *self, Object *activator, s8 dir, s32 activatorDistSquared) {
//     Trigger_Setup* setup; // sp+74
//     Trigger_Data* objdata; // sp+70
//     TriggerCommand *cmd;
//     u8 i;
//     Object* sidekick;

//     if (recomp_get_config_u32("trigger_logging") != TRIGGER_DEBUG_LOGGING_ON) {
//         return;
//     }

//     triggerCmdPrintStart = TRUE;

//     objdata = (Trigger_Data*)self->data;
//     setup = (Trigger_Setup*)self->setup;
    
//     for (i = 0, cmd = setup->commands; i < 8; i++, cmd++) {
//         if (cmd->id == 0) {
//             // No command assigned to this slot
//             continue;
//         }

//         if ((objdata->flags & TRG_RESTORE_ENTERED_STATE) && !(cmd->condition & CMD_COND_RESTORE)) {
//             continue;
//         }
        
//         if (!(cmd->condition & CMD_COND_CONTINUOUS)) {
//             // Not continuous, check for entry/exit tick
//             if (dir == 1) {
//                 // Entered
//                 if (!(cmd->condition & CMD_COND_IN)) {
//                     continue;
//                 }
//                 if ((objdata->flags & TRG_ACTIVATOR_ENTERED) && !(cmd->condition & CMD_COND_RE_ENTER)) {
//                     continue;
//                 }
//             } else if (dir == -1) {
//                 // Exited
//                 if (!(cmd->condition & CMD_COND_OUT)) {
//                     continue;
//                 }
//                 if ((objdata->flags & TRG_ACTIVATOR_EXITED) && !(cmd->condition & CMD_COND_RE_EXIT)) {
//                     continue;
//                 }
//             } else {
//                 continue;
//             }
//         } else {
//             // Continuous, check if in/out
//             if (cmd->condition & CMD_COND_IN) {
//                 if (dir < 0) {
//                     continue;
//                 }
//             } else if (cmd->condition & CMD_COND_OUT) {
//                 if (dir > 0) {
//                     continue;
//                 }
//             }
//         }
        
//         switch (cmd->id) {                  /* switch 1 */
//         case TRG_CMD_HAZARD: 
//             trigger_printf(self, "Trigger [%d], Gamplay Vulnerable [%d]\n", i, cmd->param1);
//             break;
//         case TRG_CMD_MUSIC_ACTION: 
//             // TODO: check for side effects in amseq
//             if ((dir < 0) && (gDLL_5_AMSEQ->vtbl->func2(self, (cmd->param2 | (cmd->param1 << 8))) != 0)) {
//                 trigger_printf(self, "Trigger [%d], Music Action,       Action Num [%d] Free\n", i, (cmd->param2 | (cmd->param1 << 8)));
//             } else {
//                 trigger_printf(self, "Trigger [%d], Music Action,       Action Num [%d] Set\n", i, (cmd->param2 | (cmd->param1 << 8)));
//             }
//             break;
//         case TRG_CMD_SOUND: 
//             trigger_printf(self, "Trigger [%d], Sound FX,           Action Num [%d],Handle Num [%d]\n", 
//                 i, (cmd->param2 | (cmd->param1 << 8)), objdata->soundHandles[i]);
//             break;
//         case TRG_CMD_CAMERA_ACTION: 
//             trigger_printf(self, "Trigger [%d], Camera,             Action [%d], Camera Num [%d], PassDir [%d]\n", 
//                 i, cmd->param1, cmd->param2, dir);
//             break;
//         case TRG_CMD_TRACK: 
//             // "Trigger [%d], Track Sky On"
//             // "Trigger [%d], Track Sky Off"
//             // "Trigger [%d], Track AntiAlias On"
//             // "Trigger [%d], Track AntiAlias Off"
//             // "Trigger [%d], Track SkyObjects On"
//             // "Trigger [%d], Track SkyObjects Off"
//             // "Trigger [%d], Track Dome On"
//             // "Trigger [%d], Track Dome Off"
//             // "Trigger [%d], Track MrSheen On %d"
//             // "Trigger [%d], Track MrSheen Off"
//             trigger_printf(self, "Trigger [%d], Track [%d, %d]\n", i, cmd->param1, cmd->param2);
//             break;
//         case TRG_CMD_5: 
//             trigger_printf(self, "Trigger [%d], CMD_5\n", i);
//             break;
//         case TRG_CMD_ENV_FX:
//             trigger_printf(self, "Trigger [%d], Environment Effect, Action Num [%d], Range [%d]\n", 
//                 i, (cmd->param2 | (cmd->param1 << 8)), activatorDistSquared);
//             break;
//         case TRG_CMD_LIGHTING:
//             trigger_printf(self, "Trigger [%d], Lighting,           Action      [%d], Range [%d], PassDir [%d]\n", 
//                 i, (cmd->param2 | (cmd->param1 << 8)), activatorDistSquared, dir);
//             break;
//         case TRG_CMD_ANIM_SEQ:
//             switch (cmd->param1) {
//                 case 0:
//                 case 3:
//                     trigger_printf(self, "Trigger [%d], Anim Sequence,      SequenceID [%d], Activate\n", i, cmd->param2);
//                     break;
//                 case 1:
//                     trigger_printf(self, "Trigger [%d], Anim Sequence,      SequenceID [%d], Flag = 1\n", i, cmd->param2);
//                     break;
//                 case 2:
//                     trigger_printf(self, "Trigger [%d], Anim Sequence,      SequenceID [%d], Flag = 0\n", i, cmd->param2);
//                     break;
//             }
//             break;
//         case TRG_CMD_TRIGGER:
//             trigger_printf(self, "Trigger [%d], Trigger,            Local ID   [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         // case TRG_CMD_?
//             // "Storyboard disabled, please remove trigger\n"
//         case TRG_CMD_LOD_MODEL:
//             trigger_printf(self, "Trigger [%d], LOD Model [%d]\n", i, cmd->param1);
//             break;
//         case TRG_CMD_F:
//             trigger_printf(self, "Trigger [%d], Setup Point,        Level      [%d], SetupPoint [%d]\n", i, cmd->param1, cmd->param2);
//             break;
//         case TRG_CMD_FLAG: {
//             u16 param = cmd->param2 | (cmd->param1 << 8);
//             s32 entry = param & 0x3FFF;
//             param >>= 14;
//             u32 value = main_get_bits(entry);
//             u32 prevValue = value;

//             if (param == 0) {
//                 value = 0;
//             } else if (param == 1) {
//                 value = -1;
//             } else if (param == 2) {
//                 value = ~value;
//             }

//             trigger_printf(self, "Trigger [%d], Bits [0x%x] %d -> %d \n", i, entry, prevValue, value);
//             break;
//         }
//         case TRG_CMD_FLAG_TOGGLE: {
//             u16 param = cmd->param2 | (cmd->param1 << 8);
//             s32 entry = param & 0x1FFF;
//             param >>= 13;
//             u32 value = main_get_bits(entry);
//             u32 prevValue = value;
//             value ^= (1 << param);

//             trigger_printf(self, "Trigger [%d], Bits Toggle [0x%x] %d -> %d\n", i, entry, prevValue, value);
//             break;
//         }
//         case TRG_CMD_ENABLE_OBJ_GROUP:
//             trigger_printf(self, "Trigger [%d], Object Load [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_DISABLE_OBJ_GROUP:
//             trigger_printf(self, "Trigger [%d], Object Free [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_TOGGLE_OBJ_GROUP:
//             trigger_printf(self, "Trigger [%d], Object Toggle [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_TEXTURE_LOAD:
//             trigger_printf(self, "Trigger [%d], Tex Load\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_TEXTURE_FREE:
//             trigger_printf(self, "Trigger [%d], Tex Free\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_SET_MAP_SETUP:
//             trigger_printf(self, "Trigger [%d], SetMapSetup [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_SCRIPT:
//             trigger_printf(self, "Script [%d], Subscript [%d]\n", cmd->param1, cmd->param2);
//             break;
//         case TRG_CMD_WORLD_ENABLE_OBJ_GROUP:
//             trigger_printf(self, "Trigger [%d], WorldObjectLoad [%d, %d]\n", i, cmd->param2, cmd->param1);
//             break;
//         case TRG_CMD_WORLD_DISABLE_OBJ_GROUP:
//             // "Trigger [%d], Object Free\n"
//             trigger_printf(self, "Trigger [%d], WorldObjectFree [%d, %d]\n", i, cmd->param2, cmd->param1);
//             break;
//         case TRG_CMD_KYTE_FLIGHT_GROUP:
//             trigger_printf(self, "Trigger [%d], KyteFlightGroup [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_KYTE_TALK_SEQ:
//             trigger_printf(self, "Trigger [%d], KyteTalkSeq [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_WORLD_SET_MAP_SETUP:
//             trigger_printf(self, "Trigger [%d], WorldSetMapSetup [%d, %d]\n", i, cmd->param2, cmd->param1);
//             break;
//         case TRG_CMD_11:
//             trigger_printf(self, "Trigger [%d], Tricky Bit 0x4E2 = [%d]\n", i, cmd->param2 | (cmd->param1 << 8));
//             break;
//         case TRG_CMD_SAVE_GAME:
//             trigger_printf(self, "Trigger [%d], SaveGame [%d]\n", i, cmd->param2);
//             break;
//         case TRG_CMD_MAP_LAYER:
//             if ((s8)cmd->param2 == -1) {
//                 // Note: This mod isn't DinoMod but it's convienent to have this check. param2 will only be -1 in this case.
//                 trigger_printf(self, "[DinoMod] Trigger [%d], Set MapLayer [%d]\n", i, cmd->param1);
//             } else {
//                 if (cmd->param1 == 0) {
//                     trigger_printf(self, "Trigger [%d], Increment MapLayer\n", i);
//                 } else {
//                     trigger_printf(self, "Trigger [%d], Decrement MapLayer\n", i);
//                 }
//             }
//             break;
//         case TRG_CMD_RESTART:
//             switch (cmd->param1) {            /* switch 4; irregular */
//             case 0:                         /* switch 4 */
//                 // "Restart Set [%d]\n"
//                 trigger_printf(self, "Restart Set [%d]\n", i);
//                 break;
//             case 1:                         /* switch 4 */
//                 // "Restart Clear [%d]\n"
//                 trigger_printf(self, "Restart Clear [%d]\n", i);
//                 break;
//             case 2:                         /* switch 4 */
//                 // "Restart Goto [%d]\n"
//                 trigger_printf(self, "Restart Goto [%d]\n", i);
//                 break;
//             }
//             break;
//         case TRG_CMD_SIDEKICK:
//             sidekick = get_sidekick();
//             if (sidekick != NULL) {
//                 switch (cmd->param1) {       /* switch 5; irregular */
//                 case 0:                     /* switch 5 */
//                     trigger_printf(self, "Trigger [%d], sidekick func23\n", i);
//                     break;
//                 case 1:                     /* switch 5 */
//                     trigger_printf(self, "killing sidekick\n");
//                     break;
//                 case 2:                     /* switch 5 */
//                     trigger_printf(self, "Trigger [%d], sidekick findobj\n", i);
//                     break;
//                 }
//             }
//             break;
//         case TRG_CMD_WATER_FALLS_FLAGS:
//         case TRG_CMD_WATER_FALLS_FLAGS2:
//             trigger_printf(self, "Trigger [%d], amSfxWaterFallsSetFlags,   Action [%d], PassDir [%d]\n", i, cmd->param1, dir);
//             break;
//         }
//     }
// }
