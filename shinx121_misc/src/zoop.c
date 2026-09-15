// #include "PR/gbi.h"
// #include "PR/libaudio.h"
// #include "PR/os.h"
// #include "dbgui.h"
// #include "dlls/engine/33_BaddieControl.h"
// #include "game/objects/object.h"
// #include "game/objects/object_id.h"
// #include "libnaudio/n_sndplayer.h"
// #include "modding.h"
// #include "recomputils.h"
// #include "recompevents.h"
// #include "rt64_extended_gbi.h"
// #include "sys/asset_thread.h"
// #include "sys/exception.h"
// #include "sys/fs.h"
// #include "sys/main.h"
// #include "sys/math.h"
// #include "sys/rcp.h"
// #include "sys/rsp_segment.h"
// #include "sys/audio.h"

// #include "PR/ultratypes.h"

// #include "sys/map.h"
// #include "sys/map_enums.h"
// #include "sys/objects.h"
// #include "sys/vi.h"
// #include "dll.h"

// // static s32 zoop = 5;

// // RECOMP_CALLBACK("*", recomp_on_game_tick) void asdasd() {
// //     if (zoop == 0) {
// //         zoop = -1;
// //         //warpPlayer(39, 0);

// //         Object *player = get_player();
// //         if (player != NULL) {
// //             Vec3f pos = {-1264, -1805, -8519};
// //             player->srt.transl = pos;
// //             player->positionMirror = pos;
// //             player->positionMirror2 = pos;
// //             player->positionMirror3 = pos;
// //         }
// //     }
// // }

// // #include "recomp/dlls/objects/210_player_recomp.h"

// // RECOMP_HOOK_RETURN_DLL(dll_210_control) void asd() {
// //     if (zoop > 0) {
// //         zoop--;
// //     }
// // }

// RECOMP_HOOK("__dll5_amseq_set") void amseq_set_hook(Object* obj, u16 musicActionID) {
//     recomp_printf("amseq_set(%s, %u)\n", obj != NULL && obj->def != NULL ? obj->def->name : "", musicActionID);
// }

// // #include "sys/main.h"
// // #include "game/gamebits.h"

// // static int zoop = 2;

// // RECOMP_HOOK("map_func_8004773C") void map_func_8004773C_hook(void) {
// //     if (zoop == 0) return;
// //     zoop--;

// //     PlayerLocation *savedPlayerLocation = gDLL_29_Gplay->vtbl->get_player_saved_location();
// //     savedPlayerLocation->vec.x = -943.0f;
// //     savedPlayerLocation->vec.y = 86.5f;
// //     savedPlayerLocation->vec.z = 15739.0f;

// //     mainSetBits(0x680, 0);
// //     mainSetBits(0x773, 1);

// //     gDLL_29_Gplay->vtbl->set_map_setup(MAP_GOLDEN_PLAINS, 0);
// //     gDLL_29_Gplay->vtbl->set_obj_group_status(MAP_GOLDEN_PLAINS, 1, 1);
// //     gDLL_29_Gplay->vtbl->set_obj_group_status(MAP_GOLDEN_PLAINS, 3, 1);
// //     //gDLL_29_Gplay->vtbl->set_obj_group_status(MAP_GOLDEN_PLAINS, 12, 1);
// // }
// // void func_8001F4C0(Block* arg0, s32 arg1, s32 arg2);
// // RECOMP_HOOK("func_80043950") void asd(Block* arg0, s32 arg1, s32 arg2) {
// //     func_8001F4C0(arg0, arg1, arg2);
// // }

// // RECOMP_HOOK("func_8001F4C0") void asdasd(void) {
// //     recomp_printf("boop!\n");
// // }

// // extern Vec3f D_800BB198;
// // RECOMP_PATCH void func_8001F4C0(Block* arg0, s32 arg1, s32 arg2) {
// //     f32 sp9C;
// //     f32 sp98;
// //     f32 sp94;
// //     Vec3f sp88;
// //     Vec3f sp7C;
// //     f32 sp70;
// //     Camera* camera;
// //     Vtx_t* temp_fp;
// //     Vtx_t* var_s0;
// //     f32 temp_fa1;
// //     f32 temp_fs0;
// //     f32 temp_fs1;
// //     f32 temp_fs2;
// //     f32 temp_fs3;
// //     f32 temp_fs4;
// //     f32 temp_fs5;
// //     f32 temp_ft0;
// //     f32 temp_ft2;
// //     f32 temp_ft3;
// //     f32 temp_ft4;
// //     f32 temp_fv0;
// //     f32 temp_fv0_2;
// //     f32 temp_fv0_3;
// //     f32 temp_fv1;
// //     f32 var_fv1;
// //     f32 var_fv1_2;
// //     s16 targetVertexIndex;
// //     s16 vertexIndex;
// //     s32 i;
// //     BlockShape* shape;
// //     Vtx_t* temp_v0_3;

// //     recomp_printf("boop\n");

// //     camera = get_main_camera();
// //     sp9C = camera->srt.transl.f[0] - (((f32) arg1 * 640.0f) + gWorldX);
// //     sp98 = camera->srt.transl.f[1];
// //     sp94 = camera->srt.transl.f[2] - (((f32) arg2 * 640.0f) + gWorldZ);
// //     temp_fp = arg0->vertices2[(arg0->vtxFlags & 1) ^ 1];
// //     for (i = 0; i < arg0->shapeCount; i++) {
// //         shape = &arg0->shapes[i];
// //         //if ((shape->flags & 0x4000) && (shape->flags & 0x10000000)) {
// //             vertexIndex = shape->vtxBase;
// //             targetVertexIndex = shape[1].vtxBase;
// //             while (vertexIndex < targetVertexIndex) {
// //                 sp88.f[0] = 0.0f;
// //                 sp88.f[1] = 1.0f;
// //                 sp88.f[2] = 0.0f;
// //                 temp_v0_3 = &arg0->vertices[vertexIndex];
// //                 temp_fs3 = (f32) temp_v0_3->ob[0];
// //                 temp_fs4 = (f32) temp_v0_3->ob[1];
// //                 temp_fs5 = (f32) temp_v0_3->ob[2];
// //                 temp_fs0 = sp9C - temp_fs3;
// //                 temp_fs1 = sp98 - temp_fs4;
// //                 temp_fs2 = sp94 - temp_fs5;
// //                 temp_fv0 = sqrtf(SQ(temp_fs0) + SQ(temp_fs1) + SQ(temp_fs2));
// //                 if (temp_fv0 != 0.0f) {
// //                     var_fv1 = 1.0f / temp_fv0;
// //                 } else {
// //                     var_fv1 = 0.0f;
// //                 }
                
// //                 sp7C.f[2] = temp_fs0 * var_fv1;
// //                 sp7C.f[1] = temp_fs1 * var_fv1;
// //                 sp7C.f[0] = temp_fs2 * var_fv1;
// //                 temp_fv0_2 = D_800BB198.f[0] - temp_fs3;
// //                 temp_fa1 = D_800BB198.f[1] - temp_fs4;
// //                 temp_ft4 = D_800BB198.f[2] - temp_fs5;
// //                 sqrtf(SQ(temp_fv0_2) + SQ(temp_fa1) + SQ(temp_ft4));
// //                 temp_fv0_3 = sqrtf(SQ(sp88.f[2]) + (SQ(sp88.f[0]) + SQ(sp88.f[1])));
// //                 if (temp_fv0_3 != 0.0f) {
// //                     var_fv1_2 = 1.0f / temp_fv0_3;
// //                 } else {
// //                     var_fv1_2 = 0.0f;
// //                 }
// //                 temp_ft2 = sp88.f[0] * var_fv1_2;
// //                 temp_ft0 = sp88.f[1] * var_fv1_2;
// //                 temp_ft3 = sp88.f[2] * var_fv1_2;
// //                 sp88.f[0] = temp_ft2;
// //                 sp88.f[1] = temp_ft0;
// //                 sp88.f[2] = temp_ft3;
// //                 sp70 = temp_ft3 * sp7C.f[0];
// //                 temp_fv1 = 2.0f * (sp70 + ((temp_ft2 * sp7C.f[2]) + (temp_ft0 * sp7C.f[1])));
// //                 temp_fp[vertexIndex].tc[0] = (s16) (s32) ((((temp_ft2 * temp_fv1) - sp7C.f[2]) + 1.0f) * 1024.0f);
// //                 temp_fp[vertexIndex].tc[1] = (s16) (s32) ((((temp_ft0 * temp_fv1) - sp7C.f[1]) + 1.0f) * 1024.0f);
// //                 vertexIndex += 1;
// //             }
// //         //}
// //     }
// // }
// // // size: 0x10
// // typedef struct {
// // /*0*/ CurveSetup *curve; // aka: node
// // /*4*/ u32 goalDist;
// // /*8*/ u32 netDist;
// // /*C*/ u8 prevPointIdx;
// // /*D*/ u8 unkD;
// // /*E*/ u8 visited;
// // } RoutePoint;

// // // element of a binary max-heap/priority queue
// // // size: 0x8
// // typedef struct {
// // /*0*/ u32 priority;
// // /*4*/ u16 pointIdx; // index of route point this element represents
// // } RouteHeapElement;

// // // size: 0x30
// // typedef struct {
// // /*00*/ RoutePoint *routePoints; // len: 254
// // /*04*/ RouteHeapElement *heap; // len: 254
// // /*08*/ CurveSetup **route; // len: 100
// // /*0C*/ Vec3f *unkC; // target/goal?
// // /*10*/ void *unk10;
// // /*14*/ u8 _unk14[0x18 - 0x14];
// // /*18*/ CurveSetup *unk18;
// // /*1C*/ s32 lastPointIdx;
// // /*20*/ s16 routePointsLength;
// // /*22*/ s16 heapLength;
// // /*24*/ u32 unk24;
// // /*28*/ u8 unk28;
// // /*29*/ u8 _unk29[0x2A - 0x29];
// // /*2A*/ s16 routeLength;
// // /*2C*/ s16 routeCurrIdx;
// // /*2E*/ u8 _unk2E[0x30 - 0x2E];
// // } Route;
// // void func_80039F08(RouteHeapElement* heap, s32 heapLength, s32 idx);

// // RECOMP_PATCH s32 func_8003A0B8(RouteHeapElement* heap, s16* heapLengthPtr) {
// //     u16 pointIdx;
    
// //     if (*heapLengthPtr == 0) {
// //         return -1;
// //     }

// //     pointIdx = heap[1].pointIdx;

// //     if (pointIdx < 0 || pointIdx >= 254) {
// //         recomp_eprintf("woah pointIdx out of bounds! %d \n", pointIdx);
// //         return -1;
// //     }

