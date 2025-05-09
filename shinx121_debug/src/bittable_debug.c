#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "sys/main.h"

extern BitTableEntry *gFile_BITTABLE;
extern s16 gSizeBittable;

s32 bittableDebugWindowOpen = FALSE;
s32 entry = 0;

RECOMP_CALLBACK(".", my_debug_menu_event) void bittable_debug_menu_callback() {
    dbgui_menu_item("Bit Table", &bittableDebugWindowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void bittable_debug_dbgui_callback() {
    if (bittableDebugWindowOpen) {
        if (dbgui_begin("Bit Table Debug", &bittableDebugWindowOpen)) {
            dbgui_input_int("Entry", &entry);
            
            s32 value = (s32)get_gplay_bitstring(entry);
            if (dbgui_input_int("Value", &value)) {
                set_gplay_bitstring(entry, value);
            }
            if (dbgui_button("Decrement")) {
                decrement_gplay_bitstring(entry);
            }
            dbgui_same_line();
            if (dbgui_button("Increment")) {
                increment_gplay_bitstring(entry);
            }

            dbgui_text("");
            if (entry >= 0 && entry < gSizeBittable) {
                BitTableEntry *entryInfo = &gFile_BITTABLE[entry];
                dbgui_textf("Start: %d", entryInfo->start);
                dbgui_textf("Length: %d", (entryInfo->field_0x2 & 0x1f) + 1);
                dbgui_textf("Bitstring number: %d", entryInfo->field_0x2 >> 6);
                dbgui_textf("Is task: %d", (entryInfo->field_0x2 & (1 << 5)) ? 1 : 0);
                dbgui_textf("Task: %d", entryInfo->task);
            } else {
                dbgui_text("Entry out of bounds.");
            }
        }
        dbgui_end();
    }
}
