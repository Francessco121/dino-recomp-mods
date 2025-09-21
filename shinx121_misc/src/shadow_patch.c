#include "modding.h"
#include "dbgui.h"
#include "common.h"
#include "recomputils.h"

extern s8 D_80092C00; // index of D_800B98A0
extern u32 D_800BB150; // D_800B98A0[D_800B98A0], current position of it (Gfx)
extern s16 D_80092C24; // num of elements in D_800B98A0[D_80092C00]

extern s8 D_80092C04; // index of D_800B98A8
extern u32 D_800BB140; // D_800B98A8[D_80092C04], current position of it (Vec3)
extern s16 D_80092C2C; // num of elements in D_800B98A8[D_80092C04]

extern s8 D_80092C08; // index of D_800B98B0
extern u32 D_800BB148; // D_800B98B0[D_80092C08], current position of it (Vtx)
extern s16 D_80092C34; // num of elements in D_800B98B0[D_80092C08]

extern void *D_800B98A0[2]; // Gfx array
extern void *D_800B98A8[2]; // Vec3 array
extern void *D_800B98B0[2]; // Vtx array

extern s8 D_80092C0C; // index of D_800BB158
extern u32 D_800BB184; // D_800BB158[D_80092C0C], current position of it (Gfx)
extern s16 D_80092C28; // num of elements in D_800BB158[D_80092C0C]

extern s8 D_80092C14; // index of D_800BB168
extern u32 D_800BB174; // D_800BB168[D_80092C14], current position of it (Vec3)
extern s16 D_80092C30; // num of elements in D_800BB168[D_80092C14]

extern s8 D_80092C10; // index of D_800BB160
extern u32 D_800BB17C; // D_800BB160[D_80092C10], current position of it (Vtx)
extern s16 D_80092C38; // num of elements in D_800BB160[D_80092C10]

extern void *D_800BB158[2]; // Gfx array
extern void *D_800BB168[2]; // Vec3 array
extern void *D_800BB160[2]; // Vtx array

#define D_800B98A0_SIZE (sizeof(Gfx) * 500)
#define D_800B98A8_SIZE ((sizeof(Vec3) * 666) + 8) // aligned?
#define D_800B98B0_SIZE (sizeof(Vtx) * 400)

#define RECOMP_D_800B98A0_SIZE (D_800B98A0_SIZE)
#define RECOMP_D_800B98A8_SIZE (D_800B98A8_SIZE)
#define RECOMP_D_800B98B0_SIZE (D_800B98B0_SIZE)

#define D_800BB158_SIZE (sizeof(Gfx) * 600)
#define D_800BB160_SIZE (sizeof(Vtx) * 600)
#define D_800BB168_SIZE ((sizeof(Vec3) * 933) + 4) // aligned?

#define RECOMP_D_800BB158_SIZE (D_800BB158_SIZE)
#define RECOMP_D_800BB160_SIZE (D_800BB160_SIZE)
#define RECOMP_D_800BB168_SIZE (D_800BB168_SIZE)

RECOMP_HOOK_RETURN("func_8004D470") void func_8004D470_hook() {
    mmFree(D_800B98A0[0]);
    D_800B98A0[0] = mmAlloc((RECOMP_D_800B98A0_SIZE * 2) + (RECOMP_D_800B98A8_SIZE * 2) + (RECOMP_D_800B98B0_SIZE * 2), 0x18, NULL);
    D_800B98A0[1] = (u8*)D_800B98A0[0] + RECOMP_D_800B98A0_SIZE; // 4000
    
    D_800B98A8[0] = (u8*)D_800B98A0[1] + RECOMP_D_800B98A0_SIZE; // 8000
    D_800B98A8[1] = (u8*)D_800B98A8[0] + RECOMP_D_800B98A8_SIZE; // 16000
    
    D_800B98B0[0] = (u8*)D_800B98A8[1] + RECOMP_D_800B98A8_SIZE; // 24000
    D_800B98B0[1] = (u8*)D_800B98B0[0] + RECOMP_D_800B98B0_SIZE; // 30400

    mmFree(D_800BB158[0]);
    D_800BB158[0] = mmAlloc((RECOMP_D_800BB158_SIZE * 2) + (RECOMP_D_800BB168_SIZE * 2) + (RECOMP_D_800BB160_SIZE * 2), 0x18, NULL);
    D_800BB158[1] = (u8*)D_800BB158[0] + RECOMP_D_800BB158_SIZE; // 4800
    
    D_800BB168[0] = (u8*)D_800BB158[1] + RECOMP_D_800BB158_SIZE; // 9600
    D_800BB168[1] = (u8*)D_800BB168[0] + RECOMP_D_800BB168_SIZE; // 20800

    D_800BB160[0] = (u8*)D_800BB168[1] + RECOMP_D_800BB168_SIZE; // 32000
    D_800BB160[1] = (u8*)D_800BB160[0] + RECOMP_D_800BB160_SIZE; // 41600

    recomp_printf("realloc'd shadow buffers\n");
}

static s32 shadows_debug_window_open = FALSE;

RECOMP_CALLBACK(".", my_debug_menu_event) void shadow_debug_menu_callback() {
    dbgui_menu_item("Shadows", &shadows_debug_window_open);
}

static void buffer_text(const char *name, s32 count, s32 capacity) {
    dbgui_textf("%s: %d/%d (%.2f%%)", name, count, capacity, ((f32)count / (f32)capacity) * 100.0f);
}

RECOMP_CALLBACK(".", my_dbgui_event) void shadow_debug_dbgui_callback() {
    if (shadows_debug_window_open) {
        if (dbgui_begin("Shadow Debug", &shadows_debug_window_open)) {
            dbgui_text("Buffers 1:");
            buffer_text("D_800B98A0", D_80092C24, RECOMP_D_800B98A0_SIZE / sizeof(Gfx));
            buffer_text("D_800B98A8", D_80092C2C, RECOMP_D_800B98A8_SIZE / sizeof(Vec3));
            buffer_text("D_800B98B0", D_80092C34, RECOMP_D_800B98B0_SIZE / sizeof(Vtx));
            
            dbgui_new_line();
            dbgui_text("Buffers 2:");
            buffer_text("D_800BB158", D_80092C28, RECOMP_D_800BB158_SIZE / sizeof(Gfx));
            buffer_text("D_800BB168", D_80092C30, RECOMP_D_800BB168_SIZE / sizeof(Vec3));
            buffer_text("D_800BB160", D_80092C38, RECOMP_D_800BB160_SIZE / sizeof(Vtx));
        }
        dbgui_end();
    }
}