// //     heap[1].priority = heap[*heapLengthPtr].priority;
// //     heap[1].pointIdx = heap[*heapLengthPtr].pointIdx;
// //     *heapLengthPtr -= 1;
// //     func_80039F08(heap, *heapLengthPtr, 1);
// //     return (s32) pointIdx;
// // }

// // RECOMP_PATCH s32 func_8003A240(Route* route, CurveSetup* curve, s32* outVisited) {
// //     RoutePoint* point;
// //     s32 i;

// //     for (i = 0; i < route->routePointsLength; i++) {
// //         point = &route->routePoints[i];
// //         if (curve == point->curve) {
// //             //*outVisited = point->visited;
// //             *outVisited = 0;
// //             return i;
// //         }
// //     }
// //     return -1;
// // }


// // #include "sys/voxmap.h"

// // // size: 0xE
// // typedef struct {
// //     Vec3s16 unk0;
// //     u16 unk6;
// //     u16 unk8;
// //     u8 unkA;
// //     u8 unkB;
// //     UNK_TYPE_8 unkC;
// //     u8 padD;
// // } Unk80008E40Unk0;

// // typedef struct {
// //     u16 unk0;
// //     u16 unk2;
// // } Unk80008E40Unk4;


// // typedef struct {
// //     Unk80008E40Unk0 *unk0;
// //     Unk80008E40Unk4 *unk4;
// //     Vec3f *unk8;
// //     Vec3s16 unkC;
// //     Vec3s16 unk12;
// //     UNK_TYPE_32 unk18;
// //     s16 unk1C;
// //     s16 unk1E;
// //     s16 unk20;
// //     s16 unk22;
// //     s16 unk24;
// //     u8 unk26;
// // } Unk80008E40;

// // extern void func_800095B0(Gfx** gdl, Vtx_t** vertices, DLTri** tris, Mtx** matrics, Unk80008E40* arg4);

// // static Object *self;
// // static Gfx **gdl;
// // static Mtx **mtxs;
// // static Vertex **vtxs;
// // static Triangle **pols;
// // static s8 visibility;

// // static void print_hook(Object *self_, Gfx **gdl_, Mtx **mtxs_, Vertex **vtxs_, Triangle **pols_, s8 visibility_) {
// //     self = self_;
// //     gdl = gdl_;
// //     mtxs = mtxs_;
// //     vtxs = vtxs_;
// //     pols = pols_;
// //     visibility = visibility_;
// // }

// // static void print_hook_ret(const char *tag) {
// //     if (self->data == NULL) return;

// //     DLL33_Data *dll33Data = self->data;
// //     Unk80008E40 *thing = (Unk80008E40*)&dll33Data->unk374;

// //     recomp_printf("%s %d\n", tag, thing->unk20);
// //     func_800095B0(gdl, vtxs, pols, mtxs, thing);
// // }

// // #include "recomp/dlls/_asm/215_recomp.h"
// // RECOMP_HOOK_DLL(dll_215_print) void dll_215_print_hook(Object *self, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, s8 visibility) {
// //     print_hook(self, gdl, mtxs, vtxs, pols, visibility);
// // }
// // RECOMP_HOOK_RETURN_DLL(dll_215_print) void dll_215_print_hook_ret() {
// //     print_hook_ret("SharpClaw");
// // }

// // #include "recomp/dlls/_asm/216_recomp.h"
// // RECOMP_HOOK_DLL(dll_216_print) void dll_216_print_hook(Object *self, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, s8 visibility) {
// //     print_hook(self, gdl, mtxs, vtxs, pols, visibility);
// // }
// // RECOMP_HOOK_RETURN_DLL(dll_216_print) void dll_216_print_hook_ret() {
// //     print_hook_ret("SnowWorm/SandEel");
// // }

// // #include "recomp/dlls/_asm/219_recomp.h"
// // RECOMP_HOOK_DLL(dll_219_print) void dll_219_print_hook(Object *self, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, s8 visibility) {
// //     print_hook(self, gdl, mtxs, vtxs, pols, visibility);
// // }
// // RECOMP_HOOK_RETURN_DLL(dll_219_print) void dll_219_print_hook_ret() {
// //     print_hook_ret("WallCrawler");
// // }

// // #include "recomp/dlls/objects/225_SabreBaddie_recomp.h"
// // RECOMP_HOOK_DLL(dll_225_print) void dll_225_print_hook(Object *self, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, s8 visibility) {
// //     print_hook(self, gdl, mtxs, vtxs, pols, visibility);
// // }
// // RECOMP_HOOK_RETURN_DLL(dll_225_print) void dll_225_print_hook_ret() {
// //     print_hook_ret("SabreBaddie");
// // }

// // #include "recomp/dlls/_asm/229_recomp.h"
// // RECOMP_HOOK_DLL(dll_229_print) void dll_229_print_hook(Object *self, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, s8 visibility) {
// //     print_hook(self, gdl, mtxs, vtxs, pols, visibility);
// // }
// // RECOMP_HOOK_RETURN_DLL(dll_229_print) void dll_229_print_hook_ret() {
// //     print_hook_ret("BassaltWeavel");
// // }

// // #include "recomp/dlls/_asm/428_recomp.h"
// // RECOMP_HOOK_DLL(dll_428_print) void dll_428_print_hook(Object *self, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, s8 visibility) {
// //     print_hook(self, gdl, mtxs, vtxs, pols, visibility);
// // }
// // RECOMP_HOOK_RETURN_DLL(dll_428_print) void dll_428_print_hook_ret() {
// //     print_hook_ret(":capy:");
// // }

// // extern void NOTosSetTime(s32 arg0, s32 arg1);

// // extern Gfx *gCurGfx;
// // extern u16 *gFramebufferCurrent;

// // extern s32 D_80092A50;
// // extern s32 D_80092A54;
// // extern s32 D_800B49E0;

// static s32 windowOpen = FALSE;
// // static s32 go = FALSE;
// // static s32 D_80092A50_target = 3;
// // static s32 D_80092A54_target = 15;
// // static s32 on = FALSE;

// #include "sys/newshadows.h"
// #include "sys/objects.h"
// #include "sys/memory.h"

// // RECOMP_PATCH u32 func_8004D8A4(Object* obj, u32 addr, s32 arg2) {
// //     ObjectStruct64 *obj64;

// //     obj64 = (ObjectStruct64 *) mmAlign4(addr);
// //     obj->unk64 = obj64;
// //     addr = (u32)(obj64 + 1);
// //     obj64->unk4 = texture_load((s32) -obj->def->shadowTexture, 0);
// //     obj64->unk0 = obj->def->unk00;
// //     obj64->unk2c = obj64->unk0*2;
// //     obj64->unk14.x = D_80092BD0;
// //     obj64->unk14.y = D_80092BD4;
// //     obj64->unk14.z = D_80092BD8;
// //     obj64->unk36 = 0x40;
// //     obj64->flags = 4;
// //     obj64->unk38 = 0x19;
// //     obj64->unk39 = 0x4B;
// //     obj64->unk3A = 0x96;
// //     obj64->unk3B = 0x64;
// //     func_8004D974(1);
// //     return addr;
// // }

// // static s32 recomp_shadowFaceFilter = 8; 
// // static s32 recomp_shadowFaceFilter2 = 0; 
// // static s32 recomp_shadowFaceFilterEq = 0; 

// // RECOMP_PATCH s32 func_80052300(Object* arg0, UnkFunc80051D68Arg3 *arg1, Unk8004FA58* arg2, UnkFunc80052300Arg3* arg3, s32 arg4, f32 arg5, f32 arg6, s32 arg7, s32 arg8) {
// //     s32 sp44;
// //     s32 var_a2;
// //     Unk8005341C* temp_t2;
// //     s8 var_t1;
// //     s8 var_v1;
// //     f32 sp34;
// //     f32 sp30;
// //     Unk8005341C* temp_v0;
// //     f32 f0;

// //     sp34 = arg0->srt.transl.x;
// //     sp30 = arg0->srt.transl.z;
// //     if (arg0->parent == NULL) {
// //         sp34 -= arg5;
// //         sp30 -= arg6;
// //     }
// //     temp_v0 = func_8005341C(&sp44);
// //     temp_t2 = &temp_v0[sp44];
// //     sp44 = 0;
// //     var_a2 = 0;
// //     if (arg8) {
// //         var_t1 = 4;
// //     } else {
// //         var_t1 = recomp_shadowFaceFilter;
// //     }
// //     if (temp_v0 < temp_t2) {
// //         f0 = 0.00012208521f;
// //         do {
// //             if (temp_v0->unk0 == arg0->parent) {
// //                 sp44 = temp_v0->unk4;
// //                 while (sp44 < temp_v0[1].unk4) {
// //                     /*
// //                       wall = 0x3
// //                      */
// //                     var_v1 = TRUE;
// //                     if (!(arg1[sp44].unk30 & var_t1) && (arg1[sp44].unk30 & recomp_shadowFaceFilter2)) {
// //                         var_v1 = FALSE;
// //                     }
// //                     var_v1 = arg1[sp44].unk30 == recomp_shadowFaceFilterEq;
// //                     if (var_v1 != FALSE) {
// //                         arg3->unk0[0].x = arg1[sp44].unkA[0] - sp34;
// //                         arg3->unk0[0].y = arg1[sp44].unkA[3] - arg0->srt.transl.y;
// //                         arg3->unk0[0].z = arg1[sp44].unk16[0] - sp30;
// //                         arg3->unk0[1].x = arg1[sp44].unkA[1] - sp34;
// //                         arg3->unk0[1].y = arg1[sp44].unkA[4] - arg0->srt.transl.y;
// //                         arg3->unk0[1].z = arg1[sp44].unk16[1] - sp30;
// //                         arg3->unk0[2].x = arg1[sp44].unkA[2] - sp34;
// //                         arg3->unk0[2].y = arg1[sp44].unkA[5] - arg0->srt.transl.y;
// //                         arg3->unk0[2].z = arg1[sp44].unk16[2] - sp30;
// //                         arg3 += 1;
// //                         arg2[var_a2].pos.x = arg1[sp44].unk4 * f0;
// //                         arg2[var_a2].pos.y = arg1[sp44].unk6 * f0;
// //                         arg2[var_a2].pos.z = arg1[sp44].unk8 * f0;
// //                         arg2[var_a2].unk10 = arg1[sp44].unk30;
// //                         var_a2++;
// //                     }
// //                     sp44++;
// //                 }
// //             }
// //             temp_v0++;
// //         } while (temp_v0 < temp_t2);
// //     }
// //     return var_a2;
// // }

// // extern void func_8005CA88(Object *obj, f32 *a1, u8 a2);
// // extern void func_8005CC74(Gfx **gdl, Object *arg1);

// // extern Gfx *gCurGfx;
// // extern u8 D_800BCC78;

