#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "libc/string.h"
#include "game/gamebits.h"
#include "sys/main.h"
#include "sys/memory.h"
#include "sys/fs.h"
#include "dll.h"

extern BitTableEntry *gFile_BITTABLE;

extern u8 __dll30_sRecentlyCompleted[5];
extern u8 __dll30_sCompletionIdx;
extern s8 __dll30_sRecentlyCompletedNextIdx;

extern s16 __dll30_task_get_completion_percentage(void);

static s32 windowOpen = FALSE;
static s32 loaded = FALSE;

#define MAX_TASKS (32 * 8)

static const char* taskNames[MAX_TASKS];
static const char* taskClues[MAX_TASKS];
static s32 taskGamebits[MAX_TASKS];
static s32 numTasks = 0;

static void load_tasks(void) {
    if (loaded) return;
    loaded = TRUE;

    for (s32 i = 0; i < MAX_TASKS; i++) {
        GameTextChunk* textGroup = gDLL_21_Gametext->vtbl->get_chunk(244 + i);
        
        // Move strings out of the main memory pools to avoid wasting space
        s32 nameLen = strlen(textGroup->strings[0]);
        s32 clueLen = strlen(textGroup->strings[1]);
        char* name = recomp_alloc(nameLen + 1);
        char* clue = recomp_alloc(clueLen + 1);
        bcopy(textGroup->strings[0], name, nameLen);
        bcopy(textGroup->strings[1], clue, clueLen);
        name[nameLen] = '\0';
        clue[clueLen] = '\0';
        mmFree(textGroup);

        taskNames[i] = name;
        taskClues[i] = clue;
        taskGamebits[i] = -1;
    }

    s32 numGamebits = get_file_size(BITTABLE_BIN) / sizeof(BitTableEntry);
    for (s32 i = 0; i < numGamebits; i++) {
        BitTableEntry* entry = &gFile_BITTABLE[i];
        if (entry->field_0x2 & (1 << 5)) {
            if (taskGamebits[entry->task] != -1) {
                recomp_eprintf("[debug] WARN: Task %d is mapped two by more than one gamebit 0x%X, 0x%X\n", 
                    entry->task, taskGamebits[entry->task], i);
            }
            taskGamebits[entry->task] = i;
        }
    }

    s32 prevNextTask = __dll30_sCompletionIdx;
    for (__dll30_sCompletionIdx = 0; __dll30_sCompletionIdx < 255; __dll30_sCompletionIdx++) {
        if (__dll30_task_get_completion_percentage() == 100) {
            break;
        }
    }
    numTasks = __dll30_sCompletionIdx;
    __dll30_sCompletionIdx = prevNextTask;
}

static void task_list(void) {
    if (dbgui_begin_child("tasks")) {
        for (s32 i = 0; i < numTasks; i++) {
            // Each task bitstring value is 32-bit
            s32 bsEntry = i / 32;
            s32 bit = i % 32;

            u32 bsValue = main_get_bits(BIT_Task_Bits_1 + bsEntry);
            s32 value = (bsValue >> bit) & 1;

            s32 pct = (s32)(((i + 1) / (f32)numTasks) * 100.0f);

            if (value) {
                dbgui_begin_disabled(TRUE);
            }

            dbgui_textf("%s %d: (%d%%) [0x%X] %s", value ? "[X]" : "[ ]", i, pct, taskGamebits[i], taskNames[i]);

            if (value) {
                dbgui_end_disabled();
            }
        }
    }
    dbgui_end_child();
}

static void clue_list(void) {
    if (dbgui_begin_child("clues")) {
        for (s32 i = 0; i < numTasks; i++) {
            if (dbgui_tree_node(recomp_sprintf_helper("%d: %s###%d", i, taskClues[i], i)))  {
                dbgui_textf("%s", taskNames[i]);
                dbgui_tree_pop();
            }
        }
    }
    dbgui_end_child();
}

static void recent_list(void) {
    if (dbgui_begin_child("recent")) {
        for (s32 i = 0; i < (__dll30_sRecentlyCompletedNextIdx + 1); i++) {
            s32 task = __dll30_sRecentlyCompleted[i];
            dbgui_textf("%d: %s", task, taskNames[task]);
        }
    }
    dbgui_end_child();
}

void task_debug_menu_callback(void) {
    dbgui_menu_item("Tasks", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void task_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (!loaded) {
            load_tasks();
        }

        if (dbgui_begin("Task Debug", &windowOpen)) {
            dbgui_textf("Next task: %d/%d | Completion: %d%%", __dll30_sCompletionIdx, numTasks, gDLL_30_Task->vtbl->get_completion_percentage());

            if (dbgui_begin_tab_bar("tabs")) {
                if (dbgui_begin_tab_item("Task List", NULL)) {
                    task_list();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Clues", NULL)) {
                    clue_list();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Recently Completed", NULL)) {
                    recent_list();
                    dbgui_end_tab_item();
                }
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}
