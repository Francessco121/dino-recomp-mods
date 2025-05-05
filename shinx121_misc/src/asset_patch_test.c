// #include "game/objects/object.h"
// #include "game/objects/object_def.h"
// #include "game/objects/object_id.h"
// #include "recomputils.h"
// #include "modding.h"
// #include "mod_fs.h"

// #include "mod_common.h"
// #include "sys/fs.h"

// // extern void bcopy_recomp(const void *src, void *dst, int length);
// // #define bcopy bcopy_recomp

// // extern void bzero_recomp(void *, int);
// // #define bzero bzero_recomp

// // extern void free_recomp(void* p);
// // #define free free_recomp

// INCBIN(krystalModelPatch, "assets/models0 - Krystal eye fix.bin");
// INCBIN(villagePatch, "assets/blocks0643 - LightFoot Village LOD.bin");

// typedef struct {
// /*00*/  s16 gridSizeX;
// /*02*/  s16 gridSizeZ;
// /*04*/  s16 originOffsetX; // in grid units
// /*06*/  s16 originOffsetZ; // in grid units
// /*08*/  u16 objectInstancesFileLength;
// /*0a*/  u16 unkA;
// /*0c*/  u32 *blockIDs_ptr;
// /*10*/  s8 *end_ptr;
// /*14*/  s8 *grid_A1_ptr;
// /*18*/  s8 unk18;
// /*19*/  s8 unk19;
// /*1a*/  s8 unk1a;
// /*1b*/  s8 unk1b;
// /*1c*/  s16 objectInstanceCount; //malloc-related
// /*1e*/  s16 gridB_sixteenthSize; //malloc-related ((size_of_gridB)/16 (rounded up and ignoring zero padding?)
// /*20*/  s32 *objectInstanceFile_ptr;
// /*24*/  f32 originWorldX;
// /*28*/  f32 originWorldZ;
// /*2c*/  s8 *grid_B1_ptr;
// /*30*/  s8 *grid_A2_ptr;
// /*34*/  s8 *grid_B2_ptr;
// } MAPSHeader;

// MODFS_ON_INIT_CALLBACK void my_mod_fs_init_callback() {
//     register_model_replacement(0, krystalModelPatch, krystalModelPatch_end - krystalModelPatch);
//     register_block_replacement(643, villagePatch, villagePatch_end - villagePatch);

//     ObjDef *sabreObjDef = get_handle(OBJECTS_TAB, 1, NULL);
//     recomp_printf("%s %x %x\n", sabreObjDef->name, (u32)sabreObjDef, (u32)sabreObjDef->pModelList);
//     u32 *sabreModelList = (u32*)((u32)sabreObjDef + (u32)sabreObjDef->pModelList);
//     sabreModelList[0] = 9;
//     sabreModelList[1] = 9;

//     u32 capeClawObjects = (29 * 7) + 4;
//     u32 capeClawObjectsSize;
//     ObjCreateInfo *capeClawObjInfos = get_handle(MAPS_TAB, capeClawObjects, &capeClawObjectsSize);
//     //ObjCreateInfo *infoprev = info;
//    // u32 end = (u32)info + capeClawObjectsSize;

//     MAPSHeader *header = get_handle(MAPS_TAB, 29 * 7, NULL);

//     //ObjCreateInfo *newinfos = recomp_alloc(header->objectInstanceCount * sizeof(ObjCreateInfo));

//     //recomp_printf("%x < %x\n", (u32)info, end);
//     ObjCreateInfo *info = capeClawObjInfos;
//     for (s32 i = 0; i < header->objectInstanceCount; i++) {
//         if (info->objId == OBJ_ClubSharpClaw) {
//             recomp_printf("[%d] %x: %f,%f,%f\n", i, info->objId, info->x, info->y, info->z);
//             recomp_printf("  unk2: %d\n", info->unk2);
//             recomp_printf("  unk3: %d\n", info->unk3);
//             recomp_printf("  unk4: %d\n", info->unk4);
//             recomp_printf("  unk5: %d\n", info->unk5);
//             recomp_printf("  unk6: %d\n", info->unk6);
//             recomp_printf("  unk7: %d\n", info->unk7);
//             recomp_printf("  unk14: %d\n", info->unk14);

//             if (info->unk2 != 0) {
//                 for (int k = 0x14; k < (info->unk2 << 2); k++) {
//                     recomp_printf("%x ", ((u8*)info)[k]);
//                 }
//                 recomp_printf("\n");
//             }
//         }

//         info = (ObjCreateInfo*)((u32)info + (info->unk2 << 2));
//        // if (info->objId == OBJ_ClubSharpClaw || info->objId == OBJ_TriggerPoint) {
//             //recomp_printf("%x %x\n", info->objId, info->unk2 * 4);
//                 //info->objId = OBJ_DummyObject;
//            // recomp_printf("[%d] %x: %f,%f,%f\n", count, info->objId, info->x, info->y, info->z);