// // RECOMP_HOOK_RETURN("func_8004225C") void idk_draw(void) {
// //     Object *player = get_player();
// //     if (player != NULL) {
// //         func_8005CC74(&gCurGfx, player);
// //     }
// // }

// // RECOMP_HOOK_RETURN("func_80051D68") void func_80051D68_hook(void) {
// //     s32 ret = recomphook_get_return_s32();
// //     recomp_printf("func_80051D68 -> %d\n", ret);
// // }

// RECOMP_CALLBACK(".", my_cheats_menu_event) void zoop_test_callback() {
//     dbgui_menu_item("asdasd", &windowOpen);
// }

// // static s32 gomyshadow = FALSE;
// // static s32 shadtype = 0;

// // RECOMP_ON_GAME_TICK_START_CALLBACK void asdasd(void) {
// //     if (gomyshadow) {
// //         Object *player = get_player();
// //         if (player != NULL) {
// //             Vec3f vec = { player->srt.transl.x, 0.0f - 8, player->srt.transl.z };

// //             func_8005CA88(player, vec.f, shadtype);
// //         }
// //     }
// // }

// #include "sys/fonts.h"
// // #include "libnaudio/n_sndp.h"

// // #define SNDSTATEFLAG_01 0x01
// // #define SNDSTATEFLAG_02 0x02
// // #define SNDSTATEFLAG_04 0x04
// // #define SNDSTATEFLAG_10 0x10
// // #define SNDSTATEFLAG_20 0x20

// // extern N_ALSndPlayer *g_SndPlayer;

// // extern sndstate *func_80066064(ALBank *bank, ALSound *sound);

// // static sndstate *some_sound_func2(ALBank *bank, s16 soundnum, u16 vol, ALPan pan, f32 pitch, u8 fxmix, u8 fxbus, sndstate **handleptr) {
// // 	sndstate *state;
// // 	sndstate *state2 = NULL;
// // 	ALKeyMap *keymap;
// // 	ALSound *sound;
// // 	s16 sp4e = 0;
// // 	s32 sp48;
// // 	s32 sp44;
// // 	s32 sp40 = 0;
// // 	s32 abspan;
// // 	N_ALEvent evt;
// // 	N_ALEvent evt2;

// // 	if (soundnum != 0) {
// // 		do {
// //             recomp_printf("play(%X)\n", soundnum);
// //             // if (instnum >= bank->instCount) break;
// //             // if (soundnum - 1 >= bank->instArray[instnum]->soundCount) break;

// // 			sound = bank->instArray[0]->soundArray[soundnum - 1];
// // 			state = func_80066064(bank, sound);

// // 			if (state != NULL) {
// // 				g_SndPlayer->target = (s32)state;
// // 				evt.type = AL_SNDP_PLAY_EVT;
// // 				evt.msg.generic.sndstate = state;
// // 				abspan = pan + state->pan - AL_PAN_CENTER;

// // 				if (abspan > 127) {
// // 					abspan = 127;
// // 				} else if (abspan < 0) {
// // 					abspan = 0;
// // 				}

// // 				state->pan = abspan;
// // 				state->vol = (u32)(vol * state->vol) >> 15;
// // 				state->pitch *= pitch;
// // 				state->fxmix = fxmix;
// // 				state->fxbus = fxbus;

// // 				sp44 = sound->keyMap->velocityMax * 33333;

// // 				if (state->flags & SNDSTATEFLAG_10) {
// // 					state->flags &= ~SNDSTATEFLAG_10;
// // 					n_alEvtqPostEvent(&g_SndPlayer->evtq, &evt, sp40 + 1, 0);
// // 					sp48 = sp44 + 1;
// // 					sp4e = soundnum;
// // 				} else {
// // 					n_alEvtqPostEvent(&g_SndPlayer->evtq, &evt, sp44 + 1, 0);
// // 				}

// // 				state2 = state;
// // 				if (1) {}
// // 			} else {
// //                 osSyncPrintf("Sound state allocate failed - sndId %d\n", soundnum);
// //             }

// // 			sp40 += sp44;
// // 			keymap = sound->keyMap;
// // 			soundnum = keymap->velocityMin + (keymap->keyMin & 0xc0) * 4;
// //             recomp_printf("a\n");
// // 		} while (soundnum && state);

// // 		if (state2 != NULL) {
// // 			state2->flags |= 0x1;
// // 			state2->unk30 = handleptr;

// // 			if (sp4e != 0) {
// // 				state2->flags |= SNDSTATEFLAG_10;

// // 				evt2.type = AL_SNDP_0200_EVT;
// // 				evt2.msg.generic.sndstate = state2;
// // 				evt2.msg.generic.data = sp4e;
// // 				evt2.msg.generic.data2 = bank;

// // 				n_alEvtqPostEvent(&g_SndPlayer->evtq, &evt2, sp48, 0);
// // 			}
// // 		}
// // 	}

// // 	if (handleptr != NULL) {
// // 		*handleptr = state2;
// // 	}

// // 	return state2;
// // }

// // extern ALBankFile *__dll6__bss_0;
// // extern ALBankFile **__dll5__bss_4;

// // static ALBank* lgy_bank;
// // static ALInstrument* lgy_inst;
// // static ALEnvelope* lgy_env;
// // static ALKeyMap* lgy_key;

// // // static const size_t lgy_offsets[] = {
// // // 	0x1C5C0,
// // // 	0x1C680
// // // };

// // #include "mod_common.h"

// // RECOMP_HOOK_RETURN("game_init") void soundshit(void) {
// //     // ALBank *musicBank = __dll5__bss_4[1]->bankArray[0];

// //     // ALBank *bank = mmAlloc(sizeof(ALBank) + (sizeof(ALInstrument*) * 1), 0, NULL);
// //     // bzero(bank, sizeof(ALBank));
// //     // bank->sampleRate = musicBank->sampleRate;
    
// //     // ALInstrument *instArray = mmAlloc(sizeof(ALInstrument) * 1 + (sizeof(ALSound*) * 2), 0, NULL);
// //     // bzero(instArray, sizeof(ALInstrument) * 1);
// //     // ALInstrument *inst = &instArray[0];
// //     // bank->instCount = 1;
// //     // bank->instArray[0] = inst;

// //     // inst->volume = 255;
// //     // inst->pan = AL_PAN_CENTER;
// //     // inst->priority = 5;
// //     // inst->bendRange = 200;

// //     // inst->soundCount = 2;
// //     // inst->soundArray[0] = __dll5__bss_4[1]->bankArray[0]->instArray[0x75]->soundArray[0xC];
// //     // inst->soundArray[1] = __dll5__bss_4[1]->bankArray[0]->instArray[0x75]->soundArray[0xD]; // that one

// //     // lgy_env = mmAlloc(sizeof(ALEnvelope), 0, NULL);
// // 	// bzero(lgy_env, sizeof(ALEnvelope));
// // 	// {
// // 	// 	lgy_env->decayTime = 6462404;
// // 	// 	lgy_env->releaseTime = 0;
// // 	// 	lgy_env->attackVolume = 127;
// // 	// 	lgy_env->decayVolume = 127;
// // 	// }

// // 	// lgy_key = mmAlloc(sizeof(ALKeyMap), 0, NULL);
// // 	// bzero(lgy_key, sizeof(ALKeyMap));
// // 	// {
// // 	// 	lgy_key->keyBase = 60;
// // 	// }

// //     // for (s32 i = 0; i < 2; i++) {
// //     //     ALSound *sound = inst->soundArray[i];

// //     //     sound->envelope = lgy_env;
// //     //     sound->keyMap = lgy_key;
// //     // }

// //     // lgy_bank = bank;





// //     // ALSound *lgy_snds[2] = {
// //     //     __dll5__bss_4[1]->bankArray[0]->instArray[2]->soundArray[0],
// //     //     __dll5__bss_4[1]->bankArray[0]->instArray[2]->soundArray[1],
// //     // };

// // 	// //const int lgy_count = sizeof(lgy_offsets) / sizeof(lgy_offsets[0]);
// // 	// const int lgy_count = sizeof(lgy_snds) / sizeof(lgy_snds[0]);

// //     // lgy_env = mmAlloc(sizeof(ALEnvelope), 0, NULL);
// // 	// bzero(lgy_env, sizeof(ALEnvelope));
// // 	// {
// // 	// 	lgy_env->decayTime = 2688000;
// // 	// 	lgy_env->releaseTime = 3004000;
// // 	// 	lgy_env->attackVolume = AL_VOL_FULL;
// // 	// 	lgy_env->decayVolume = AL_VOL_FULL;
// // 	// }

// // 	// lgy_key = mmAlloc(sizeof(ALKeyMap), 0, NULL);
// // 	// bzero(lgy_key, sizeof(ALKeyMap));
// // 	// {
// // 	// 	lgy_key->keyBase = 54;
// // 	// }

// // 	// lgy_inst = mmAlloc(sizeof(ALInstrument) + (sizeof(ALSound*) * (lgy_count - 1)), 0, NULL);
// // 	// bzero(lgy_inst, sizeof(ALInstrument) + (sizeof(ALSound*) * (lgy_count - 1)));
// // 	// {
// // 	// 	lgy_inst->volume = AL_VOL_FULL;
// // 	// 	lgy_inst->pan = AL_PAN_CENTER;
// // 	// 	lgy_inst->priority = 5;
// // 	// 	lgy_inst->bendRange = 200;

// // 	// 	lgy_inst->soundCount = lgy_count;
// // 	// 	for (s32 i = 0; i < lgy_count; i++)
// // 	// 	{
// // 	// 		//ALSound* sound = (void*)(music_ctl + lgy_offsets[i]);
// // 	// 		ALSound* sound = lgy_snds[i];
// //     //         recomp_printf("lgy[%d] = 0x%X\n", i, (u32)&sound - (u32)&__dll5__bss_4[1]);
// // 	// 		lgy_inst->soundArray[i] = sound;
// // 	// 	}
// // 	// }

// // 	// lgy_bank = mmAlloc(sizeof(ALBank), 0, NULL);
// // 	// bzero(lgy_bank, sizeof(ALBank));
// // 	// {
// // 	// 	lgy_bank->instCount = 1;
// // 	// 	lgy_bank->sampleRate = 22050;
// // 	// 	lgy_bank->instArray[0] = lgy_inst;
// // 	// }

// // 	// for (s32 i = 0; i < lgy_count; i++)
// // 	// {
// // 	// 	//ALSound* sound = (void*)(music_ctl + lgy_offsets[i]);
// // 	// 	ALSound* sound = lgy_snds[i];

// // 	// 	sound->envelope = lgy_env;
// // 	// 	sound->keyMap = lgy_key;
// // 	// }
// // }

