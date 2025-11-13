#include "modding.h"
#include "dbgui.h"

#include "PR/ultratypes.h"
#include "sys/newshadows.h"

static s32 windowOpen = FALSE;

static u32 D_800B98A0_capacity;
static u32 D_800B98A8_capacity;
static u32 D_800B98B0_capacity;

static u32 D_800BB158_capacity;
static u32 D_800BB160_capacity;
static u32 D_800BB168_capacity;

static u32 D_800B98A0_count;
static u32 D_800B98A8_count;
static u32 D_800B98B0_count;

static u32 D_800BB158_count;
static u32 D_800BB160_count;
static u32 D_800BB168_count;

RECOMP_CALLBACK(".", my_debug_menu_event) void shadow_debug_menu_callback() {
    dbgui_menu_item("Shadow", &windowOpen);
}

static void buffer_text(const char *name, u32 count, u32 capacity) {
    dbgui_textf("%s: %d/%d (%.2f%%)", name, count, capacity, ((f32)count / (f32)capacity) * 100.0f);
}

RECOMP_CALLBACK(".", my_dbgui_event) void shadow_debug_dbgui_callback() {
    if (windowOpen) {
        if (dbgui_begin("Shadow Debug", &windowOpen)) {
            dbgui_text("Buffers 1:");
            buffer_text("D_800B98A0", D_800B98A0_count, D_800B98A0_capacity);
            buffer_text("D_800B98A8", D_800B98A8_count, D_800B98A8_capacity);
            buffer_text("D_800B98B0", D_800B98B0_count, D_800B98B0_capacity);
            
            dbgui_new_line();
            dbgui_text("Buffers 2:");
            buffer_text("D_800BB158", D_800BB158_count, D_800BB158_capacity);
            buffer_text("D_800BB168", D_800BB168_count, D_800BB168_capacity);
            buffer_text("D_800BB160", D_800BB160_count, D_800BB160_capacity);
        }
        dbgui_end();
    }
}

RECOMP_HOOK("func_8004D9B8") void func_8004D9B8_hook(void) {
    D_800B98A0_capacity = (u32)D_800B98A0[1] - (u32)D_800B98A0[0];
    D_800B98A8_capacity = (u32)D_800B98A8[1] - (u32)D_800B98A8[0];
    D_800B98B0_capacity = (u32)D_800B98B0[1] - (u32)D_800B98B0[0];

    D_800B98A0_count = (u32)D_800BB150 - (u32)D_800B98A0[D_80092C00];
    D_800B98A8_count = (u32)D_800BB140 - (u32)D_800B98A8[D_80092C04];
    D_800B98B0_count = (u32)D_800BB148 - (u32)D_800B98B0[D_80092C08];
}

RECOMP_HOOK("func_8004DABC") void func_8004DABC_hook(void) {
    D_800BB158_capacity = (s32)D_800BB158[1] - (s32)D_800BB158[0];
    D_800BB160_capacity = (s32)D_800BB160[1] - (s32)D_800BB160[0];
    D_800BB168_capacity = (s32)D_800BB168[1] - (s32)D_800BB168[0];

    D_800BB158_count = (u32)D_800BB184 - (u32)D_800BB158[D_80092C0C];
    D_800BB160_count = (u32)D_800BB17C - (u32)D_800BB160[D_80092C10];
    D_800BB168_count = (u32)D_800BB174 - (u32)D_800BB168[D_80092C14];
}