//             //recomp_printf("  unk3: %d\n", info->unk3);
//            //recomp_printf("  unk4: %d\n", info->unk4);
//            // recomp_printf("  unk5: %d\n", info->unk5);
//            // recomp_printf("  unk6: %d\n", info->unk6);
//            // recomp_printf("  unk7: %d\n", info->unk7);
//           //  recomp_printf("  unk14: %d\n", info->unk14);

//        // }
//       //  info->objId = OBJ_DummyObject;
//       //  info->unk4 = 1;
//         // info->x = 347.064453f;
//         // info->y = 5.390846f;
//         // info->z = 959.935547f;

//         // ObjCreateInfo *info2 = &info[i];

//         // newinfos[i].objId = OBJ_DummyObject;
//         // newinfos[i].unk2 = sizeof(ObjCreateInfo) / 4;
//         // newinfos[i].unk3 = 0;
//         // newinfos[i].unk4 = info2->unk4; //1;
//         // newinfos[i].unk5 = 4;
//         // newinfos[i].unk6 = 0x80;
//         // newinfos[i].unk7 = 0x64;
//         // newinfos[i].x = info2->x;
//         // newinfos[i].y = info2->y;
//         // newinfos[i].z = info2->z;
//         // newinfos[i].unk14 = 273673;



//         // if (info->unk2 == 0 || (info->unk4 & 0x10) != 0) {
//         //     recomp_printf("idk man\n");
//         //     break;
//         // }
//         // info = (ObjCreateInfo*)((u32)info + (info->unk2 * 4));

//         //count++;
//     }

//     u8 extradata[] = {
//         0x0, 0x3 ,0x49 ,0xf3 ,0xff ,0xff ,0xff ,0xff ,0xff ,0xff ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 
//         ,0x0 ,0x3 ,0x0 ,0xe ,0x0 ,0x2 ,0x0 ,0x0 ,0xff ,0x1 ,0xff ,0xff ,0xe ,0x0 ,0x0 ,0x0 ,0x0 ,0x0};

//     ObjCreateInfo *obj = resize_tab_entry(MAPS_TAB, capeClawObjects, 0x50 * 1);
//     //bcopy(newinfos, objs, header->objectInstanceCount * sizeof(ObjCreateInfo));

//     for (s32 i = 0; i < 1; i++) {
//         obj->objId = OBJ_DIM_Boss;
//         obj->unk2 = 0x50 / 4;
//         obj->unk3 = 0;
//         obj->unk4 = 1;
//         obj->unk5 = 4;
//         obj->unk6 = 0x80;
//         obj->unk7 = 0x64;
//         obj->x = 347.064453f + (3 * i);
//         obj->y = 5.390846f + (60 * i);
//         obj->z = 959.935547f + (3 * i);
//         obj->unk14 = 0;

//         u8 *d = (u8*)((u32)&obj[i] + sizeof(ObjCreateInfo));
//         for (s32 k = 0; k < 36; k++) {
//             d[k] = extradata[k];
//         }

//         obj = (ObjCreateInfo*)((u32)obj + (obj->unk2 << 2));
//     }

//     header->objectInstanceCount = 1;
//     header->objectInstancesFileLength = 0x50 * 1;

//     // recomp_printf("count %d\n", count);

//     // info = resize_tab_entry(MAPS_TAB, capeClawObjects, capeClawObjectsSize + sizeof(ObjCreateInfo));
//     // //info = (ObjCreateInfo*)((u32)info + capeClawObjectsSize);
//     // //bcopy(info, info + sizeof(ObjCreateInfo), capeClawObjectsSize);
//     // for (s32 i = capeClawObjectsSize + sizeof(ObjCreateInfo) - 1; i >= 0; i--) {
//     //     *((u8*)info + sizeof(ObjCreateInfo) + i) = *((u8*)info + i);
//     // }
//     // info->objId = OBJ_DummyObject;
//     // info->unk2 = sizeof(ObjCreateInfo) / 4;
//     // info->unk3 = 0;
//     // info->unk4 = 1;
//     // info->unk5 = 4;
//     // info->unk6 = 0x80;
//     // info->unk7 = 0x64;
//     // info->x = 347.064453f;
//     // info->y = 5.390846f;
//     // info->z = 959.935547f;
//     // info->unk14 = 273673;

//    // MAPSHeader *header = get_handle(MAPS_TAB, 29 * 7, NULL);
//     //header->objectInstanceCount++;
//    // header->objectInstancesFileLength += sizeof(ObjCreateInfo);

//    // ObjCreateInfo *objs = get_handle(MAPS_TAB, capeClawObjects, &capeClawObjectsSize);
//     //ObjCreateInfo *objs = resize_tab_entry(MAPS_TAB, capeClawObjects, header->objectInstanceCount * sizeof(ObjCreateInfo));

//     // for (s32 i = 0; i < header->objectInstanceCount; i++) {
//     //     ObjCreateInfo *info = &objs[i];

//     // }
// }