// void func_8001440C(s32 arg0);
// void clear_PlayerPosBuffer(void);
// void game_init(void);
// void init_bittable(void);
// void game_tick_no_expansion(void);
// void game_tick(void);
// void func_80014074(void);
// void alloc_frame_buffers(void);
// void func_80013D80(void);
// s8 func_800143FC(void);
// void update_PlayerPosBuffer();
// void test_write(void);
// void check_dongle(void);

// extern s8 D_800B09C0;
// extern s32 D_8008C968;

// static Object *thenet;

// RECOMP_CALLBACK(".", my_dbgui_event) void zoop_test_dbgui_callback() {
//     if (windowOpen) {
//         if (dbgui_begin("asdasdasd", &windowOpen)) {
//             // const static DbgUiInputIntOptions inputOptions = {
//             //     .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
//             // };
//             // dbgui_input_int_ext("! this", &recomp_shadowFaceFilter, &inputOptions);
//             // dbgui_input_int_ext("& this", &recomp_shadowFaceFilter2, &inputOptions);
//             // dbgui_input_int_ext("= this", &recomp_shadowFaceFilterEq, &inputOptions);

//             // s32 D_800BCC78_32 = D_800BCC78;
//             // if (dbgui_checkbox("D_800BCC78", &D_800BCC78_32)) {
//             //     D_800BCC78 = D_800BCC78_32;
//             // }
//             // dbgui_input_int("Type", &shadtype);

//             // static s32 on = FALSE;
//             // dbgui_checkbox("On", &on);
//             // gomyshadow = FALSE;
//             // if (on || dbgui_button("Go")) {
//             //     gomyshadow = TRUE;
//             // }

//             // static s32 fontID = 0;
//             // dbgui_input_int("font ID", &fontID);

//             // if (dbgui_button("kill")) {
//             //     font_unload(fontID);
//             // }

//             // if (dbgui_button("1")) {
//             //     some_sound_func2(lgy_bank, 1, 0x7FFF, AL_PAN_CENTER, 1, 0, 1, NULL);
//             //     //some_sound_func2(__dll5__bss_4[1]->bankArray[0], 0x75, 0xC, 0x4000U, 0x40U, 1.0f, 0U, 1U, NULL);
//             // }
//             // if (dbgui_button("2")) {
//             //     some_sound_func2(lgy_bank, 2, 0x7FFF, AL_PAN_CENTER, 1, 0, 1, NULL);
//             //     //some_sound_func2(__dll5__bss_4[1]->bankArray[0], 0x75, 0xD, 0x4000U, 0x40U, 1.0f, 0U, 1U, NULL);
//             // }
// //             static s32 inst = 0;
// //             static s32 sound = 0;
// //             static const DbgUiInputIntOptions hexInput = {
// //     .step = 1,
// //     .stepFast = 0x10,
// //     .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
// // };
// // if (dbgui_input_int_ext("inst", &inst, &hexInput)) {
// //                         if (inst < 0) inst = 0;
// //                     }
// //                     if (dbgui_input_int_ext("sound", &sound, &hexInput)) {
// //                         if (sound < 0) sound = 0;
// //                     }
// //              if (dbgui_button("go")) {
// //                 some_sound_func2(__dll5__bss_4[1]->bankArray[0], inst, sound + 1, 0x7FFF, AL_PAN_CENTER, 1.0f, 0U, 1U, NULL);
// //             }

//             // if (dbgui_button("warpppp")) {
//             //     // PlayerLocation *temp_v0;

//             //     // s32 playerno = 1;

//             //     // func_8001440C(0);

//             //     // if (playerno <= PLAYER_NONE) {
//             //     //     playerno = PLAYER_SABRE;
//             //     // }

//             //     // clear_PlayerPosBuffer();

//             //     // gDLL_30_Task->vtbl->load_recently_completed();
//             //     // gDLL_29_Gplay->vtbl->set_playerno(playerno);

//             //     // temp_v0 = gDLL_29_Gplay->vtbl->get_player_saved_location();

//             //     // map_func_80048054(MAP_CLOUDRUNNER_TRAPROOMS, 0, &temp_v0->vec.x, &temp_v0->vec.y, &temp_v0->vec.z, &temp_v0->mapLayer);

//             //     // // temp_v0->vec.x = 345.4056091308594f;
//             //     // // temp_v0->vec.y = 30.69402503967285f;
//             //     // // temp_v0->vec.z = 1696.9542236328126f;
//             //     // temp_v0->vec.x = 291;
//             //     // temp_v0->vec.y = 172;
//             //     // temp_v0->vec.z = 1608;

//             //     // gDLL_29_Gplay->vtbl->checkpoint(&temp_v0->vec, 0, 0, temp_v0->mapLayer);

//             //     // D_800B09C0 = 1;
//             //     // D_8008C968 = 0;


//             //     Object *player = get_player();
//             //     player->srt.transl.x = 291;
//             //     player->srt.transl.y = 172;
//             //     player->srt.transl.z = 1608;
//             //     player->positionMirror = player->srt.transl;
//             //     player->positionMirror2 = player->srt.transl;
//             //     player->positionMirror3 = player->srt.transl;
//             // }

//             static s32 mapID = 0;
//             static s32 setupID = 0;
//             static s32 playerNo = 0;
//             dbgui_input_int("map ID", &mapID);
//             dbgui_input_int("setup ID", &setupID);
//             dbgui_input_int("player num", &playerNo);

//             if (dbgui_button("change map")) {
//                 main_change_map(mapID, setupID, playerNo, -1);
//             }
            
//             // dbgui_input_int("D_80092A50", &D_80092A50_target);
//             // dbgui_input_int("D_80092A54", &D_80092A54_target);
//             // dbgui_checkbox("On", &on);

//             // if (on) {
//             //     NOTosSetTime(D_80092A50_target, D_80092A54_target);
//             //     go = TRUE;
//             // }
//             // if (dbgui_button("Go")) {
//             //     go = TRUE;
//             // }

//             // dbgui_textf("D_80092A50: %d", D_80092A50);
//             // dbgui_textf("D_80092A54: %d", D_80092A54);
//             // dbgui_textf("D_800B49E0: %d", D_800B49E0);

//             if (dbgui_button("fishingnet")) {
//                 Object *player = get_player();
//                 if (player != NULL) {
//                     ObjSetup *setup = obj_alloc_setup(sizeof(ObjSetup), 9);
//                     setup->fadeDistance = 0xFF;
//                     setup->loadDistance = 0xFF;
//                     setup->loadFlags = OBJSETUP_LOAD_MAIN | OBJSETUP_LOAD_LEVEL;
//                     setup->x = player->globalPosition.x;
//                     setup->y = player->globalPosition.y;
//                     setup->z = player->globalPosition.z;

//                     thenet = obj_create(setup, OBJINIT_STANDALONE | OBJINIT_BY_TABIDX, map_world_xz_to_map_id(player->srt.transl.x, player->srt.transl.z), 0, NULL);
//                 }
//             }

//             if (dbgui_button("0x1")) {
//                 if (thenet != NULL) ((DLL_Unknown*)thenet->dll)->vtbl->func[10].withThreeArgs((s32)thenet, 0x1, NULL);
//             }
//             dbgui_same_line();
//             if (dbgui_button("0x2")) {
//                 if (thenet != NULL) ((DLL_Unknown*)thenet->dll)->vtbl->func[10].withThreeArgs((s32)thenet, 0x2, NULL);
//             }
//             dbgui_same_line();
//             if (dbgui_button("0x4")) {
//                 if (thenet != NULL) ((DLL_Unknown*)thenet->dll)->vtbl->func[10].withThreeArgs((s32)thenet, 0x4, NULL);
//             }
      
//             if (dbgui_button("0x8")) {
//                 if (thenet != NULL) ((DLL_Unknown*)thenet->dll)->vtbl->func[10].withThreeArgs((s32)thenet, 0x8, NULL);
//             }
//             dbgui_same_line();
//             if (dbgui_button("0x20")) {
//                 if (thenet != NULL) ((DLL_Unknown*)thenet->dll)->vtbl->func[10].withThreeArgs((s32)thenet, 0x20, NULL);
//             }
//             dbgui_same_line();
//             if (dbgui_button("0x40")) {
//                 if (thenet != NULL) ((DLL_Unknown*)thenet->dll)->vtbl->func[10].withThreeArgs((s32)thenet, 0x40, NULL);
//             }
//             dbgui_same_line();

//             if (dbgui_button("BassaltWeavel")) {
//                 Object *player = get_player();
//                 if (player != NULL) {
//                     Baddie_Setup *setup = obj_alloc_setup(sizeof(Baddie_Setup), OBJ_BassaltWeavel);
//                     setup->base.fadeDistance = 0xFF;
//                     setup->base.loadDistance = 0xFF;
//                     setup->base.loadFlags = OBJSETUP_LOAD_MAIN;
//                     setup->base.x = player->globalPosition.x;
//                     setup->base.y = player->globalPosition.y;
//                     setup->base.z = player->globalPosition.z;
//                     setup->unk18 = -1;
//                     setup->unk1A = -1;
//                     setup->unk1C = -1;
//                     setup->unk2E = -1;
//                     setup->unk30 = -1;
//                     setup->quarterHitpoints = 6;

//                     obj_create((ObjSetup*)setup, OBJINIT_STANDALONE, map_world_xz_to_map_id(player->srt.transl.x, player->srt.transl.z), 0, NULL);
//                 }
//             }

//             if (dbgui_button("DIM_Boss")) {
//                 Object *player = get_player();
//                 if (player != NULL) {
//                     Baddie_Setup *setup = obj_alloc_setup(sizeof(Baddie_Setup), OBJ_DIM_Boss);
//                     setup->base.fadeDistance = 0xFF;
//                     setup->base.loadDistance = 0xFF;
//                     setup->base.loadFlags = OBJSETUP_LOAD_MAIN;
//                     setup->base.x = player->globalPosition.x;
//                     setup->base.y = player->globalPosition.y;
//                     setup->base.z = player->globalPosition.z;

//                     obj_create((ObjSetup*)setup, OBJINIT_STANDALONE, -1, 0, NULL);
//                 }
//             }

//             if (dbgui_button("OBJ_PineCone")) {
//                 Object *player = get_player();
//                 if (player != NULL) {
//                     ObjSetup *setup = obj_alloc_setup(sizeof(ObjSetup) + 0x10, OBJ_PineCone);
//                     setup->fadeDistance = 0xFF;
//                     setup->loadDistance = 0xFF;
//                     setup->loadFlags = OBJSETUP_LOAD_MAIN;
//                     setup->x = player->globalPosition.x;
//                     setup->y = player->globalPosition.y;
//                     setup->z = player->globalPosition.z;

