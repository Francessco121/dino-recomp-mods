#include "game/objects/object_id.h"
#include "modding.h"
#include "recompconfig.h"
#include "common.h"
#include "sys/dll.h"
#include "sys/objects.h"
#include "dll.h"

// #include "recomp/dlls/engine/77_scarab_recomp.h"

// DLL_INTERFACE(DLL_77) {
// /*:*/ DLL_INTERFACE_BASE(DLL);
// /*0*/ s32 (*func0)(void);
// /*1*/ void (*func1)(void);
// /*2*/ void (*func2)(Gfx** gfx, Mtx** mtx, Vtx** vtx);
// };

// extern Gfx *gCurGfx;
// extern Mtx *gCurMtx;
// extern Vertex *gCurVtx;

// static DLL_77 *dll77; 

// RECOMP_CALLBACK("*", recomp_on_game_tick) void test_game_tick() {
//     Object *player = get_player();

//     if (player != NULL) {
//         if (dll77 == NULL) {
//             dll77 = dll_load(77, 3, TRUE);
//         }

//         dll77->vtbl->func2(&gCurGfx, &gCurMtx, &gCurVtx);
//     }
// }

#include "recomp/dlls/objects/210_player_recomp.h"

extern Vec3f _data_3C[];
extern Vec3f _data_7C[];
extern f32 _data_88[];

RECOMP_HOOK_DLL(dll_210_ctor) void player_ctor_hook() {
    // _data_3C[0].x = 50.0f;
    // _data_3C[0].y += 100.0f;
    // _data_3C[1].y += 100.0f;

    //_data_7C[0].x = -10.0f;

    //_data_88[0] = 20.0f;
}

#include "recomp/dlls/engine/27_recomp.h"

extern void dll_27_func_1148(Object* obj, DLL27_Data* data, MtxF* mtx);

// RECOMP_HOOK_DLL(dll_27_func_624) void asdasdasdas(Object* arg0, DLL27_Data* arg1, f32 delayFloat) {
//     if (arg0->id == OBJ_Krystal) {
//         arg1->unk257 = 2;
//     }
// }

// RECOMP_PATCH void dll_27_func_1E8(Object* arg0, DLL27_Data* arg1, f32 arg2) {
//     MtxF sp110;
//     Vec3f spE0[4];
//     f32 spD0[4];
//     f32 temp_fa1;
//     f32 temp_ft4;
//     f32 temp_fa0;
//     f32 temp_fv0;
//     f32 minX;
//     f32 maxY;
//     f32 minY;
//     f32 maxZ;
//     f32 minZ;
//     f32 maxX;
//     s32 var_s4;
//     s32 i;

//     if (arg1->unk257 == 0) {
//         return;
//     }

//     transform_point_by_object(
//         arg0->srt.transl.f[0], arg0->srt.transl.f[1], arg0->srt.transl.f[2], 
//         &arg0->positionMirror.x, &arg0->positionMirror.y, &arg0->positionMirror.z, 
//         arg0->parent);
//     dll_27_func_1148(arg0, arg1, &sp110);

//     for (var_s4 = 0, i = 0; var_s4 < ((s32) arg1->unk258 >> 4); ) {
//         vec3_transform(&sp110, 
//                        arg1->unk4[var_s4].x, arg1->unk4[var_s4].y, arg1->unk4[var_s4].z, 
//                        &spE0[var_s4].x, &spE0[var_s4].y, &spE0[var_s4].z);
//         spD0[i] = arg1->unk68.unk40[i];
//         spD0[i] = sqrtf((2 * spD0[i]) * spD0[i]);
//         i++;
//         var_s4++;
//     }
    
//     maxX = -100000.0f;
//     minX = 100000.0f;
//     maxY = -100000.0f;
//     minY = 100000.0f;
//     maxZ = -100000.0f;
//     minZ = 100000.0f;
//     var_s4 = 0;
//     i = 0;
//     while (i < ((s32) arg1->unk258 >> 4)) {
//         temp_fv0 = spE0[var_s4].x + spD0[i];
//         if (maxX < temp_fv0) {
//             maxX = temp_fv0;
//         }
//         temp_fv0 = spE0[var_s4].x - spD0[i];
//         if (temp_fv0 < minX) {
//             minX = temp_fv0;
//         }
//         temp_fv0 = spE0[var_s4].y + spD0[i];
//         if (maxY < temp_fv0) {
//             maxY = temp_fv0;
//         }
//         temp_fv0 = spE0[var_s4].y - spD0[i];
//         if (temp_fv0 < minY) {
//             minY = temp_fv0;
//         }
//         temp_fv0 = spE0[var_s4].z + spD0[i];
//         if (maxZ < temp_fv0) {
//             maxZ = temp_fv0;
//         }
//         temp_fv0 = spE0[var_s4].z - spD0[i];
//         if (temp_fv0 < minZ) {
//             minZ = temp_fv0;
//         }

//         if (((!arg1->unk38[var_s4].x) && (!arg1->unk38[var_s4].x)) && (!arg1->unk38[var_s4].x)){} // @fake

//         temp_fa1 = arg1->unk38[var_s4].y;
//         temp_ft4 = arg1->unk38[var_s4].z;

//         temp_fv0 = arg1->unk38[var_s4].x + spD0[i];
//         if (maxX < temp_fv0) {
//             maxX = temp_fv0;
//         }
//         temp_fv0 = arg1->unk38[var_s4].x - spD0[i];
//         if (temp_fv0 < minX) {
//             minX = temp_fv0;
//         }
//         temp_fv0 = temp_fa1 + spD0[i];
//         if (maxY < temp_fv0) {
//             maxY = temp_fv0;
//         }
//         temp_fv0 = temp_fa1 - spD0[i];
//         if (temp_fv0 < minY) {
//             minY = temp_fv0;
//         }
//         temp_fv0 = temp_ft4 + spD0[i];
//         if (maxZ < temp_fv0) {
//             maxZ = temp_fv0;
//         }
//         temp_fv0 = temp_ft4 - spD0[i];
//         if (temp_fv0 < minZ) {
//             minZ = temp_fv0;
//         }
//         var_s4++;
//         i++;
        
//     }
//     arg1->unk23C.min.x = minX;
//     arg1->unk23C.max.x = maxX;
//     arg1->unk23C.min.y = (minY - arg1->unk254);
//     arg1->unk23C.max.y = (arg1->unk254 + maxY);
//     arg1->unk23C.min.z = minZ;
//     arg1->unk23C.max.z = maxZ;

//     // if (arg0->id == OBJ_Krystal) {
//     //     arg1->unk23C.min.x = minX+20;
//     //     arg1->unk23C.max.x = maxX-20;
//     //     arg1->unk23C.min.y = (minY - arg1->unk254);
//     //     arg1->unk23C.max.y = (arg1->unk254 + maxY);
//     //     arg1->unk23C.min.z = minZ+20;
//     //     arg1->unk23C.max.z = maxZ-20;
//     // }
// }

// RECOMP_PATCH void dll_27_func_5A8(Object* arg0, DLL27_Data* arg1) {
//     s32 var_a2;

//     if (arg0->id == OBJ_Krystal) return;

//     var_a2 = 0;
//     if ((arg1->unk257 == 1) || (arg1->unk257 == 2)) {
//         if (arg1->unk0 & 4) {
//             var_a2 = 1;
//         }
//         if (arg1->unk0 & 0x01000000) {
//             var_a2 = (var_a2 | 0x20) & 0xFF;
//         }

//         func_80053750(arg0, &arg1->unk23C, var_a2);
//     }
// }
