// #include "PR/ultratypes.h"
// #include "game/objects/object_id.h"
// #include "game/objects/object.h"
// #include "recomputils.h"
// #include "sys/fs.h"
// #include "sys/map.h"
// #include "sys/map_enums.h"

// #include "extfs.h"

// #include "mod_common.h"

// INCBIN(krystalModelPatch, "assets/models0 - Krystal eye fix.bin");
// INCBIN(villagePatch, "assets/blocks0643 - LightFoot Village LOD.bin");
// INCBIN(shHits0643Patch, "assets/hits0643.bin");

// EXTFS_ON_LOAD_REPLACEMENTS_CALLBACK void my_extfs_replacements() {
//     extfs_models_set_replacement(0, krystalModelPatch, krystalModelPatch_end - krystalModelPatch);
//     extfs_blocks_set_replacement(643, villagePatch, villagePatch_end - villagePatch);
//     extfs_hits_set_replacement(643, shHits0643Patch, shHits0643Patch_end - shHits0643Patch);
// }

// static void cc_log_patch() {
//     MapHeader *header = extfs_maps_get(MAP_CAPE_CLAW, 0, NULL);
//     void *objects = extfs_maps_get(MAP_CAPE_CLAW, 4, NULL);

//     ObjCreateInfo *objInfo = (ObjCreateInfo*)objects;

//     for (s32 i = 0; i < header->objectInstanceCount; i++) {
//         if (objInfo->uID == 11480) {
//             // Swap DFlog with BWLog
//             objInfo->objId = OBJ_BWLog;
//         }

//         objInfo = (ObjCreateInfo*)((u32)objInfo + (objInfo->quarterSize << 2));
//     }
// }

// static void sh_hits_patch() {
//     // Adjust hit line on the upper right of the path leading to diamond bay
//     u32 hitsSize;
//     HitsLine *lines = extfs_hits_get(349, &hitsSize);
//     lines[7].Ax = 0;
//     lines[7].Ay = -725;
//     lines[7].Az = 281;
//     lines[7].Bx = 98;
//     lines[7].By = -732;
//     lines[7].Bz = 178;

//     // Add new line to the upper right of the path leading to diamond bay preventing the player from walking off the ledge
//     hitsSize += sizeof(HitsLine);
//     lines = extfs_hits_resize(349, hitsSize);
//     HitsLine *newLine = &lines[(hitsSize / sizeof(HitsLine)) - 1];
//     newLine->Ax = 0;
//     newLine->Ay = -726;
//     newLine->Az = 254;
//     newLine->Bx = 0;
//     newLine->By = -725;
//     newLine->Bz = 281;
//     newLine->heightA = 40;
//     newLine->heightB = 40;
//     newLine->settingsA = 0x0;
//     newLine->settingsB = 0x82;
//     newLine->animatorID = -1;
// }

// EXTFS_ON_LOAD_MODIFICATIONS_CALLBACK void my_extfs_modifications() {
//     ObjDef *sabreObjDef = extfs_objects_get(1, NULL);
//     u32 *sabreModelList = (u32*)((u32)sabreObjDef + (u32)sabreObjDef->pModelList);
//     sabreModelList[0] = 9;
//     sabreModelList[1] = 9;

//     cc_log_patch();
//     sh_hits_patch();
// }