//                     obj_create(setup, OBJINIT_STANDALONE, -1, 0, NULL);
//                 }
//             }

//             if (dbgui_button("OBJ_PirahnaBaddie")) {
//                 Object *player = get_player();
//                 if (player != NULL) {
//                     ObjSetup *setup = obj_alloc_setup(sizeof(ObjSetup) + 0x10, OBJ_PirahnaBaddie);
//                     setup->fadeDistance = 0xFF;
//                     setup->loadDistance = 0xFF;
//                     setup->loadFlags = OBJSETUP_LOAD_MAIN;
//                     setup->x = player->globalPosition.x;
//                     setup->y = player->globalPosition.y;
//                     setup->z = player->globalPosition.z;

//                     obj_create(setup, OBJINIT_STANDALONE, -1, 0, NULL);
//                 }
//             }

//             if (dbgui_button("OBJ_Beacon")) {
//                 Object *player = get_player();
//                 if (player != NULL) {
//                     ObjSetup *setup = obj_alloc_setup(sizeof(ObjSetup) + 0x10, OBJ_Beacon);
//                     setup->fadeDistance = 0xFF;
//                     setup->loadDistance = 0xFF;
//                     setup->loadFlags = OBJSETUP_LOAD_MAIN;
//                     setup->x = player->globalPosition.x;
//                     setup->y = player->globalPosition.y;
//                     setup->z = player->globalPosition.z;

//                     obj_create(setup, OBJINIT_STANDALONE, -1, 0, NULL);
//                 }
//             }
//         }
//         dbgui_end();
//     }
// }

// // RECOMP_HOOK_RETURN("func_8003E9F0") void func_8003E9F0_hook(void) {
// //     if (go) {
// //         NOTosSetTime(D_80092A50_target, D_80092A54_target);
// //         go = FALSE;
// //     }
// // }

// // RECOMP_PATCH void func_8003E9F0(Gfx** gdl, s32 updateRate) {
// //     s32 temp_v0;
// //     s32 *v0;

// //     if (D_80092A50 > 0) {
// //         // yeah I don't even know what this is
// //         if ((D_80092A50 == 0xA) && ((temp_v0 = D_800B49E0) == 0)) {
// //             D_800B49E0 = temp_v0 = D_80092A54;
// //         }
// //         //func_8003FE70(gdl, *D_80092A54, D_80092A50, func_80004A4C());
// //         v0 = &D_800B49E0;
// //         if (D_80092A50 == 0xA) {
// //             temp_v0 = D_800B49E0;
// //             temp_v0 -= updateRate;
// //             *v0 = temp_v0;
// //             if (*v0 <= 0) {
// //                 D_80092A50 = 0;
// //                 *v0 = 0;
// //             }
// //         } else {
// //             D_80092A50 = 0;
// //         }
// //         return;
// //     } else {
// //         D_800B49E0 = 0;
// //     }
// // }

// // RECOMP_ON_GAME_TICK_START_CALLBACK void zoop_game_tick_start(void) {

// // }

// extern Gfx *gMainGfx[2];
// extern Gfx *gCurGfx;
// extern Mtx *gMainMtx[2];
// extern Mtx *gCurMtx;
// extern Vertex *gMainVtx[2];
// extern Vertex *gCurVtx;
// extern Triangle *gMainPol[2];
// extern Triangle *gCurPol;

// // extern u8 gFrameBufIdx;
// // extern s8 gPauseState;

// // extern void rdp_init(Gfx **gdl);

// // static void do_fake_frame_start(void) {
// //     u8 phi_v1;
// //     u32 updateRate;
// //     Gfx **gdl;

// //     osSetTime(0);
// //     // dl_next_debug_info_set();

// //     gdl = &gCurGfx;

// //     // unused return type
// //     gfxtask_run_xbus(gMainGfx[gFrameBufIdx], gCurGfx, 0);

// //     gFrameBufIdx ^= 1;
// //     gCurGfx = gMainGfx[gFrameBufIdx];
// //     gCurMtx = gMainMtx[gFrameBufIdx];
// //     gCurVtx = gMainVtx[gFrameBufIdx];
// //     gCurPol = gMainPol[gFrameBufIdx];

// //     // @recomp: Enable RT64 extended GBI commands
// //     gEXEnable((*gdl)++);

// //     // dl_add_debug_info(gCurGfx, 0, "main/main.c", 0x28E);
// //     rsp_segment(&gCurGfx, SEGMENT_MAIN, (void *)K0BASE);
// //     rsp_segment(&gCurGfx, SEGMENT_FRAMEBUFFER, gFramebufferCurrent);
// //     rsp_segment(&gCurGfx, SEGMENT_ZBUFFER, D_800BCCB4);
// //     //func_8003E9F0(&gCurGfx, gUpdateRate);
// //     dl_set_all_dirty();
// //     func_8003DB5C();

// //     if (gDLBuilder->needsPipeSync != 0) {
// //         gDLBuilder->needsPipeSync = 0;
// //         gDPPipeSync(gCurGfx++);
// //     }

// //     gDPSetDepthImage(gCurGfx++, SEGMENT_ZBUFFER << 24);

// //     rsp_init(&gCurGfx);
// //     phi_v1 = 2;

// //     if (func_80041D5C() == 0)
// //         phi_v1 = 0;
// //     else if (func_80041D74() == 0)
// //         phi_v1 = 3;

// //     func_80037A14(&gCurGfx, &gCurMtx, phi_v1);
// // }

// // static void do_fake_frame_end(void) {
// //     u32 updateRate;
// //     Gfx **gdl;

// //     gdl = &gCurGfx;

// //     // rt64 hack
// //     u32 viSize = vi_get_current_size();
// //     u32 viWidth = GET_VIDEO_WIDTH(viSize);
// //     u32 viHeight = GET_VIDEO_HEIGHT(viSize);

// //     gDPSetCombineMode((*gdl)++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM); 
// //     gDPSetOtherMode((*gdl)++, 
// //         G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | 
// //             G_TP_PERSP | G_CYC_1CYCLE |  G_PM_NPRIMITIVE, 
// //         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2);
// //     gDPSetScissor((*gdl)++, G_SC_NON_INTERLACE, 0, 0, viWidth, viHeight);
// //     gDPSetFillColor((*gdl)++, (GPACK_RGBA5551(0, 0, 0, 0) << 16) | GPACK_RGBA5551(0, 0, 0, 0));
// //     gDPFillRectangle((*gdl)++, 0, 0, viWidth, viHeight);
// //     // rt64 hack

// //     gDPFullSync(gCurGfx++);
// //     gSPEndDisplayList(gCurGfx++);

// //     gfxtask_wait();
// //     // obj_do_deferred_free();
// //     // mmFreeTick();

// //     if (gPauseState == 0) {
// //         func_80001A3C();
// //     }

// //     gUpdateRate = vi_frame_sync(0);
    
// //     //if (0) {}

// //     updateRate = (u32) gUpdateRate;
// //     if (gUpdateRate > 6) {
// //         gUpdateRate = 6;
// //         updateRate = gUpdateRate;
// //     }
// //     gUpdateRateF = (f32) updateRate;
// //     gUpdateRateInverseF = 1.0f / gUpdateRateF;
// //     gUpdateRateMirror = updateRate;
// //     gUpdateRateMirrorF = gUpdateRateF;
// //     gUpdateRateInverseMirrorF = 1.0f / gUpdateRateMirrorF;
    
// //     // func_80014074();
// //     // write_c_file_label_pointers("main/main.c", 0x37C);
// // }

// // extern u8 sBGPrimColourR;
// // extern u8 sBGPrimColourG;
// // extern u8 sBGPrimColourB;

// // static void zoop_fbfx(s32 var_t0, s32 arg1) {
// //     Gfx **gdl = &gCurGfx;

// //     gfxtask_wait();

// //     f32 var_v1_2 = (arg1 - var_t0) / 3.0f;
// //     if (var_v1_2 >= 5.0f) {
// //         var_v1_2 = -1.0f;
// //     }

// //     u32 viSize = vi_get_current_size();
// //     u32 viWidth = GET_VIDEO_WIDTH(viSize);
// //     u32 viHeight = GET_VIDEO_HEIGHT(viSize);

// //     s32 ulx, uly, lrx, lry;
// //     func_80002130(&ulx, &uly, &lrx, &lry);

// //     f32 alpha = var_v1_2 == -1.0f ? 0.0f : 1.0f / recomp_powf(2, var_v1_2);

// //     recomp_printf("alpha: %f\n", alpha);

// //     ///
// //     // gDPSetCombineMode((*gdl)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
// //     // gDPSetOtherMode((*gdl)++, 
// //     //     G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_PERSP | G_CYC_FILL |  G_PM_NPRIMITIVE, 
// //     //     G_AC_NONE | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2);
// //     // gDPPipeSync((*gdl)++);
// //     // dl_set_fill_color(gdl, (GPACK_ZDZ(G_MAXFBZ, 0) << 16) | GPACK_ZDZ(G_MAXFBZ, 0));
// //     // gDPFillRectangle((*gdl)++, ulx, uly, lrx, lry);
// //     // gDPPipeSync((*gdl)++);
// //     // dl_set_fill_color(gdl, 
// //     //     (GPACK_RGBA5551(sBGPrimColourR, sBGPrimColourG, sBGPrimColourB, 1) << 16) 
// //     //         | GPACK_RGBA5551(sBGPrimColourR, sBGPrimColourG, sBGPrimColourB, 1));
// //     // gDPFillRectangle((*gdl)++, 0, 0, viWidth - 1, viHeight - 1);
// //     // gDPPipeSync((*gdl)++);
// //     ///

// //     gSPClearGeometryMode((*gdl)++, 0xFFFFFF);
// //     gDPSetCombineMode((*gdl)++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);    
// //     gDPSetOtherMode((*gdl)++, 
// //         G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | 
// //         G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | 
// //         G_PM_NPRIMITIVE, G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2);

// //     gDPSetPrimColor((*gdl)++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
// //     gDPLoadTextureTile((*gdl)++, gFramebufferNext, G_IM_FMT_RGBA, G_IM_SIZ_16b,
// //         viWidth, viHeight,
// //         0, 0,
// //         viWidth - 1, viHeight - 1, 0, 
// //         G_TX_CLAMP | G_TX_NOMIRROR, 
// //         G_TX_CLAMP | G_TX_NOMIRROR,
// //         G_TX_NOMASK, G_TX_NOMASK,
// //         G_TX_NOLOD, G_TX_NOLOD
// //     );
// //     gSPTextureRectangle((*gdl)++, 0, 0, (viWidth) * 4, (viHeight) * 4, 0, 0, 0, 1 << 10, 1 << 10);

