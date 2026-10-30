#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_menus.h"

#include "dlls/engine/29_gplay.h"
#include "game/gamebits.h"
#include "sys/main.h"
#include "sys/pi.h"
#include "dll.h"

extern BitTableEntry *gFile_BITTABLE;

static s32 windowOpen = FALSE;
static s32 bittableSize = -1;

static void edit_tab(void) {
    static s32 entry = 0;

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

static u32 read_bit(GameState* state, s32 entry) {
    u8 *bitString;
    u32 value;
    s32 idx;
    s32 mask;
    s32 endBit;
    s32 startBit;

    if (entry == BIT_ALWAYS_1) {
        return 1;
    } else if (entry == BIT_ALWAYS_0) {
        return 0;
    } else if (entry == -1) {
        return 0;
    } else if (entry < 0 || entry >= bittableSize) {
        return 0;
    } else {
        switch (gFile_BITTABLE[entry].field_0x2 >> 6) {
            case 0: // Never saved to savegame
                bitString = &state->bitString[0];
                break;
            case 1: // Saved with savepoints
                bitString = &state->save.main.bitString[0];
                break;
            case 2: // Always saved
                bitString = &state->save.file.bitString[0];
                break;
            case 3: // Saved with savepoints that include a map save
                bitString = &state->save.map.bitString[0];
                break;
        }

        startBit = gFile_BITTABLE[entry].start;
        endBit = (gFile_BITTABLE[entry].field_0x2 & 0x1f) + 1;
        value = 0;
        mask = 1;

        for (idx = startBit; idx < (startBit + endBit); idx++) {
            // A clever way to read from bitString bit by bit
            if ((*(u8 *)((u32)bitString + (idx >> 3)) & (1 << (idx & 7))) != 0) {
                value |= mask;
            }

            mask = mask << 1;
        }
    }

    return value;
}

static GameState* stateSnapshots[2] = {NULL, NULL};

static void free_snapshot(s32 idx) {
    if (stateSnapshots[idx] != NULL) {
        recomp_free(stateSnapshots[idx]);
        stateSnapshots[idx] = NULL;
    }
}

static void snapshot_game_state(s32 idx) {
    if (stateSnapshots[idx] == NULL) {
        stateSnapshots[idx] = recomp_alloc(sizeof(GameState));
    }

    bcopy(gDLL_29_Gplay->vtbl->get_state(), stateSnapshots[idx], sizeof(GameState));
}

static void watch_tab(void) {
    static s32 enabled = FALSE;
    static s32 paused = FALSE;
    static s32 hexValues = FALSE;
    static s32 typeFilters[4] = {TRUE, TRUE, TRUE, TRUE};
    static s32 showTaskBits = TRUE;
    static s32 showNewdayBits = TRUE;

    if (dbgui_checkbox("Enabled", &enabled)) {
        if (enabled) {
            snapshot_game_state(0);
        } else {
            free_snapshot(0);
            free_snapshot(1);
            paused = FALSE;
        }
    }

    if (!enabled) {
        return;
    }

    dbgui_same_line();
    dbgui_checkbox("Hex Values", &hexValues);

    if (dbgui_button("Reset")) {
        snapshot_game_state(0);

        if (paused) {
            free_snapshot(1);
            paused = FALSE;
        }
    }
    dbgui_same_line();
    if (dbgui_button(paused ? "Resume" : "Pause")) {
        paused = !paused;
        if (paused) {
            snapshot_game_state(1);
        } else {
            free_snapshot(1);
        }
    }

    if (dbgui_tree_node("Filters")) {
        dbgui_text("Bitstrings:");
        for (s32 i = 0; i < 4; i++) {
            if (i > 0) {
                dbgui_same_line();
            }
            dbgui_checkbox(recomp_sprintf_helper("%d", i), &typeFilters[i]);
        }
        dbgui_separator();

        dbgui_checkbox("Task Bits", &showTaskBits);
        dbgui_set_item_tooltip("Bits 0x12F-0x13C");
        dbgui_checkbox("Newday Bits", &showNewdayBits);
        dbgui_set_item_tooltip("Bit 0x2BA");

        dbgui_tree_pop();
    }
    
    dbgui_separator_text("Changes");

    if (stateSnapshots[0] == NULL || (paused && stateSnapshots[1] == NULL)) {
        return;
    }

    GameState* currentState = gDLL_29_Gplay->vtbl->get_state();

    if (dbgui_begin_child("scroll")) {
        for (s32 i = 0; i < bittableSize; i++) {
            s32 bitstring = gFile_BITTABLE[i].field_0x2 >> 6;
            if (bitstring < 0 || bitstring >= 4 || !typeFilters[bitstring]) {
                continue;
            }
            if (bitstring == 0 && gFile_BITTABLE[i].start == 0 && i != 0) {
                // don't track bit 0 aliases
                continue;
            }
            if (!showTaskBits && (i >= BIT_Task_Bits_1 && i <= BIT_Next_Game_Task)) {
                continue;
            }
            if (!showNewdayBits && i == BIT_SC_UNKNOWN_2BA) {
                continue;
            }

            u32 prev = read_bit(stateSnapshots[0], i);
            u32 curr = read_bit(paused ? stateSnapshots[1] : currentState, i);

            if (prev != curr) {
                if (hexValues) {
                    dbgui_textf("[%X] 0x%X -> 0x%X", i, prev, curr);
                } else {
                    dbgui_textf("[%X] %d -> %d", i, prev, curr);
                }
            }
        }
    }
    dbgui_end_child();
}

void bittable_debug_menu_callback(void) {
    dbgui_menu_item("Bit Table", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void bittable_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (bittableSize == -1) {
            bittableSize = piRomGetFileSize(BITTABLE_BIN) / sizeof(BitTableEntry);
        }

        if (dbgui_begin("Bit Table Debug", &windowOpen)) {
            if (dbgui_begin_tab_bar("tabs")) {
                if (dbgui_begin_tab_item("Edit", NULL)) {
                    edit_tab();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Watch", NULL)) {
                    watch_tab();
                    dbgui_end_tab_item();
                }
                
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}
