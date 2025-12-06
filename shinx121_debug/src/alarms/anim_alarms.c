#include "modding.h"
#include "recomputils.h"

#include "PR/ultratypes.h"
#include "dlls/engine/3_animation.h"
#include "sys/dll.h"
#include "dll.h"

#include "recomp/dlls/engine/3_ANIM_recomp.h"

typedef enum {
/*00*/    ANIMCURVES_CHANNEL_headRotateZ = 0,
/*01*/    ANIMCURVES_CHANNEL_headRotateX = 1,
/*02*/    ANIMCURVES_CHANNEL_headRotateY = 2,
/*03*/    ANIMCURVES_CHANNEL_opacity = 3,
/*04*/    ANIMCURVES_CHANNEL_dayTime = 4,
/*05*/    ANIMCURVES_CHANNEL_scale = 5,
/*06*/    ANIMCURVES_CHANNEL_rotateZ = 6,
/*07*/    ANIMCURVES_CHANNEL_rotateY = 7,
/*08*/    ANIMCURVES_CHANNEL_rotateX = 8,
/*09*/    ANIMCURVES_CHANNEL_animSpeed = 9,
/*0A*/    ANIMCURVES_CHANNEL_animBlendSpeed = 10,
/*0B*/    ANIMCURVES_CHANNEL_translateZ = 11,
/*0C*/    ANIMCURVES_CHANNEL_translateY = 12,
/*0D*/    ANIMCURVES_CHANNEL_translateX = 13,
/*0E*/    ANIMCURVES_CHANNEL_fieldOfView = 14,
/*0F*/    ANIMCURVES_CHANNEL_eyeX = 15,
/*10*/    ANIMCURVES_CHANNEL_eyeY = 16,
/*11*/    ANIMCURVES_CHANNEL_jaw = 17,
/*12*/    ANIMCURVES_CHANNEL_soundVolume = 18
} AnimCurvesKeyframeChannels;

typedef enum {
/*FF*/    ANIMCURVES_EVENTS_setDuration = -1,
/*00*/    ANIMCURVES_EVENTS_timing = 0,
/*01*/    ANIMCURVES_EVENTS_unk1 = 1,
/*02*/    ANIMCURVES_EVENTS_playAnimation = 2,
/*03*/    ANIMCURVES_EVENTS_setObj = 3,
/*04*/    ANIMCURVES_EVENTS_blendShape = 4,
/*05*/    ANIMCURVES_EVENTS_unk5 = 5,
/*06*/    ANIMCURVES_EVENTS_sound = 6,
/*07*/    ANIMCURVES_EVENTS_unk7 = 7,
/*08*/    ANIMCURVES_EVENTS_unk8 = 8,
/*09*/    ANIMCURVES_EVENTS_unk9 = 9,
/*0A*/    ANIMCURVES_EVENTS_unkA = 10,
/*0B*/    ANIMCURVES_EVENTS_subEvent = 11,
/*0C*/    ANIMCURVES_EVENTS_unkC = 12,
/*0D*/    ANIMCURVES_EVENTS_setObjParam = 13,
/*0E*/    ANIMCURVES_EVENTS_setLoop = 14,
/*0F*/    ANIMCURVES_EVENTS_soundOther = 15,
/*7F*/    ANIMCURVES_EVENTS_stopSoundOther = 127
} AnimCurvesEvents;

typedef struct {
Vec3f unk0; 
s8 unkC;
} CameraFunc15Unk_unk74;


typedef struct {
    s32 unk0;
    s32 unk4;
} AnimBSS0;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
} SequenceBoneStructUnk;

typedef struct {
    Object *unk0;
    s32 unk4;
} ANIMBSSUnk0;

typedef struct{
    Object* object;
    Object* overrideObject;
} ANIMActorOverride;

typedef struct {
    u16 unk0;
    u16 unk2;
    u16 totalActors; //maybe?
} ANIMUnk698;

