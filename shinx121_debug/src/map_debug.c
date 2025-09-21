#include "modding.h"
#include "dbgui.h"
#include "common.h"
#include "sys/fs.h"
#include "sys/memory.h"

extern void *gLoadedMapsDataTable[];

static s32 map_debug_window_open = FALSE;
static char mapNames[120][28];
static s32 loadedMapNames = FALSE;

RECOMP_CALLBACK(".", my_debug_menu_event) void map_debug_menu_callback() {
    dbgui_menu_item("Maps", &map_debug_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void map_debug_dbgui_callback() {
    if (map_debug_window_open) {
        if (!loadedMapNames) {
            void * mapInfo = read_alloc_file(MAPINFO_BIN, 0);
            for (s32 i = 0; i < 98; i++) {
                bcopy((void*)((u32)mapInfo + (0x20 * i)), mapNames[i], 28);
            }
            mmFree(mapInfo);

            loadedMapNames = TRUE;
        }

        if (dbgui_begin("Map Debug", &map_debug_window_open)) {
            dbgui_text("Loaded maps:");
            for (s32 i = 0; i < 98; i++) {
                if (gLoadedMapsDataTable[i] != NULL) {
                    dbgui_textf("%d: %s", i, mapNames[i]);
                }
            }
        }
        dbgui_end();
    }
}