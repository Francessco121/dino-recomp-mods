#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_menus.h"

#include "sys/main.h"
#include "sys/pi.h"

extern BitTableEntry *gFile_BITTABLE;

static s32 bittableDebugWindowOpen = FALSE;
static s32 entry = 0;
static s32 bittableSize = -1;

void bittable_debug_menu_callback(void) {
    dbgui_menu_item("Bit Table", &bittableDebugWindowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void bittable_debug_dbgui_callback(void) {
    if (bittableDebugWindowOpen) {
        if (bittableSize == -1) {
            bittableSize = piRomGetFileSize(BITTABLE_BIN) / sizeof(BitTableEntry);
        }

        if (dbgui_begin("Bit Table Debug", &bittableDebugWindowOpen)) {
            const static DbgUiInputIntOptions inputOptions = {
                .step = 1,
                .stepFast = 100,
                .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
            };
            dbgui_input_int_ext("Entry", &entry, &inputOptions);
            
            s32 value = (s32)mainGetBits(entry);
            if (dbgui_input_int("Value", &value)) {
                mainSetBits(entry, value);
            }
            if (dbgui_button("Decrement")) {
                mainDecrementBits(entry);
            }
            dbgui_same_line();
            if (dbgui_button("Increment")) {
                mainIncrementBits(entry);
            }

            dbgui_new_line();
            if (entry >= 0 && entry < bittableSize) {
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