/*0x0*/ extern ANIMBSSUnk0 _bss_0[4];
/*0x20*/ extern s8 _bss_20; //count of items in bss0?
/*0x24*/ extern u32 _bss_24;
/*0x28*/ extern u8 _bss_28[0x4];
/*0x2C*/ extern u8 _bss_2C[0x4];
/*0x30*/ extern u8 _bss_30[0x2];
/*0x32*/ extern u8 _bss_32[0x1];
/*0x33*/ extern u8 _bss_33[0x1];
/*0x34*/ extern u8 _bss_34[0x4];
/*0x38*/ extern u8 _bss_38[0x8];
/*0x40*/ extern u8 _bss_40[0x48];
/*0x88*/ extern u8 _bss_88[0x1];
/*0x89*/ extern u8 _bss_89[0x1];
/*0x8A*/ extern u8 _bss_8A[0x1];
/*0x8B*/ extern s8 _bss_8B;
/*0x8C*/ extern s32 _bss_8C;
/*0x90*/ extern s32 _bss_90;
/*0x94*/ extern s32 _bss_94;
/*0x98*/ extern s32 _bss_98;
/*0x9C*/ extern u8 _bss_9C[0x4];
/*0xA0*/ extern f32 _bss_A0;
/*0xA4*/ extern s8 _bss_A4[0x4];
/*0xA8*/ extern s8 _bss_A8[ANIMCURVES_SCENES_MAX];
/*0xD8*/ extern s8 _bss_D8[ANIMCURVES_SCENES_MAX];
/*0x108*/ extern s8 _bss_108[ANIMCURVES_SCENES_MAX];
/*0x138*/ extern s8 _bss_138[ANIMCURVES_SCENES_MAX];
/*0x168*/ extern s8 _bss_168[ANIMCURVES_SCENES_MAX];
/*0x198*/ extern s8 _bss_198[ANIMCURVES_SCENES_MAX];
/*0x1C8*/ extern s8 _bss_1C8[ANIMCURVES_SCENES_MAX];
/*0x1F8*/ extern s16 _bss_1F8[ANIMCURVES_SCENES_MAX];
/*0x258*/ extern s16 _bss_258[ANIMCURVES_SCENES_MAX];
/*0x2B8*/ extern u8 _bss_2B8[0x8];
/*0x2C0*/ extern u8 _bss_2C0[0x58];
/*0x318*/ extern u16 _bss_318[0x4];
/*0x320*/ extern u8 _bss_320[0x58];
/*0x378*/ extern s8 _bss_378[ANIMCURVES_SCENES_MAX];
/*0x3A8*/ extern u8 _bss_3A8[0x8];
/*0x3B0*/ extern u8 _bss_3B0[0x28];
/*0x3D8*/ extern s32 _bss_3D8[ANIMCURVES_SCENES_MAX];
/*0x490*/ extern s8 _bss_490[ANIMCURVES_SCENES_MAX];
/*0x4C0*/ extern u8 _bss_4C0[0x30];
/*0x4F0*/ extern u8 _bss_4F0[0xb4];
/*0x5A4*/ extern u8 _bss_5A4[0x4];
/*0x5A8*/ extern u8 _bss_5A8[0x4];
/*0x5AC*/ extern u8 _bss_5AC[0x4];
/*0x5B0*/ extern u8 _bss_5B0[0x8];
/*0x5B8*/ extern Vec3f _bss_5B8;
/*0x5C4*/ extern u8 _bss_5C4[0x4];
/*0x5C8*/ extern u8 _bss_5C8[0x4];
/*0x5CC*/ extern u8 _bss_5CC[0x4];
/*0x5D0*/ extern u8 _bss_5D0[0x4];
/*0x5D4*/ extern u8 _bss_5D4[0x4];
/*0x5D8*/ extern void* _bss_5D8; //sequence file buffer
/*0x5DC*/ extern u8 _bss_5DC[0x4];
/*0x5E0*/ extern u8 _bss_5E0[0x4];
/*0x5E4*/ extern u8 _bss_5E4[0x4];
/*0x5E8*/ extern u8 _bss_5E8[0x8];
/*0x5F0*/ extern u8 _bss_5F0[0xa0];
/*0x690*/ extern u8 _bss_690[0x8];
/*0x698*/ extern ANIMUnk698 _bss_698[1];
/*0x69E*/ extern u8 _bss_69E[0x2];
/*0x6A0*/ extern u8 _bss_6A0[0x58];
/*0x6F8*/ extern s8 _bss_6F8;
/*0x6FC*/ extern s32 _bss_6FC;
/*0x700*/ extern s16 _bss_700[2];
/*0x708*/ extern ANIMActorOverride _bss_708[44][16];
/*0x1D0C*/ extern u8 _bss_1D0C[0x7C];
/*0x1D88*/ extern u8 _bss_1D88[8];

