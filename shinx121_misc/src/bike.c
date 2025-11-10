#include "PR/ultratypes.h"
#include "modding.h"
#include "recomputils.h"
#include "dbgui.h"

#include "PR/os.h"
#include "dlls/objects/210_player.h"
#include "game/objects/object.h"
#include "sys/fs.h"
#include "sys/joypad.h"
#include "sys/memory.h"
#include "sys/map.h"
#include "sys/objects.h"
#include "common.h"
#include "functions.h"
#include "dll.h"

static s32 windowOpen = 0;
static Object *bike;

static void destroy_bike(void) {
    if (bike != NULL) {
        obj_destroy_object(bike);
        bike = NULL;
    }
}

static void spawn_bike(void) {
    destroy_bike();

    Object *player = get_player();
    
    ObjSetup *setup = obj_alloc_create_info(0x24, OBJ_CRSnowBike);
    setup->loadFlags = 0x10;
    setup->fadeFlags = 4;
    setup->loadDistance = 1;
    setup->fadeDistance = 99;
    setup->x = player->srt.transl.x;
    setup->y = player->srt.transl.y;
    setup->z = player->srt.transl.z;
    *((u32*)((u32)setup + 0x18)) = 0xB800FFFF;
    *((u32*)((u32)setup + 0x1C)) = 0x0001FFFF;
    *((u32*)((u32)setup + 0x20)) = 0x1C000000;

    bike = obj_create(setup, OBJ_INIT_FLAG1, -1, 0, NULL);
}

RECOMP_CALLBACK(".", my_cheats_menu_event) void bike_cheats_menu_callback() {
    dbgui_menu_item("Bike", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void bike_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Bike", &windowOpen)) {
            if (dbgui_button("Spawn Bike")) {
                spawn_bike();
            }

            if (dbgui_button("Destroy Bike")) {
                destroy_bike();
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

    if (bike != NULL) {
        void *objdata = bike->data;
        *((s32*)((u32)objdata + 0x3C8)) = 10000;
    }
}