// //     // gDPSetPrimColor((*gdl)++, 0, 0, 0xFF, 0xFF, 0xFF, alpha);
// //     // gDPLoadTextureTile((*gdl)++, gFramebufferNext, G_IM_FMT_RGBA, G_IM_SIZ_16b,
// //     //     viWidth, viHeight,
// //     //     0, 0,
// //     //     viWidth - 1, viHeight - 1, 0, 
// //     //     G_TX_CLAMP | G_TX_NOMIRROR, 
// //     //     G_TX_CLAMP | G_TX_NOMIRROR,
// //     //     G_TX_NOMASK, G_TX_NOMASK,
// //     //     G_TX_NOLOD, G_TX_NOLOD
// //     // );
// //     // gSPTextureRectangle((*gdl)++, 0, 0, (viWidth) * 4, (viHeight) * 4, 0, 0, 0, 1 << 10, 1 << 10);

// //     gDPPipeSync((*gdl)++);
// //     gDPSetCombineMode((*gdl)++, G_CC_DECALRGBA, G_CC_DECALRGBA); 
// //     gDPSetOtherMode((*gdl)++, 
// //         G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | 
// //             G_TP_PERSP | G_CYC_FILL |  G_PM_NPRIMITIVE, 
// //         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2);
// //     gDPSetScissor((*gdl)++, G_SC_NON_INTERLACE, 0, 0, viWidth, viHeight);
// //     gDPSetFillColor((*gdl)++, (GPACK_RGBA5551(0, 0, 0, 255) << 16) | GPACK_RGBA5551(0, 0, 0, 255));
// //     gDPFillRectangle((*gdl)++, var_t0 * 8, var_t0 * 8, 64 + var_t0 * 8, 64 + var_t0 * 8);
// // }

// // RECOMP_HOOK("dl_next_debug_info_set") void zoop_fbfx_hook(void) {
// //     if (D_80092A50 > 0 && D_80092A50 != 10) {
// //         s32 arg1 = D_80092A54;

// //         for (s32 var_t0 = 0; var_t0 < arg1; var_t0++) {
// //             do_fake_frame_start();
// //             zoop_fbfx(var_t0, arg1);
// //             do_fake_frame_end();
// //         }
// //     }
// // }

// // RECOMP_ON_GAME_TICK_CALLBACK void zoop_game_tick(void) {
// //     Gfx **gdl = &gCurGfx;

// //     if (D_80092A50 == 10) {
// //         u32 viSize = vi_get_current_size();
// //         u32 viWidth = GET_VIDEO_WIDTH(viSize);
// //         u32 viHeight = GET_VIDEO_HEIGHT(viSize);

// //         // scale effect with framerate
// //         // https://github.com/Zelda64Recomp/Zelda64Recomp/blob/ab677e76615e5e47b3b26c822ca426485752ac77/patches/effect_patches.c#L90-L102
// //         s32 alpha = 128;
// //         f32 exponent = 30.0f / recomp_get_refresh_rate();
// //         f32 alpha_float = recomp_powf(alpha / 255.0f, exponent);
// //         alpha_float = MIN(alpha_float, 0.825f); // TODO: enable high precision framebuffer and remove this line
// //         alpha = (s32)(alpha_float * 255.0f);

// //         gSPClearGeometryMode((*gdl)++, 0xFFFFFF);
// //         gDPSetCombineMode((*gdl)++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);    
// //         gDPSetOtherMode((*gdl)++, 
// //             G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | 
// //             G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | 
// //             G_PM_NPRIMITIVE, G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2);
// //         gDPSetPrimColor((*gdl)++, 0, 0, 0xFF, 0xFF, 0xFF, alpha);
// //         gDPLoadTextureTile((*gdl)++, gFramebufferNext, G_IM_FMT_RGBA, G_IM_SIZ_16b,
// //             viWidth, viHeight,
// //             0, 0,
// //             viWidth - 1, viHeight - 1, 0, 
// //             G_TX_CLAMP | G_TX_NOMIRROR, 
// //             G_TX_CLAMP | G_TX_NOMIRROR,
// //             G_TX_NOMASK, G_TX_NOMASK,
// //             G_TX_NOLOD, G_TX_NOLOD
// //         );
// //         gSPTextureRectangle((*gdl)++, 0, 0, (viWidth) * 4, (viHeight) * 4, 0, 0, 0, 1 << 10, 1 << 10);
// //     }
// // }

// //#include "recomp/dlls/objects/211_Tricky_recomp.h"
// //#include "recomp/dlls/objects/496_snowhorn_recomp.h"

// void shadows_update_dynamic_tex(Object *obj, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols);

// static Object *_obj;
// static Gfx** _gdl;
// static Mtx** _mtxs;
// static Vertex** _vtxs;
// static Triangle** _pols;

// // RECOMP_HOOK("copy_obj_position_mirrors") void obj_shadow_setup_hook(Object *obj) {
// //     _obj = obj;
// // }

// // static s32 nextShadowTexID = 1;

// // RECOMP_HOOK_RETURN("copy_obj_position_mirrors") void obj_shadow_setup_hook_ret(void) {
// //     if (_obj != NULL && _obj->shadow != NULL) {
// //         if (nextShadowTexID >= 2) return;
// //         if (_obj->shadow->flags & 8) return;

// //         if (_obj->id != OBJ_MagicPlant) return;

// //         //_obj->shadow->unk2c = _obj->shadow->unk0 * 0.5f;
// //         //_obj->shadow->flags |= (8 | nextShadowTexID | 0x4000 | 0x200 | 0x20);
// //         _obj->shadow->flags |= (8 | nextShadowTexID);
// //         _obj->shadow->flags &= ~0x800;
// //         // _obj->shadow->flags &= ~0x10;
// //         // _obj->shadow->tr.x = 0;
// //         // _obj->shadow->tr.y = 0;
// //         // _obj->shadow->tr.z = 0;
// //         nextShadowTexID++;
// //     }
// // }

// // RECOMP_HOOK("shadow_update_obj") void shadow_update_obj_hook(Object* obj, s32 arg1, s32 arg2, s32 updateRate) {
// //     // if (obj->id == OBJ_NWmammothwalk) {
// //     //     MtxF mf;
// //     //     SRT srt = {
// //     //         .transl = { 0, 0, 0 },
// //     //         .flags = 0,
// //     //         .scale = 1.0f,
// //     //         .yaw = obj->srt.yaw,
// //     //         .pitch = obj->srt.pitch,
// //     //         .roll = obj->srt.roll
// //     //     };
// //     //     matrix_from_srt_reversed(&mf, &srt);
// //     //     Vec3f offset_relative = {0,100,-50};
// //     //     Vec3f offset;
// //     //     vec3_transform_no_translate(&mf, &offset_relative, &offset);
// //     //     obj->shadow->tr.x = obj->srt.transl.x + offset.x;
// //     //     obj->shadow->tr.y = obj->srt.transl.y + offset.y;
// //     //     obj->shadow->tr.z = obj->srt.transl.z + offset.z;
// //     // }

// // }

// RECOMP_HOOK("draw_object") void draw_object_shadow(Object *obj, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols, f32 yPrescale) {
//     _obj = obj;
//     _gdl = gdl;
//     _mtxs = mtxs;
//     _vtxs = vtxs;
//     _pols = pols;
// }

// RECOMP_HOOK_RETURN("draw_object") void draw_object_shadow_ret(void) {
//     if (_obj != NULL && _obj->shadow != NULL && _obj->shadow->flags & 8) {
//         if (_obj->id == OBJ_Krystal || _obj->id == OBJ_Sabre) return;

//         recomp_printf("recomp shadow %d  0x%X\n", _obj->id, _obj->shadow->flags);
//         shadows_update_dynamic_tex(_obj, _gdl, _mtxs, _vtxs, _pols);
//     }
// }

// // RECOMP_PATCH void func_800359D0(Object *obj, Gfx **gdl, Mtx **rspMtxs,Vertex **vtxs, Triangle **pols, u32 param_6)
// // {
// //     Gfx *mygdl;
// //     Mtx *outRspMtxs;
// //     void *d;
// //     u32 mainIdx;
// //     u32 otherIdx;
// //     ModelInstance *modelInst;
// //     ModelInstance *modelInst2;

// //     mygdl = *gdl;
// //     outRspMtxs = *rspMtxs;

// //     mainIdx = obj->modelInstIdx;
// //     if (param_6) {
// //         otherIdx = 0;
// //     } else {
// //         otherIdx = obj->def->numModels - 1;
// //     }

// //     modelInst = obj->modelInsts[mainIdx];
// //     modelInst2 = obj->modelInsts[otherIdx];

// //     //if (modelInst2 == NULL) {
// //         modelInst2 = modelInst;

// //         // static s32 debounce = 1;
// //         // if (debounce && obj->id == OBJ_Sabre) {
// //         //     debounce = 0;
// //         //     Gfx *ptr = modelInst2->displayList;

// //         //     while (1) {
// //         //         recomp_printf("%08X %08X ", ptr->words.w0, ptr->words.w1);
                
// //         //         if (ptr->words.w0 == _SHIFTL(G_ENDDL, 24, 8)) {
// //         //             break;
// //         //         }

// //         //         ptr++;
// //         //     }

// //         //     recomp_printf("\n");
// //         // }
// //     //}

// //     if (obj->id == OBJ_Sabre) {
// //         gSPNoOp(&modelInst2->displayList[0]);
// //         gSPLoadGeometryMode(&modelInst2->displayList[1], G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
// //         gDPSetCombineLERP(&modelInst2->displayList[2], SHADE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED)
// //         gDPSetOtherMode(&modelInst2->displayList[3], G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE, G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_OPA_SURF2);
    
// //         Gfx *ptr = modelInst2->displayList + 4;

// //         while (1) {
// //             // if ((ptr->words.w0 & 0xFF000000) == _SHIFTL(G_DL, 24, 8)) {
// //             //     gSPNoOp(ptr);
// //             // }
// //             // if ((ptr->words.w0 & 0xFF000000) == _SHIFTL(G_GEOMETRYMODE, 24, 8)) {
// //             //     gSPNoOp(ptr);
// //             // }
// //             // if ((ptr->words.w0 & 0xFF000000) == _SHIFTL(G_SETCOMBINE, 24, 8)) {
// //             //     gSPNoOp(ptr);
// //             // }
// //             if ((ptr->words.w0 & 0xFF000000) == _SHIFTL(G_RDPSETOTHERMODE, 24, 8)) {
// //                 gSPNoOp(ptr);
// //             }
            
// //             if (ptr->words.w0 == _SHIFTL(G_ENDDL, 24, 8)) {
// //                 break;
// //             }

// //             ptr++;
// //         }
// //     }