static void *hijack_dll_export(DLLFile *dll, s32 exportIdx, void *hijack) {
    u32 *vtbl = DLL_FILE_TO_EXPORTS(dll);

    // +1 to get skip the initial null pointer. Export indices do not include the null at the start of the vtable.
    void **exportPtr = (void**)&vtbl[exportIdx + 1];
    void *original = *exportPtr;
    *exportPtr = hijack;

    return original;
}

typedef s32 (*Export17)(s32 objectSeqIndex, Object* object, s32 arg2);
static Export17 export17_func; 
static s32 export17_hijack(s32 objectSeqIndex, Object* object, s32 arg2);

// Note: Do a return hook so we have a higher chance of being last.
// Do this weirdly cause a normal hook won't catch the hijack by dinomod.
static DLLFile *recomp_sDll;
RECOMP_HOOK_DLL(dll_3_ctor) void anim_ctor_hook(DLLFile *dll) {
    recomp_sDll = dll;
}
RECOMP_HOOK_RETURN_DLL(dll_3_ctor) void anim_ctor_ret_hook() {
    export17_func = hijack_dll_export(recomp_sDll, 17, export17_hijack);
}

RECOMP_HOOK_RETURN_DLL(dll_3_dtor) void anim_dtor_hook() {
    export17_func = NULL;
}

s32 export17_hijack(s32 objectSeqIndex, Object* object, s32 arg2) {
    s32 ret = export17_func(objectSeqIndex, object, arg2);

    if (ret == -1) {
        s32 freeSlots = 0;
        for (s32 i = 25; i < 45; i++) {
            if (_bss_168[i] == 0) {
                freeSlots++;
            }
        }

        s32 seq = -1;
        s32 invalidObj = FALSE;
        s32 oobSeqIdx = FALSE;
        s32 objUID = -1;
        if (object != NULL && object->def != NULL) {
            if (objectSeqIndex >= 0 && 
                objectSeqIndex < object->def->numSequences &&
                object->def->pSeq != NULL) {
                seq = object->def->pSeq[objectSeqIndex];
            } else {
                oobSeqIdx = TRUE;
            }

            if (object->setup != NULL) {
                objUID = object->setup->uID;
            }
        } else {
            invalidObj = TRUE;
        }

        if (invalidObj) {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence. Object pointer is invalid.\n  ANIM->export17(%d, %p, %d)\n",
                objectSeqIndex, object, arg2);
        } else if (oobSeqIdx) {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence. Object sequence index out of bounds (object has %d sequences).\n  ANIM->export17(%d, %p, %d)\n  Object: %s (ID 0x%X, UID 0x%X)\n",
                object->def->numSequences,
                objectSeqIndex, object, arg2,
                object->def->name, object->id, objUID);
        } else if (freeSlots == 0) {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence. No sequence slots available.\n  ANIM->export17(%d, %p, %d)\n  Seq: 0x%X\n  Object: %s (ID 0x%X, UID 0x%X)\n",
                objectSeqIndex, object, arg2,
                seq,
                object->def->name, object->id, objUID);
        } else {
            recomp_eprintf(
                "[ANIM] Failed to start obj sequence.\n  ANIM->export17(%d, %p, %d)\n  Seq: 0x%X\n  Object: %s (ID 0x%X, UID 0x%X)\n",
                objectSeqIndex, object, arg2,
                seq,
                object->def->name, object->id, objUID);
        }
    }

    return ret;
}
