#include "PR/ultratypes.h"
#include "dlls/objects/793_BWLog.h"
#include "game/objects/object_id.h"
#include "modding.h"
#include "recomputils.h"
#include "dbgui.h"

#include "PR/os.h"
#include "dlls/objects/210_player.h"
#include "game/objects/object.h"
#include "sys/pi.h"
#include "sys/joypad.h"
#include "sys/memory.h"
#include "sys/map.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "common.h"
#include "dll.h"

typedef struct {
/*00*/ ObjSetup base;
/*18*/ s8 yaw;
/*19*/ s8 spawnLogDisabled;
/*1A*/ s16 range;
} DFdockpoint_Setup;

static s32 windowOpen = 0;
static Object *bike;

static Object *validate_bike(void) {
    s32 objListCount;
    Object **objList = objGetAllOfType(0xB, &objListCount);
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
    
    ObjSetup *setup = objAllocSetup(0x24, OBJ_CRSnowBike);
    setup->loadFlags = 0x10;
    setup->fadeFlags = 4;
    setup->loadDistance = 1;
    setup->fadeDistance = 99;
    setup->x = player->globalPosition.x;
    setup->y = player->globalPosition.y;
    setup->z = player->globalPosition.z;
    *((u32*)((u32)setup + 0x18)) = 0xB800FFFF;
    *((u32*)((u32)setup + 0x1C)) = 0x0001FFFF;
    *((u32*)((u32)setup + 0x20)) = 0x1C000000;

    bike = objSetupObject(setup, OBJINIT_STANDALONE | OBJINIT_FLAG4, -1, 0, NULL);
}

static void spawn_dockpoint(void) {
    Object *player = objGetPlayer();

    DFdockpoint_Setup* dock = objAllocSetup(0x2C, OBJ_DFdockpoint);
    dock->base.quarterSize = 9;
    dock->base.loadFlags = OBJSETUP_LOAD_MAIN;
    dock->base.loadDistance = 50;
    dock->base.fadeFlags = OBJSETUP_FADE_MAIN;
    dock->base.fadeDistance = 45;
    dock->base.x = player->globalPosition.x;
    dock->base.y = player->globalPosition.y;
    dock->base.z = player->globalPosition.z;
    dock->yaw = player->srt.yaw;
    dock->spawnLogDisabled = 1;
    dock->range = 500;
    objSetupObject((ObjSetup*)dock, OBJINIT_STANDALONE | OBJINIT_FLAG4, 
        mapWorldXZToMapID(player->globalPosition.x, player->globalPosition.z), -1, NULL);
}

static void spawn_bwlog(void) {
    Object *player = objGetPlayer();

    BWLog_Setup* logsetup = objAllocSetup(sizeof(BWLog_Setup), OBJ_BWLog);
    logsetup->base.quarterSize = 9;
    logsetup->base.loadFlags = OBJSETUP_LOAD_MAIN;
    logsetup->base.loadDistance = 50;
    logsetup->base.fadeFlags = OBJSETUP_FADE_MAIN;
    logsetup->base.fadeDistance = 45;
    logsetup->base.x = player->globalPosition.x;
    logsetup->base.y = player->globalPosition.y;
    logsetup->base.z = player->globalPosition.z;
    logsetup->yaw = player->srt.yaw;
    objSetupObject((ObjSetup*)logsetup, OBJINIT_STANDALONE | OBJINIT_FLAG4, 
        mapWorldXZToMapID(player->globalPosition.x, player->globalPosition.z), -1, NULL);

    spawn_dockpoint();
}

static void spawn_dflog(void) {
    Object *player = objGetPlayer();

    BWLog_Setup* logsetup = objAllocSetup(sizeof(BWLog_Setup), OBJ_DFlog);
    logsetup->base.quarterSize = 9;
    logsetup->base.loadFlags = OBJSETUP_LOAD_MAIN;
    logsetup->base.loadDistance = 50;
    logsetup->base.fadeFlags = OBJSETUP_FADE_MAIN;
    logsetup->base.fadeDistance = 45;
    logsetup->base.x = player->globalPosition.x;
    logsetup->base.y = player->globalPosition.y;
    logsetup->base.z = player->globalPosition.z;
    logsetup->yaw = player->srt.yaw;
    objSetupObject((ObjSetup*)logsetup, OBJINIT_STANDALONE | OBJINIT_FLAG4, 
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