// //     d = modelInst->matrices[modelInst->unk34 & 1];
// //     gSPSegment(mygdl++, SEGMENT_3, d);
// //     gSPSegment(mygdl++, SEGMENT_5, modelInst2->vertices[(modelInst2->unk34 >> 1) & 0x1]);
// //     gSPDisplayList(mygdl++, OS_K0_TO_PHYSICAL(modelInst2->displayList));

// //     dl_set_all_dirty();
// //     func_8003DB5C();

// //     *gdl = mygdl;
// //     *rspMtxs = outRspMtxs;
// // }

// #include "recomp/dlls/engine/5_AMSEQ_recomp.h"

// extern s32 D_800967A0[127];
// extern f32 D_8009699C[100];

// struct oscstate2 {
//  struct oscstate2 *unk00;
//  u8 unk04;
//  s32 unk08;
//  f32 unk0c;
//  f32 unk10;
//     u16 unk14;
//     u16 unk16;
//     f32 unk18;
//     f32 unk1C;
// };

// extern struct oscstate2 *_bss_14;

// // RECOMP_PATCH s32 dll_5_func_2C44(void** oscState, f32* initVal, u8 oscType, u8 oscRate, u8 oscDepth, u8 oscDelay, u8 unk07) {
// //     struct oscstate2* var_v0_2;
// //     f32 oscDepthf;

// //     recomp_printf("yuh %d  %d/16000=%f (%f)\n", unk07, D_800967A0[unk07], 
// //         1.0f/(D_800967A0[unk07]/16000), 1.0f/((f32)D_800967A0[unk07]/16000));

// //     if (oscRate > 99) {
// //         oscRate = 99;
// //     }
// //     if (oscDelay > 127) {
// //         oscDelay = 127;
// //     }
// //     if (unk07 > 127) {
// //         unk07 = 127;
// //     }
// //     if (_bss_14 == NULL) {
// //         recomp_printf("amAudio: WARNING no free oscstates.\n");
// //         return 0;
// //     }
// //     var_v0_2 = _bss_14;
// //     if (unk07 == 0) {
// //         var_v0_2->unk18 = 1;
// //         var_v0_2->unk1C = 0;
// //     } else {
// //         var_v0_2->unk18 = 0;
// //         var_v0_2->unk1C = 1.0f / ((f32)D_800967A0[unk07] / 16000);
// //     }
// //     var_v0_2->unk04 = oscType;
// //     var_v0_2->unk14 = 0;
// //     var_v0_2->unk16 = 1000000.0f / (f32)D_8009699C[oscRate] / 16000;
// //     var_v0_2->unk08 = 16000;
// //     oscDepthf = (f32) oscDepth;
// //     if (oscType & 0x80) {
// //         oscDepthf = _depth2Cents(oscDepthf);
// //     }
// //     oscType &= ~0x80;
// //     switch (oscType & ~0x80) {
// //     case 2:
// //     case 3:
// //     case 4:
// //     case 5:
// //         var_v0_2->unk0c = oscDepthf;
// //         if ((oscType) == 2) {
// //             var_v0_2->unk10 = -oscDepthf;
// //         } else {
// //             var_v0_2->unk10 = 0;
// //         }
// //         var_v0_2->unk08 = 500000.0f / (f32)D_8009699C[oscRate];
// //         break;
// //     case 6:
// //     case 8:
// //     case 11:
// //     case 12:
// //         var_v0_2->unk10 = 0;
// //         var_v0_2->unk0c = oscDepthf;
// //         break;
// //     case 7:
// //     case 9:
// //     case 13:
// //         var_v0_2->unk10 = oscDepthf * 0.5f;
// //         var_v0_2->unk0c = oscDepthf * 0.5f;
// //         break;
// //     case 10:
// //         var_v0_2->unk10 = -oscDepthf;
// //         var_v0_2->unk0c = oscDepthf * 2;
// //         break;
// //     default:
// //         recomp_printf("amAudio: unsupported vibrato/tremolo type,%d\n", var_v0_2->unk04);
// //         return 0;
// //     }

// //     if (var_v0_2->unk04 & 0x80) {
// //         *initVal = alCents2Ratio(var_v0_2->unk10);
// //     } else {
// //         *initVal = var_v0_2->unk10 + 127.0f;
// //     }
// //     *oscState = var_v0_2;
// //     _bss_14 = _bss_14->unk00;
// //     if (oscDelay) {
// //         return D_800967A0[oscDelay];
// //     }
    
// //     return 16000;
// // }

// RECOMP_HOOK("n_alCSPSetTempo") void n_alCSPSetTempo_hook(void *seqp, s32 tempo) {
//     recomp_printf("n_alCSPSetTempo(%p, %d)\n", seqp, tempo);
// }

// // #include "recomp/dlls/engine/3_ANIM_recomp.h"

// // extern s8 _data_1C[4];
// // extern s32 _data_20; // object ID of the thing the player should hold when playing the first time pickup sequence
// // extern Object *_data_24;

// // RECOMP_PATCH s32 dll_3_func_9E58(s32 arg0, Object *arg1, s32 arg2) {
// //     //_data_20 = arg0;
// //     _data_20 = 512;
// //     _data_24 = arg1;
// //     _data_1C[0] = arg2;
// //     return 1;
// // }

// #include "recomp/dlls/objects/210_player_recomp.h"

// // static s32 holdup = 100;

// RECOMP_HOOK_DLL(dll_210_print) void player_print_hook(Object *obj, Gfx **gdl, Mtx **mtxs, Vertex **vtxs, Triangle **pols) {
// //RECOMP_HOOK("draw_render_list") void egtjnrthijnrtsgh(void) {
//     static Vec3f something;

//     // Gfx **gdl = &gCurGfx;
//     // Mtx **mtxs = &gCurMtx;
//     // Vertex **vtxs = &gCurVtx;
//     // Triangle **pols = &gCurPol;

//     Object *player = get_player();
    
//     if (thenet != NULL && !(thenet->stateFlags & OBJSTATE_DESTROYED)) {
//         //if (holdup-- > 0) return;

//         thenet->srt.transl.y = player->globalPosition.y + 5;

//         if (thenet->srt.transl.x == 0 && thenet->srt.transl.z == 0) {
            

            
//         }

//         thenet->srt.transl.x = player->globalPosition.x;
//         thenet->srt.transl.z = player->globalPosition.z - 20;

//         something = thenet->srt.transl;
//             //something.x -= 5;
//             something.z -= 10;
//             something.y -= 50;


//         ((u8*)thenet->data)[1] = 1;

//         thenet->objhitInfo->unk58 |= 7;

//         //recomp_printf("the net is drawing\n");
//         //((DLL_Unknown*)thenet->dll)->vtbl->func[8].withSevenArgsAsdasdasdas(thenet, gdl, mtxs, vtxs, pols, &thenet->srt.transl, &something);
//     }
// }

// #include "PR/ultratypes.h"
// #include "game/gamebits.h"
// #include "game/objects/object_id.h"
// #include "sys/objanim.h"
// #include "sys/objects.h"
// #include "sys/gfx/animseq.h"
// #include "dlls/objects/210_player.h"
// #include "dlls/objects/common/foodbag.h"
// #include "dll.h"
// #include "recomp/dlls/objects/318_fish_recomp.h"

// typedef struct {
//     f32 unk0;
//     f32 unk4;
//     f32 unk8;
//     f32 unkC[2];
//     f32 unk14;
//     u8 unk18;
//     u8 unk19;
//     s16 unk1A;
//     s16 unk1C;
//     f32 unk20;
//     f32 unk24;
//     f32 unk28;
//     f32 unk2C;
//     s16 unk30;
//     f32 unk34;
//     f32 unk38;
//     f32 unk3C;
//     f32 unk40;
//     f32 unk44;
//     f32 unk48;
//     f32 unk4C;
//     f32 unk50;
//     f32 unk54;
//     f32 unk58;
//     f32 unk5C;
//     f32 unk60;
//     f32 unk64;
//     f32 unk68;
//     f32 unk6C;
//     f32 unk70;
//     f32 unk74;
//     f32 unk78;
//     u8 unk7C;
// } fish_Data;

// typedef struct {
//     ObjSetup base;
//     u8 unk18;
//     u8 unk19;
//     u8 unk1A;
//     u8 unk1B;
//     u16 unk1C;
//     u16 unk1E;
//     u8 unk20;
//     u8 unk21;
//     u8 unk22;
//     u8 unk23;
//     u8 unk24;
//     u16 unk26;
//     u8 unk28;
// } fish_Setup;

// extern void fish_func_10BC(Object* self);
// extern void fish_func_11C8(Object *self, s16 a1, u8 a2);
// extern s32 fish_func_1978(Object* self);
// extern s32 fish_func_1EF4(Object* self);
// extern void fish_func_2150(Object* self);
// extern void fish_func_84(Object* self);
// extern s32 fish_func_F74(Object* self);
// extern s32 fish_func_1880(Object *self, Vec3f* arg1, Vec3s16* arg2);

// RECOMP_PATCH void fish_control(Object* self) {
//     s32 temp_v0;
//     fish_Setup* setup;
//     f32 var_fa1;
//     fish_Data* objdata;
//     Object* player;
//     f32 temp_fa0;
//     f32 temp_fv1_2;
//     f32 temp_fv1_3;

//     setup = (fish_Setup*)self->setup;
//     objdata = self->data;
    
