#include "modding.h"
#include "recomputils.h"
#include "dbgui.h"

#include "PR/os.h"
#include "PR/ultratypes.h"
#include "game/objects/object_id.h"
#include "game/objects/object.h"
#include "game/objects/setups/49_SnowBike.h"
#include "game/objects/setups/51_Generic.h"
#include "dlls/objects/210_player.h"
#include "sys/pi.h"
#include "sys/joypad.h"
#include "sys/memory.h"
#include "sys/map.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "common.h"
#include "dll.h"

static s32 windowOpen = 0;
static Object *bike;

static Object *validate_bike(void) {
    s32 objListCount;
    Object **objList = objGetAllOfType(OBJTYPE_Vehicle, &objListCount);
    for (s32 i = 0; i < objListCount; i++) {
        if (objList[i] == bike) {
            return objList[i];
        }
    }

    // Must have been destroyed
    bike = NULL;
    return NULL;
}

static s32 is_player_on_bike(void) {
    Object *bike = validate_bike();

    Object *player = objGetPlayer();
    Player_Data *playerdata = (Player_Data*)player->data;

    return bike != NULL && playerdata->vehicle == bike;
}

static void destroy_bike(void) {
    if (is_player_on_bike()) return;
    validate_bike();

    if (bike != NULL) {
        objFreeObject(bike);
        bike = NULL;
    }
}

static void spawn_bike(void) {
    if (is_player_on_bike()) return;
    destroy_bike();

    Object *player = objGetPlayer();
    
    SnowBike_Setup *setup = objAllocSetup(sizeof(SnowBike_Setup), OBJ_CRSnowBike);
    setup->base.loadFlags = OBJSETUP_LOAD_MANUAL;
    setup->base.fadeFlags = OBJSETUP_FADE_CAMERA;
    setup->base.loadDistance = 1;
    setup->base.fadeDistance = 99;
    setup->base.x = player->globalPosition.x;
    setup->base.y = player->globalPosition.y;
    setup->base.z = player->globalPosition.z;
    setup->yaw = 0xB8;
    setup->isSharpClawBike = FALSE;
    setup->gamebitUnlocked = -1;
    setup->gamebitFinished = -1;

    bike = objSetupObject(&setup->base, OBJINIT_STANDALONE | OBJINIT_FLAG4, -1, 0, NULL);
}

static void spawn_dockpoint(void) {
    Object *player = objGetPlayer();

    DFDockpoint_Setup* dock = objAllocSetup(sizeof(DFDockpoint_Setup), OBJ_DFdockpoint);
    dock->base.quarterSize = 9;
    dock->base.loadFlags = OBJSETUP_LOAD_MAIN;
    dock->base.loadDistance = 50;
    dock->base.fadeFlags = OBJSETUP_FADE_MAIN;
    dock->base.fadeDistance = 45;
    dock->base.x = player->globalPosition.x;
    dock->base.y = player->globalPosition.y;
    dock->base.z = player->globalPosition.z;
    dock->yaw = player->srt.yaw;
    dock->type = 1;
    dock->range = 500;
    objSetupObject(&dock->base, OBJINIT_STANDALONE | OBJINIT_FLAG4, 
        mapWorldXZToMapID(player->globalPosition.x, player->globalPosition.z), -1, NULL);
}

static void spawn_bwlog(void) {
    Object *player = objGetPlayer();

    Generic_Setup* logsetup = objAllocSetup(sizeof(Generic_Setup), OBJ_BWLog);
    logsetup->base.quarterSize = 9;
    logsetup->base.loadFlags = OBJSETUP_LOAD_MAIN;
    logsetup->base.loadDistance = 50;
    logsetup->base.fadeFlags = OBJSETUP_FADE_MAIN;
    logsetup->base.fadeDistance = 45;
    logsetup->base.x = player->globalPosition.x;
    logsetup->base.y = player->globalPosition.y;
    logsetup->base.z = player->globalPosition.z;
    logsetup->yaw = player->srt.yaw;
    objSetupObject(&logsetup->base, OBJINIT_STANDALONE | OBJINIT_FLAG4, 
        mapWorldXZToMapID(player->globalPosition.x, player->globalPosition.z), -1, NULL);

    spawn_dockpoint();
}

static void spawn_dflog(void) {
    Object *player = objGetPlayer();

    Generic_Setup* logsetup = objAllocSetup(sizeof(Generic_Setup), OBJ_DFlog);
    logsetup->base.quarterSize = 9;
    logsetup->base.loadFlags = OBJSETUP_LOAD_MAIN;
    logsetup->base.loadDistance = 50;
    logsetup->base.fadeFlags = OBJSETUP_FADE_MAIN;
    logsetup->base.fadeDistance = 45;
    logsetup->base.x = player->globalPosition.x;
    logsetup->base.y = player->globalPosition.y;
    logsetup->base.z = player->globalPosition.z;
    logsetup->yaw = player->srt.yaw;
    objSetupObject(&logsetup->base, OBJINIT_STANDALONE | OBJINIT_FLAG4, 
        mapWorldXZToMapID(player->globalPosition.x, player->globalPosition.z), -1, NULL);
    
        spawn_dockpoint();
}

RECOMP_HOOK("__dll417_DFlog_vehicle_SetMountState") void dll_417_func_86C_hook(Object *self, s32 a1) {
    if (a1 != 0) {
        objClearMapID(self);
    } else {
        objInferMapID(self);
    }
}

RECOMP_CALLBACK(".", my_cheats_menu_event) void bike_cheats_menu_callback() {
    dbgui_menu_item("Bike", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void bike_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Bike", &windowOpen)) {
            Object *bike = validate_bike();
            if (bike == NULL) {
                if (dbgui_button("Spawn Bike")) {
                    spawn_bike();
                }
            } else {
                if (dbgui_button("Destroy Bike")) {
                    destroy_bike();
                }
            }

            if (dbgui_button("Spawn BWLog")) {
                spawn_bwlog();
            }
            if (dbgui_button("Spawn DFlog")) {
                spawn_dflog();
            }
        }
        dbgui_end();
    }
}

RECOMP_CALLBACK("*", recomp_on_game_tick) void test_game_tick() {
    // if (get_player() != NULL) {
    //     if (joy_get_pressed_raw(0) & U_JPAD) {
    //         if (bike == NULL) {
    //             spawn_bike();
    //         } else {
    //             destroy_bike();
    //         }
    //     }
    // }

    Object *bike = validate_bike();

    if (bike != NULL) {
        void *objdata = bike->data;
        *((s32*)((u32)objdata + 0x3C8)) = 14000;
    }
}
