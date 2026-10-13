#include "modding.h"
#include "recomputils.h"

#include "PR/ultratypes.h"
#include "sys/math.h"

#include "recomp/dlls/engine/29_gplay_recomp.h"

RECOMP_HOOK_DLL(gplay_savepoint) void gplay_savepoint_logging_hook(Vec3f *position, s16 yaw, s32 flags, s32 mapLayer) {
    if (position == NULL) {
        recomp_printf("[gplay] Set savepoint: pos=(null)  yaw=0x%X  flags=0x%X  layer=%d\n",
            yaw, flags, mapLayer);
    } else {
        recomp_printf("[gplay] Set savepoint: pos=%f,%f,%f  yaw=0x%X  flags=0x%X  layer=%d\n",
            position->x, position->y, position->z, yaw, flags, mapLayer);
    }
}

RECOMP_HOOK_DLL(gplay_restart_set) void gplay_restart_set_logging_hook(Vec3f *position, s16 yaw, s32 mapLayer) {
    recomp_printf("[gplay] Set restart point: pos=%f,%f,%f  yaw=0x%X  layer=%d\n",
        position->x, position->y, position->z, yaw, mapLayer);
}

RECOMP_HOOK_DLL(gplay_restart_goto) void gplay_restart_goto_logging_hook(void) {
    recomp_printf("[gplay] Go to restart point\n");
}

RECOMP_HOOK_DLL(gplay_restart_clear) void gplay_restart_clear_logging_hook(void) {
    recomp_printf("[gplay] Cleared restart point\n");
}

RECOMP_HOOK_DLL(gplay_start_loaded_game) void gplay_start_loaded_game_logging_hook(void) {
    recomp_printf("[gplay] Go to savepoint\n");
}