//     player = get_player();
//     if (objdata->unk7C == 0) {
//         fish_func_84(self);
//     }
//     if (player->id == 0x1F) {
//         if (mainGetBits(BIT_2EA) == 0) {
//             self->unkAF |= 8;
//         } else {
//             self->unkAF &= ~0x8;
//         }
//     } else {
//         if (mainGetBits(BIT_3E1) == 0) {
//             self->unkAF |= 8;
//         } else {
//             self->unkAF &= ~0x8;
//         }
//     }
//     temp_v0 = func_80025F40(self, NULL, NULL, NULL);
//     if ((temp_v0 != 0))
//         recomp_printf("fish collision: 0x%X\n", temp_v0);
//     if ((temp_v0 != 0) && (temp_v0 == 0x11)) {
//         fish_func_2150(self);
//         return;
//     }
//     objdata->unk4 += gUpdateRateF;
//     switch (objdata->unk18) {
//     case 0:
//         self->unkAF |= 8;
//         if ((f32) setup->unk1E <= objdata->unk4) {
//             objdata->unk18 = 1U;
//             objdata->unk4 -= (f32) setup->unk1E;
//         }
//         break;
//     case 1:
//         if (objdata->unk4 <= 60.0f) {
//             self->unkAF |= 8;
//             self->opacity = (u8) (s32) ((objdata->unk4 / 60.0f) * 255.0f);
//             fish_func_11C8(self, NULL, 0);
//         } else {
//             self->opacity = 0xFF;
//             self->objhitInfo->unk58 = objdata->unk30;
//             objdata->unk18 = 2U;
//             objdata->unk4 -= 60.0f;
//             fish_func_11C8(self, NULL, 0);
//         }
//         break;
//     case 2:
//         if (objdata->unk4 <= (f32) setup->unk1C) {
//             fish_func_10BC(self);
//         } else {
//             self->srt.transl.f[1] = setup->base.y;
//             objdata->unk18 = 3U;
//             objdata->unk4 = (f32) (objdata->unk4 - (f32) setup->unk1C);
//             fish_func_11C8(self, NULL, 0);
//             func_80023D30(self, 2, 0.0f, 0U);
//             func_80024D74(self, 0x1E);
//         }
//         break;
//     case 3:
//         if (objdata->unk4 <=  (f32) setup->unk1A) {
//             self->srt.transl.f[1] = setup->base.y + (((objdata->unk2C - setup->base.y) * objdata->unk4) /  (f32) setup->unk1A);
//             fish_func_10BC(self);
//         } else {
//             self->srt.transl.f[1] = objdata->unk2C;
//             objdata->unk18 = 4U;
//             objdata->unk4 = (f32) (objdata->unk4 - (f32) setup->unk1A);
//             fish_func_11C8(self, NULL, 0);
//             func_80023D30(self, 0, 0.0f, 0U);
//             func_80024D74(self, 0x1E);
//         }
//         break;
//     case 4:
//         if ((f32) setup->unk26 <= objdata->unk4) {
//             if (fish_func_F74(self) != 0) {
//                 objdata->unk4 = (f32) gUpdateRateF;
//                 objdata->unk18 = 5U;
//                 objdata->unk58 = (f32) self->srt.transl.f[0];
//                 objdata->unk5C = (f32) self->srt.transl.f[1];
//                 objdata->unk60 = (f32) self->srt.transl.f[2];
//                 objdata->unk54 = 0.0f;
                
//                 func_80023D30(self, 1, 0.0f, 0U);
//                 func_80024D74(self, 0x1E);
//                 fish_func_1EF4(self);
//             } else {
//                 fish_func_11C8(self, NULL, 0);
//             }
//         } else {
//             fish_func_10BC(self);
//         }
//         temp_fv1_2 = objdata->unk4 * objdata->unk4;
//         temp_fa0 = temp_fv1_2 * temp_fv1_2;
//         objdata->unk8 += gUpdateRateF;
//         var_fa1 = temp_fa0 * temp_fa0 * objdata->unk14;
//         if ((f32) setup->unk1B < var_fa1) {
//             var_fa1 = (f32) setup->unk1B;
//         }
//         temp_fv1_3 = 60.0f / var_fa1;
//         if (temp_fv1_3 <= objdata->unk8) {
//             objdata->unk8 = (f32) (objdata->unk8 - temp_fv1_3);
//             if ((player != NULL) && (vec3_distance_squared(&self->globalPosition, &player->globalPosition) < 10000.0f)) {
//                 gDLL_17_partfx->vtbl->spawn(self, 0x26, NULL, 0, -1, objdata->unkC);
//             }
//         }
//         break;
//     case 5:
//         if (fish_func_1EF4(self) == 0) {
//             objdata->unk58 = (f32) self->srt.transl.f[0];
//             objdata->unk5C = (f32) self->srt.transl.f[1];
//             objdata->unk60 = (f32) self->srt.transl.f[2];
//             fish_func_1978(self);
//         }
//         break;
//     case 6:
//         if (fish_func_1978(self) == 0) {
//             fish_func_11C8(self, NULL, 0);
//         }
//         break;
//     case 7:
//         if (objdata->unk4 <= (f32) setup->unk24) {
//             self->srt.transl.f[1] = objdata->unk2C - (((objdata->unk2C - setup->base.y) * objdata->unk4) / (f32) setup->unk24);
//             fish_func_10BC(self);
//         } else {
//             self->srt.transl.f[1] = setup->base.y;
//             objdata->unk18 = 2U;
//             objdata->unk4 = (f32) (objdata->unk4 - (f32) setup->unk24);
//             fish_func_11C8(self, NULL, 0);
//             func_80023D30(self, 0, 0.0f, 0U);
//             func_80024D74(self, 0x1E);
//         }
//         break;
//     }
//     func_80024108(self, objdata->unk0, gUpdateRateF, NULL);
// }

// #include "PR/libaudio.h"
// #include "PR/os.h"
// #include "libnaudio/n_libaudio.h"
// #include "libnaudio/n_unkfuncs.h"
// #include "sys/asset_thread.h"
// #include "sys/audio/speaker.h"
// #include "sys/audio.h"
// #include "sys/fs.h"
// #include "sys/main.h"
// #include "sys/memory.h"
// #include "dll.h"
// #include "macros.h"
// #include "prevent_bss_reordering.h"
// #include "sys/mpeg.h" 

// extern OSSched* gAudioSched;
// extern OSScClient gAudioScClient;
// extern OSThread gAudioThread;
// extern OSScTask gAudioTask;
// extern u64 gAudioThreadStack[STACKSIZE(AUDIO_THREAD_STACK_SIZE)];
// extern N_ALGlobals __am_g;
// extern Acmd *__am_ACMDList[NUM_ACMD_LISTS];
// extern void *__am_audioInfo[NUM_OUTPUT_BUFFERS];
// extern short gFrameSamplesList[NUM_OUTPUT_BUFFERS];
// extern OSMesgQueue __am_audioFrameMsgQ;
// extern OSMesgQueue __am_audioReplyMsgQ;
// extern OSMesg __am_audioFrameMsgBuf[MAX_MESGS];
// extern OSMesg __am_audioReplyMsgBuf[MAX_MESGS];
// extern AMDMAState dmaState;
// extern AMDMABuffer gAudDmaBuffs[NUM_DMA_BUFFERS];
// extern s32 D_800AB078; // gMinFrameSize?
// extern s32 D_800AB07C; // gFrameSize?
// extern s32 D_800AB080; // gMaxFrameSize?
// extern s32 D_800AB084; // gAudioLagFrames?
// extern OSIoMesg audDMAIOMesgBuf[NUM_DMA_MESSAGES];
// extern OSMesgQueue audDMAMessageQ;
// extern OSMesg audDMAMessageBuf[NUM_DMA_MESSAGES];
// extern u32 D_800AB960;

// extern void __amMain(void *arg);
// extern void __amHandleFrameMsg(void);
// extern void __amHandleDoneMsg(void);
// extern void __clearAudioDMA(void);
// extern ALDMAproc __amDmaNew(AMDMAState **state);

// RECOMP_PATCH void init_audio(OSSched* sched, OSPri threadPriority) {
//     ALSynConfig c;
//     f32 fsize;
//     s32 frameSize;
//     u32* audioTab;
//     s32 offset;
//     s32 size;
//     s32 i;

//     audioTab = NULL;
    
// #ifdef AVOID_UB
//     mpeg_init(NULL);
// #else
//     mpeg_init();
// #endif
    
//     c.maxVVoices = c.maxPVoices = 72;
//     c.maxUpdates = 96;
//     c.maxFXbusses = 2;
//     c.outputRate = osAiSetFrequency(OUTPUT_RATE);
//     c.dmaproc = &__amDmaNew;
//     c.heap = NULL;
    
//     queue_alloc_load_file((void**)&audioTab, AUDIO_TAB);
    
//     offset = audioTab[24];
//     size = audioTab[25] - offset;
//     assetRomLoadSection((void** ) &D_800AB960, AUDIO_BIN, offset, size);

//     recomp_printf("asdasdasdasdadadsadasdasdasdas\n");

//     const int fx = 0;
    
//     offset = audioTab[D_800AB960 + fx];
//     size = audioTab[D_800AB960 + fx + 1] - offset;
    
//     c.fxTypes[0] = AL_FX_CUSTOM;
//     c.params[0] = mmAlloc(size, ALLOC_TAG_AUDIO_COL, NULL);
//     assetRomLoadSection((void**)c.params[0], AUDIO_BIN, offset, size);
    
//     c.fxTypes[1] = AL_FX_CUSTOM;
//     c.params[1] = mmAlloc(size, ALLOC_TAG_AUDIO_COL, NULL);
//     assetRomLoadSection((void**)c.params[1], AUDIO_BIN, offset, size);
    
//     n_alInit(&__am_g, &c);
    
//     mmFree(c.params[0]);
//     mmFree(c.params[1]);
//     mmFree(audioTab);
    
//     /*
//      * Calculate the frame sample parameters from the
//      * video field rate and the output rate
//      */
//     fsize = ((f32) c.outputRate * 2) / (f32) gDisplayHertz;
//     D_800AB078 = (s32)fsize;
//     if (D_800AB078 < fsize) {
//         D_800AB078++;
//     }
//     D_800AB078 = ((D_800AB078 / 184) * 184) + 184;
//     D_800AB07C = D_800AB078 - 184;
//     D_800AB080 = D_800AB078 + 80;

//     D_800AB084 = 0;

//     for (i = 0; i < NUM_ACMD_LISTS; i++) {
//         __am_ACMDList[i] = (Acmd*)alHeapAlloc(c.heap, 1, 0x4000);
//     }
//     for (i = 0; i < NUM_OUTPUT_BUFFERS; i++) {
//         __am_audioInfo[i] = alHeapAlloc(c.heap, 1, D_800AB080 * 4);
//         gFrameSamplesList[i] = 0;
//     }
    
//     gAudDmaBuffs->node.prev = NULL;
//     gAudDmaBuffs->node.next = NULL;
//     for (i = 0; i < (NUM_DMA_BUFFERS - 1); i++) {
//         alLink(&gAudDmaBuffs[i + 1].node, &gAudDmaBuffs[i].node);
//         gAudDmaBuffs[i].ptr = alHeapAlloc(c.heap, 1, 0x800);
//     }
//     /* last buffer already linked, but still needs buffer */
//     gAudDmaBuffs[i].ptr = alHeapAlloc(c.heap, 1, 0x800);
    
//     gAudioSched = sched;
//     osCreateMesgQueue(&__am_audioReplyMsgQ, __am_audioReplyMsgBuf, MAX_MESGS);
//     osCreateMesgQueue(&__am_audioFrameMsgQ, __am_audioFrameMsgBuf, MAX_MESGS);
//     osCreateMesgQueue(&audDMAMessageQ, audDMAMessageBuf, NUM_DMA_MESSAGES);
//     osCreateThread(&gAudioThread, 4, __amMain, NULL, 
//         &gAudioThreadStack[STACKSIZE(AUDIO_THREAD_STACK_SIZE)], threadPriority);
    
//     speaker_set_mode(4);
//     speaker_func_80063bb4(0, 4);
//     speaker_func_80063bb4(1, 4);
// }
