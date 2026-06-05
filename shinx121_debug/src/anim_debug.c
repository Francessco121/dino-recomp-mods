#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "objects/object_debug.h"
#include "debug_menus.h"

#include "PR/ultratypes.h"
#include "sys/gfx/animseq.h"
#include "dlls/engine/3_animation.h"
#include "dll.h"

#include "recomp/dlls/engine/3_ANIM_recomp.h"

// Maximum number of active object sequences
#define MAX_SEQSLOTS 45
// Maximum number of actors in an object sequence
#define MAX_ACTORS 16

typedef struct {
    Object *obj;
    s32 preemptTime;
} PreemptTime;

typedef struct{
    Object* object;
    Object* overrideObject;
} ANIMActorOverride;

typedef struct {
    s16 seqSlot;
    s16 startTime;
    u16 numActors;
} Activate;

typedef struct {
    Object* actor;
    s16 value;
    s8 type;
} QueuedEnvFx;

typedef struct {
    s32* events; // pointer to list of code events
    s16 numEvents;
    s16 time; // timestamp of code event
} CodeEventList;

/*0x0*/ extern PreemptTime sPreemptTimeList[4];
/*0x20*/ extern s8 sPreemptTimeListCount;
/*0x24*/ extern f32 _bss_24;
/*0x28*/ extern f32 _bss_28;
/*0x2C*/ extern f32 _bss_2C;
/*0x30*/ extern s16 _bss_30;
/*0x32*/ extern s8 _bss_32;
/*0x33*/ extern s8 _bss_33;
/*0x38*/ extern QueuedEnvFx sEnvFxQueue[10];
/*0x88*/ extern s8 sEnvFxQueueCount;
/*0x89*/ extern s8 _bss_89;
/*0x8A*/ extern s8 _bss_8A;
/*0x8B*/ extern s8 _bss_8B;
/*0x8C*/ extern s32 sCameraModule;
/*0x90*/ extern s32 _bss_90;
/*0x94*/ extern s32 _bss_94;
/*0x98*/ extern s32 _bss_98;
/*0x9C*/ extern s16 sPendingWarpID;
/*0xA0*/ extern f32 _bss_A0;
/*0xA4*/ extern s8 sSeqEnded;
/*0xA8*/ extern s8 _bss_A8[MAX_SEQSLOTS];
/*0xD8*/ extern s8 _bss_D8[MAX_SEQSLOTS];
/*0x108*/ extern s8 _bss_108[MAX_SEQSLOTS];
/*0x138*/ extern s8 sEventFlags[MAX_SEQSLOTS];
/*0x168*/ extern s8 sSlotInUse[MAX_SEQSLOTS];
/*0x198*/ extern s8 _bss_198[MAX_SEQSLOTS];
/*0x1C8*/ extern s8 _bss_1C8[MAX_SEQSLOTS];
/*0x1F8*/ extern s16 _bss_1F8[MAX_SEQSLOTS];
/*0x258*/ extern s16 _bss_258[MAX_SEQSLOTS];
/*0x2B8*/ extern s16 _bss_2B8[MAX_SEQSLOTS];
/*0x318*/ extern s16 _bss_318[MAX_SEQSLOTS];
/*0x378*/ extern u8 _bss_378[MAX_SEQSLOTS];
/*0x3A8*/ extern u8 _bss_3A8[MAX_SEQSLOTS];
/*0x3D8*/ extern s32 sSlotObjID[MAX_SEQSLOTS];
/*0x490*/ extern u8 _bss_490[MAX_SEQSLOTS];
/*0x4C0*/ extern u8 _bss_4C0[MAX_SEQSLOTS];
/*0x4F0*/ extern f32 _bss_4F0[MAX_SEQSLOTS];
/*0x5A4*/ extern f32 _bss_5A4;
/*0x5A8*/ extern f32 _bss_5A8;
/*0x5AC*/ extern u8 _bss_5AC;
/*0x5B0*/ extern f32 _bss_5B0;
/*0x5B8*/ extern Vec3f _bss_5B8;
/*0x5C4*/ extern f32 _bss_5C4;
/*0x5C8*/ extern s32 _bss_5C8;
/*0x5CC*/ extern s8 sProcessedAnimCallback;
/*0x5D0*/ extern s32 _bss_5D0;
/*0x5D4*/ extern s32 _bss_5D4;
/*0x5D8*/ extern void* sTempBuffer; //sequence file buffer
/*0x5DC*/ extern f32 _bss_5DC;
/*0x5E0*/ extern f32 _bss_5E0;
/*0x5E4*/ extern f32 _bss_5E4;
/*0x5E8*/ extern f32 _bss_5E8;
/*0x5F0*/ extern CodeEventList sCodeEvtQueue[20];
/*0x690*/ extern s32 sCodeEvtQueueCount;
/*0x698*/ extern Activate sActivates[16];
/*0x6F8*/ extern s8 sActivatesCount;
/*0x6FC*/ extern Object *_bss_6FC; // camera animobj (AnimCamera)?
/*0x700*/ extern s16 _bss_700;
/*0x708*/ extern ANIMActorOverride sOverrides[45][16];
typedef struct {
    u32 unk0_8: 1;
    u32 unk0_1: 31; // unused
} UnkBss1D88;
/*0x1D88*/ static UnkBss1D88 _bss_1D88;

static s32 windowOpen = FALSE;

void anim_debug_menu_callback(void) {
    dbgui_menu_item("Anim", &windowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void anim_debug_dbgui_callback(void) {
    if (windowOpen) {
        if (dbgui_begin("Anim Debug", &windowOpen)) {
            dbgui_textf("sPreemptTimeListCount: %d", sPreemptTimeListCount);
            dbgui_textf("_bss_24: %d", _bss_24);
            dbgui_textf("sCameraModule: %d", sCameraModule);
            dbgui_textf("_bss_8B: %d", _bss_8B);
            dbgui_textf("_bss_A0: %f", _bss_A0);
            dbgui_textf("_bss_6FC: %d", _bss_6FC);
            dbgui_textf("sActivatesCount: %d", sActivatesCount);

            if (dbgui_tree_node("_bss_108")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_108[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("sEventFlags")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, sEventFlags[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("sSlotInUse")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, sSlotInUse[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_A8")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_A8[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_D8")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_D8[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_198")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_198[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_1C8")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_1C8[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_1F8")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_1F8[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_258")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_258[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_378")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_378[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("sSlotObjID")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, sSlotObjID[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("_bss_490")) {
                for (s32 i = 0; i < MAX_SEQSLOTS; i++) dbgui_textf("[%d] %d", i, _bss_490[i]);
                dbgui_tree_pop();
            }
            if (dbgui_tree_node("sOverrides")) {
                for (s32 i = 0; i < 44; i++) {
                    ANIMActorOverride* actors = sOverrides[i];
                    if (dbgui_tree_node(recomp_sprintf_helper("%d", i))) {
                        dbgui_textf("object: %p", actors[i].object);
                        dbgui_textf("overrideObject: %p", actors[i].overrideObject);
                        dbgui_tree_pop();
                    }
                }
                dbgui_tree_pop();
            }
        }
        dbgui_end();
    }
}
